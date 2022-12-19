/** @file
  SPMI Table Generator

  Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
  Copyright (c) 2022, ARM Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.5 Specification

  @par Glossary:
  - Cm or CM   - Configuration Manager
  - Obj or OBJ - Object
**/

#include <Library/AcpiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/AcpiTable.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/TableHelperLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <IndustryStandard/ServiceProcessorManagementInterfaceTable.h>

/**
  ARM standard SPMI Generator

  Requirements:
    The following Configuration Manager Object(s) are used by this Generator:
    - EStdObjIpmiDeviceInfo

*/

/** This macro expands to a function that retrieves the IPMI Device
    information from the Configuration Manager.
**/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjIpmiDeviceInfo,
  CM_STD_IPMI_DEVICE_INFO
  )

/** Construct the SPMI ACPI table.

  Called by the Dynamic Table Manager, this function invokes the
  Configuration Manager protocol interface to get the required hardware
  information for generating the ACPI table.

  If this function allocates any resources then they must be freed
  in the FreeSpmiTableResources function.

  @param [in]  This           Pointer to the table generator.
  @param [in]  AcpiTableInfo  Pointer to the ACPI Table Info.
  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.
  @param [out] Table          Pointer to the constructed ACPI Table.

  @retval EFI_SUCCESS           Table generated successfully.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
  @retval EFI_OUT_OF_RESOURCES  Memory allocation failed.
**/
STATIC
EFI_STATUS
EFIAPI
BuildSpmiTable (
  IN  CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN  CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  EFI_STATUS                                             Status;
  UINT32                                                 TableSize;
  CM_STD_IPMI_DEVICE_INFO                                *IpmiInfo;
  EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE  *Spmi;

  ASSERT (
    (This != NULL) &&
    (AcpiTableInfo != NULL) &&
    (CfgMgrProtocol != NULL) &&
    (Table != NULL) &&
    (AcpiTableInfo->TableGeneratorId == This->GeneratorID) &&
    (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature)
    );

  if ((AcpiTableInfo->AcpiTableRevision < This->MinAcpiTableRevision) ||
      (AcpiTableInfo->AcpiTableRevision > This->AcpiTableRevision))
  {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SPMI: Requested table revision = %d is not supported. "
      "Supported table revisions: Minimum = %d. Maximum = %d\n",
      AcpiTableInfo->AcpiTableRevision,
      This->MinAcpiTableRevision,
      This->AcpiTableRevision
      ));
    return EFI_INVALID_PARAMETER;
  }

  *Table = NULL;

  Status = GetEStdObjIpmiDeviceInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &IpmiInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to get Ipmi Device CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  // Calculate the size of the SPMI table
  TableSize = sizeof (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE);

  // Allocate the Buffer for SPMI table
  *Table = (EFI_ACPI_DESCRIPTION_HEADER *)AllocateZeroPool (TableSize);
  if (*Table == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SPMI: Failed to allocate memory for SPMI Table, Size = %d," \
      " Status = %r\n",
      TableSize,
      Status
      ));
    return EFI_OUT_OF_RESOURCES;
  }

  Spmi = (EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE *)*Table;

  Status = AddAcpiHeader (
             CfgMgrProtocol,
             This,
             &Spmi->Header,
             AcpiTableInfo,
             TableSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SPMI: Failed to add ACPI header. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Setup the Reserved fields
  Spmi->Reserved1 = 0x01;  // Must always be 01h for backward compatibility
  Spmi->Reserved2 = EFI_ACPI_RESERVED_BYTE;
  Spmi->Reserved3 = EFI_ACPI_RESERVED_BYTE;

  Spmi->InterfaceType         = IpmiInfo->IpmiIntfType;
  Spmi->SpecificationRevision = IpmiInfo->IpmiSpecRevision;

  switch (Spmi->InterfaceType) {
    case EFI_ACPI_IPMI_INTERFACE_TYPE_SSIF:
      Spmi->InterruptType         = 0x00; // Use 0b for SSIF
      Spmi->Gpe                   = 0x00; // Only valid if InterruptType is set.
      Spmi->PciDeviceFlag         = 0x00; // Set to 0b for SSIF
      Spmi->GlobalSystemInterrupt = 0x00; // Only valid if InterruptType is set.
      Spmi->DeviceId.Uid          = IpmiInfo->IpmiUid;

      Spmi->BaseAddress.AddressSpaceId    = EFI_ACPI_3_0_SMBUS;
      Spmi->BaseAddress.RegisterBitWidth  = 0;
      Spmi->BaseAddress.RegisterBitOffset = 0;
      Spmi->BaseAddress.AccessSize        = EFI_ACPI_3_0_BYTE;
      Spmi->BaseAddress.Address           = IpmiInfo->IpmiI2CSlaveAddress;
      break;

    default:
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: SPMI: Ipmi interface type %d unsupported\n",
        Spmi->InterfaceType
        ));
      Status = EFI_UNSUPPORTED;
      goto error_handler;
  }

  return EFI_SUCCESS;

error_handler:

  if (*Table != NULL) {
    FreePool (*Table);
    *Table = NULL;
  }

  return Status;
}

/** Free any resources allocated for constructing the SPMI.

  @param [in]      This           Pointer to the table generator.
  @param [in]      AcpiTableInfo  Pointer to the ACPI Table Info.
  @param [in]      CfgMgrProtocol Pointer to the Configuration Manager
                                  Protocol Interface.
  @param [in, out] Table          Pointer to the ACPI Table.

  @retval EFI_SUCCESS           The resources were freed successfully.
  @retval EFI_INVALID_PARAMETER The table pointer is NULL or invalid.
**/
STATIC
EFI_STATUS
FreeSpmiTableResources (
  IN      CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN      CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN OUT        EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  ASSERT (
    (This != NULL) &&
    (AcpiTableInfo != NULL) &&
    (CfgMgrProtocol != NULL) &&
    (AcpiTableInfo->TableGeneratorId == This->GeneratorID) &&
    (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature)
    );

  if ((Table == NULL) || (*Table == NULL)) {
    DEBUG ((DEBUG_ERROR, "ERROR: SPMI: Invalid Table Pointer\n"));
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;

  return EFI_SUCCESS;
}

/** The SPMI Table Generator revision.
*/
#define SPMI_GENERATOR_REVISION  CREATE_REVISION (1, 0)

/** The interface for the SPMI Table Generator.
*/
STATIC
CONST
ACPI_TABLE_GENERATOR  SpmiGenerator = {
  // Generator ID
  CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdSpmi),
  // Generator Description
  L"ACPI.STD.SPMI.GENERATOR",
  // ACPI Table Signature
  EFI_ACPI_6_4_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE,
  // ACPI Table Revision supported by this Generator
  EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION,
  // Minimum supported ACPI Table Revision
  EFI_ACPI_SERVICE_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION,
  // Creator ID
  TABLE_GENERATOR_CREATOR_ID_ARM,
  // Creator Revision
  SPMI_GENERATOR_REVISION,
  // Build Table function
  BuildSpmiTable,
  // Free Resource function
  FreeSpmiTableResources,
  // Extended build function not needed
  NULL,
  // Extended build function not implemented by the generator.
  // Hence extended free resource function is not required.
  NULL
};

/** Register the Generator with the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
AcpiSpmiLibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterAcpiTableGenerator (&SpmiGenerator);
  DEBUG ((DEBUG_INFO, "SPMI: Register Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/** Deregister the Generator from the ACPI Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
AcpiSpmiLibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterAcpiTableGenerator (&SpmiGenerator);
  DEBUG ((DEBUG_INFO, "SPMI: Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
