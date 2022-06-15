/** @file
  SLIT Table Generator

  Copyright (c) 2019 - 2020, Arm Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.3 Specification, January 2019

  @par Glossary:
  - Cm or CM   - Configuration Manager
  - Obj or OBJ - Object
**/


#include <IndustryStandard/Slit.h>
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

/**
  ARM standard SLIT Generator

  Requirements:
    The following Configuration Manager Object(s) are used by this Generator:
    - EArmObjSystemLocalityInfo (Required)
*/

/**
  This macro expands to a function that retrieves the Relative Distance
  information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjSystemLocalityInfo,
  CM_ARM_SYSTEM_LOCALITY_INFO
  );

/** Construct the SLIT ACPI table.

  Called by the Dynamic Table Manager, this function invokes the
  Configuration Manager protocol interface to get the required hardware
  information for generating the ACPI table.

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

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
BuildSlitTable (
  IN  CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN  CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  EFI_STATUS  Status;
  UINT32      TableSize;
  UINT32      SlitObjectCount;
  UINT32      SlitObjectOffset;
  UINT64      NumSysLocalities;
  UINT8       *RelDistInfo;

  CM_ARM_SYSTEM_LOCALITY_INFO                        *SysLocalityInfo;
  EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE     *Slit;

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
      "ERROR: SLIT: Requested table revision = %d is not supported. "
      "Supported table revisions: Minimum = %d. Maximum = %d\n",
      AcpiTableInfo->AcpiTableRevision,
      This->MinAcpiTableRevision,
      This->AcpiTableRevision
      ));
    return EFI_INVALID_PARAMETER;
  }

  *Table = NULL;

  Status = GetEArmObjSystemLocalityInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &SysLocalityInfo,
             &SlitObjectCount
             );
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SLIT: Failed to get Relative Distance Info. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Calculate the size of the SLIT table
  TableSize = sizeof (EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE);

  // Number of System Localities
  NumSysLocalities = SysLocalityInfo->NumSystemLocalities;

  if (SlitObjectCount != 0) {
    SlitObjectOffset = TableSize;
    TableSize   += (sizeof (UINT8) * NumSysLocalities * NumSysLocalities);
  }

  // Allocate the Buffer for SLIT table
  *Table = (EFI_ACPI_DESCRIPTION_HEADER *)AllocateZeroPool (TableSize);
  if (*Table == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SLIT: Failed to allocate memory for SLIT Table, Size = %d," \
      " Status = %r\n",
      TableSize,
      Status
      ));
    goto error_handler;
  }

  Slit = (EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE *)*Table;

  DEBUG ((
    DEBUG_INFO,
    "SLIT: Slit = 0x%p TableSize = 0x%x\n",
    Slit,
    TableSize
    ));

  Status = AddAcpiHeader (
             CfgMgrProtocol,
             This,
             &Slit->Header,
             AcpiTableInfo,
             TableSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: SLIT: Failed to add ACPI header. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Number of System Localities
  Slit->NumLocalities = NumSysLocalities;

  // Offset to place the Relative Distance Info
  RelDistInfo = (UINT8*)((UINT8*)Slit + SlitObjectOffset);

  // Adding Relative distance in SLIT Table
  for (UINT64 Index = 0; Index < Slit->NumLocalities * Slit->NumLocalities; Index++) {
    *RelDistInfo  = SysLocalityInfo->Distance[Index];
    // Next
    RelDistInfo++;
  }// while

  return Status;

error_handler:

  if (*Table != NULL) {
    FreePool (*Table);
    *Table = NULL;
  }

  return Status;
}

/** Free any resources allocated for constructing the SLIT.

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
FreeSlitTableResources (
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
    DEBUG ((DEBUG_ERROR, "ERROR: SLIT: Invalid Table Pointer\n"));
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;
  return EFI_SUCCESS;
}

/** The SLIT Table Generator revision.
*/
#define SLIT_GENERATOR_REVISION  CREATE_REVISION (1, 0)

/** The interface for the SLIT Table Generator.
*/
STATIC
CONST
ACPI_TABLE_GENERATOR  SlitGenerator = {
  // Generator ID
  CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdSlit),
  // Generator Description
  L"ACPI.STD.SLIT.GENERATOR",
  // ACPI Table Signature
  EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE,
  // ACPI Table Revision supported by this Generator
  EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE_REVISION,
  // Minimum supported ACPI Table Revision
  EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE_REVISION,
  // Creator ID
  TABLE_GENERATOR_CREATOR_ID_ARM,
  // Creator Revision
  SLIT_GENERATOR_REVISION,
  // Build Table function
  BuildSlitTable,
  // Free Resource function
  FreeSlitTableResources,
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
AcpiSlitLibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterAcpiTableGenerator (&SlitGenerator);
  DEBUG ((DEBUG_INFO, "SLIT: Register Generator. Status = %r\n", Status));
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
AcpiSlitLibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterAcpiTableGenerator (&SlitGenerator);
  DEBUG ((DEBUG_INFO, "SLIT: Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
