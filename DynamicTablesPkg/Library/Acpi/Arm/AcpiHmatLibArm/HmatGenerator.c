/** @file
  HMAT Table Generator

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

/**
  ARM standard HMAT Generator

  Requirements:
    The following Configuration Manager Object(s) are used by this Generator:
    - EArmObjLocalityLatencyBandwidthInfo

*/

/** This macro expands to a function that retrieves the
 * Locality Latency and Bandwidth Information from the
 * Configuration Manager.
*/

GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjLocalityLatencyBandwidthInfo,
  CM_ARM_LOCALITY_LATENCY_BANDWIDTH_INFO
  );

STATIC
UINT64
GetSizeofThisLatencyBandwidthStructure (
  IN CONST   CM_ARM_LOCALITY_LATENCY_BANDWIDTH_INFO  *LatencyBandwidthInfo,
  IN         UINT32                                  Indexer
  )
{
  UINT64  Size;

  Size = sizeof (EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO) +
         sizeof (UINT32) * LatencyBandwidthInfo[Indexer].NumInitiatorProximityDomains +
         sizeof (UINT32) * LatencyBandwidthInfo[Indexer].NumTargetProximityDomains +
         sizeof (UINT16) * LatencyBandwidthInfo[Indexer].NumInitiatorProximityDomains *
         LatencyBandwidthInfo[Indexer].NumTargetProximityDomains;

  return Size;
}

STATIC
UINT32
GetSizeofAllLatencyBandwidthStructures (
  IN CONST   CM_ARM_LOCALITY_LATENCY_BANDWIDTH_INFO  *LatencyBandwidthInfo,
  IN         UINT32                                  LatencyBandwidthInfoCount
  )
{
  UINT32  Size;
  UINT32  Indexer;

  Size    = 0;
  Indexer = 0;

  while (Indexer < LatencyBandwidthInfoCount) {
    Size += GetSizeofThisLatencyBandwidthStructure (LatencyBandwidthInfo, Indexer);
    Indexer++;
  }

  return Size;
}

STATIC
EFI_STATUS
AddLatencyBandwidthInfoStructures (
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL                                       *CONST  CfgMgrProtocol,
  IN      CONST EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER                           *Hmat,
  IN      CONST UINT32                                                                             LatencyBandwidthInfoOffset,
  IN      CONST CM_ARM_LOCALITY_LATENCY_BANDWIDTH_INFO                                             *LatencyBandwidthInfo,
  IN            UINT32                                                                             LatencyBandwidthInfoCount
  )
{
  EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO  *InfoStruct;
  UINT32                                                                  *ProximityDomainValue;
  UINT16                                                                  *LatencyBandwidthValue;
  UINT32                                                                  DomainIndex;
  UINT32                                                                  ValIndex;

  ASSERT (Hmat != NULL);
  UINT32  Indexer;

  Indexer = 0;

  InfoStruct = (EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO *)((UINT8 *)Hmat + LatencyBandwidthInfoOffset);

  while (Indexer < LatencyBandwidthInfoCount) {
    InfoStruct->Type                              = (UINT16)(EFI_ACPI_6_4_HMAT_TYPE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO);
    InfoStruct->Reserved[0]                       = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->Reserved[1]                       = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->Length                            = (UINT32)(GetSizeofThisLatencyBandwidthStructure (LatencyBandwidthInfo, Indexer));
    InfoStruct->Flags                             = LatencyBandwidthInfo[Indexer].Flags;
    InfoStruct->DataType                          = LatencyBandwidthInfo[Indexer].DataType;
    InfoStruct->MinTransferSize                   = LatencyBandwidthInfo[Indexer].MinTransferSize;
    InfoStruct->Reserved1                         = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->NumberOfInitiatorProximityDomains = LatencyBandwidthInfo[Indexer].NumInitiatorProximityDomains;
    InfoStruct->NumberOfTargetProximityDomains    = LatencyBandwidthInfo[Indexer].NumTargetProximityDomains;
    InfoStruct->Reserved2[0]                      = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->Reserved2[1]                      = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->Reserved2[2]                      = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->Reserved2[3]                      = EFI_ACPI_RESERVED_BYTE;
    InfoStruct->EntryBaseUnit                     = LatencyBandwidthInfo[Indexer].EntryBaseUnit;

    // assigning proximity domain lists
    ProximityDomainValue = (UINT32 *)((UINT8 *)InfoStruct + sizeof (EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO));
    for (DomainIndex = 0; DomainIndex < InfoStruct->NumberOfInitiatorProximityDomains; DomainIndex++) {
      *ProximityDomainValue = LatencyBandwidthInfo[Indexer].InitiatorProximityDomainList[DomainIndex];
      ProximityDomainValue++;
    }

    for (DomainIndex = 0; DomainIndex < InfoStruct->NumberOfTargetProximityDomains; DomainIndex++) {
      *ProximityDomainValue = LatencyBandwidthInfo[Indexer].TargetProximityDomainList[DomainIndex];
      ProximityDomainValue++;
    }

    // assigning latency or bandwidth values
    LatencyBandwidthValue = (UINT16 *)((UINT8 *)InfoStruct + sizeof (EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO) +
                                       sizeof (UINT32) * LatencyBandwidthInfo[Indexer].NumInitiatorProximityDomains +
                                       sizeof (UINT32) * LatencyBandwidthInfo[Indexer].NumTargetProximityDomains);
    for (ValIndex = 0; ValIndex < InfoStruct->NumberOfInitiatorProximityDomains * InfoStruct->NumberOfTargetProximityDomains; ValIndex++) {
      *LatencyBandwidthValue = LatencyBandwidthInfo[Indexer].LatencyBandwidthEntries[ValIndex];
      LatencyBandwidthValue++;
    }

    // Next InfoStruct
    InfoStruct = (EFI_ACPI_6_4_HMAT_STRUCTURE_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO *)((UINT8 *)InfoStruct + InfoStruct->Length);
    Indexer++;
  }

  return EFI_SUCCESS;
}

/** Construct the HMAT ACPI table.

  Called by the Dynamic Table Manager, this function invokes the
  Configuration Manager protocol interface to get the required hardware
  information for generating the ACPI table.

  If this function allocates any resources then they must be freed
  in the FreeHmatTableResources function.

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
BuildHmatTable (
  IN  CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN  CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  EFI_STATUS  Status;
  UINT32      TableSize;

  CM_ARM_LOCALITY_LATENCY_BANDWIDTH_INFO  *LatencyBandwidthInfo;
  UINT32                                  LatencyBandwidthInfoCount;
  UINT32                                  LatencyBandwidthInfoOffset;
  UINT32                                  LatencyBandwithInfoStructsSize;

  EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER  *Hmat;

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
      "ERROR: HMAT: Requested table revision = %d is not supported. "
      "Supported table revisions: Minimum = %d. Maximum = %d\n",
      AcpiTableInfo->AcpiTableRevision,
      This->MinAcpiTableRevision,
      This->AcpiTableRevision
      ));
    return EFI_INVALID_PARAMETER;
  }

  *Table = NULL;

  Status = GetEArmObjLocalityLatencyBandwidthInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &LatencyBandwidthInfo,
             &LatencyBandwidthInfoCount
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: HMAT: Failed to get Latency and Bandwidth Info. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  if (LatencyBandwidthInfoCount == 0) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: HMAT: Latency Bandwidth information not provided.\n"
      ));
    ASSERT (0);
    Status = EFI_INVALID_PARAMETER;
    goto error_handler;
  }

  // Calculate the size of the HMAT table header
  TableSize = sizeof (EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER);

  // Include the size of Latency and Bandwidth info structures
  if (LatencyBandwidthInfoCount != 0) {
    LatencyBandwidthInfoOffset = TableSize;
    // Size of Latency Bandwidth structures.
    LatencyBandwithInfoStructsSize = GetSizeofAllLatencyBandwidthStructures (
                                       LatencyBandwidthInfo,
                                       LatencyBandwidthInfoCount
                                       );
  } else {
    LatencyBandwithInfoStructsSize = 0;
  }

  if (LatencyBandwithInfoStructsSize > MAX_UINT32) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR:HMAT: Invalid Size of Latency Bandwidth Info structures. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  TableSize += LatencyBandwithInfoStructsSize;

  // Allocate the Buffer for HMAT table
  *Table = (EFI_ACPI_DESCRIPTION_HEADER *)AllocateZeroPool (TableSize);
  if (*Table == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: HMAT: Failed to allocate memory for HMAT Table, Size = %d," \
      " Status = %r\n",
      TableSize,
      Status
      ));
    goto error_handler;
  }

  Hmat = (EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_HEADER *)*Table;

  DEBUG ((
    DEBUG_INFO,
    "HMAT: Hmat = 0x%p TableSize = 0x%x\n",
    Hmat,
    TableSize
    ));

  Status = AddAcpiHeader (
             CfgMgrProtocol,
             This,
             &Hmat->Header,
             AcpiTableInfo,
             TableSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: HMAT: Failed to add ACPI header. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Setup the Reserved fields
  Hmat->Reserved[0] = EFI_ACPI_RESERVED_BYTE;
  Hmat->Reserved[1] = EFI_ACPI_RESERVED_BYTE;
  Hmat->Reserved[2] = EFI_ACPI_RESERVED_BYTE;
  Hmat->Reserved[3] = EFI_ACPI_RESERVED_BYTE;

  if (LatencyBandwidthInfoCount != 0) {
    Status = AddLatencyBandwidthInfoStructures (
               CfgMgrProtocol,
               Hmat,
               LatencyBandwidthInfoOffset,
               LatencyBandwidthInfo,
               LatencyBandwidthInfoCount
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: HMAT: Failed to add Latency Bandwith Info structures. Status = %r\n",
        Status
        ));
      goto error_handler;
    }
  }

  return Status;

error_handler:

  if (*Table != NULL) {
    FreePool (*Table);
    *Table = NULL;
  }

  return Status;
}

/** Free any resources allocated for constructing the HMAT.

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
FreeHmatTableResources (
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
    DEBUG ((DEBUG_ERROR, "ERROR: HMAT: Invalid Table Pointer\n"));
    ASSERT (0);
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;

  return EFI_SUCCESS;
}

/** The HMAT Table Generator revision.
*/
#define HMAT_GENERATOR_REVISION  CREATE_REVISION (1, 0)

/** The interface for the HMAT Table Generator.
*/
STATIC
CONST
ACPI_TABLE_GENERATOR  HmatGenerator = {
  // Generator ID
  CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdHmat),
  // Generator Description
  L"ACPI.STD.HMAT.GENERATOR",
  // ACPI Table Signature
  EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_SIGNATURE,
  // ACPI Table Revision supported by this Generator
  EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_REVISION,
  // Minimum supported ACPI Table Revision
  EFI_ACPI_6_4_HETEROGENEOUS_MEMORY_ATTRIBUTE_TABLE_REVISION,
  // Creator ID
  TABLE_GENERATOR_CREATOR_ID_ARM,
  // Creator Revision
  HMAT_GENERATOR_REVISION,
  // Build Table function
  BuildHmatTable,
  // Free Resource function
  FreeHmatTableResources,
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
AcpiHmatLibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterAcpiTableGenerator (&HmatGenerator);
  DEBUG ((DEBUG_INFO, "HMAT: Register Generator. Status = %r\n", Status));
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
AcpiHmatLibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterAcpiTableGenerator (&HmatGenerator);
  DEBUG ((DEBUG_INFO, "HMAT: Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
