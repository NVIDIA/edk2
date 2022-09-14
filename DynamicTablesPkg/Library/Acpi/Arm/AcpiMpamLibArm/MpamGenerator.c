/** @file
  MPAM Table Generator

  Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
  Copyright (c) 2022, ARM Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.4 Specification, January 2021

  @par Glossary:
  - Cm or CM   - Configuration Manager
  - Obj or OBJ - Object
**/

#include <IndustryStandard/Mpam.h>
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

#include "MpamGenerator.h"

/**
  ARM standard MPAM Generator

  Requirements:
    The following Configuration Manager Object(s) are used by this Generator:
    - EArmObjMscNodeInfo (REQUIRED)
    - EArmObjResNodeInfo
*/

/**
  This macro expands to a function that retrieves the MSC Node information
  from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjMscNodeInfo,
  CM_ARM_MSC_NODE_INFO
  );

/**
  This macro expands to a function that retrieves the Resource Node
  information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjResNodeInfo,
  CM_ARM_RESOURCE_NODE_INFO
  );

/**
  This macro expands to a function that retrieves the Functional Dependency List
  information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjFuncDepInfo,
  CM_ARM_FUNC_DEP_INFO
  );

/**
  Returns the size of the MPAM Memory System Controller (MSC) Node

  @param [in]  Node     Pointer to MSC Node Info CM object

  @param [in]  Node     Pointer to Resource Node Info CM object

  @param [in]  Node     Pointer to Resource Node List indexer

  @retval               Size of the MSC Node in bytes.
**/
STATIC
UINT64
GetMscNodeSize (
  IN  CONST CM_ARM_MSC_NODE_INFO       *MscNode,
  IN  CONST CM_ARM_RESOURCE_NODE_INFO  *ResourceNodeList,
  IN        UINT32                     *ResourceNodeIndexer
  )
{
  UINT64  MscNodeSize;
  UINT8   ResourceNodeCtr;

  ASSERT (MscNode != NULL);

  // <size of Memory System Controller Node>
  // Size of MSC node is size of header, plus size of all
  // resource nodes in this MSC node
  MscNodeSize = sizeof (EFI_ACPI_6_4_MPAM_MSC_NODE);

  if (MscNode->NumResourceNodes > 0) {
    ASSERT (ResourceNodeList != NULL);
  }

  // Loop for each Resource node
  for (ResourceNodeCtr = 0; ResourceNodeCtr < MscNode->NumResourceNodes; ResourceNodeCtr++) {
    // Add size of Func Dep List if any to the resource node size
    MscNodeSize += sizeof (EFI_ACPI_6_4_MPAM_RESOURCE_NODE) +
                   ResourceNodeList[*ResourceNodeIndexer].NumFuncDep * sizeof (EFI_ACPI_6_4_MPAM_FUNC_DEP_NODE);
    (*ResourceNodeIndexer)++;
  }

  return MscNodeSize;
}

/** Returns the total size required for the MSC and
    updates the Node Indexer.

    This function calculates the size required for the node group
    and also populates the Node Indexer array with offsets for the
    individual nodes.

    @param [in]       NodeStartOffset   Offset from the start of the
                                        MPAM where this node group starts.
    @param [in]       MscNodeList       Pointer to MSC Group node list.
    @param [in]       MscNodeCount      Count of the MSC Group nodes.
    @param [in]       ResourceNodeList  Pointer to MSC Group node list.
    @param [in]       ResourceNodeCount Count of the MSC Group nodes.
    @param [in]       FuncDepList       Pointer to Functional Dependency list.
    @param [in, out]  NodeIndexer       Pointer to the next Node Indexer.

    @retval Total size of the MSC Group Nodes.
**/
STATIC
UINT64
GetSizeofMscGroupNodes (
  IN      CONST UINT32                         NodeStartOffset,
  IN      CONST CM_ARM_MSC_NODE_INFO           *MscNodeList,
  IN            UINT32                         MscNodeCount,
  IN      CONST CM_ARM_RESOURCE_NODE_INFO      *ResourceNodeList,
  IN            UINT32                         ResourceNodeCount,
  IN      CONST CM_ARM_FUNC_DEP_INFO           *FuncDepList,
  IN OUT        MPAM_NODE_INDEXER     **CONST  NodeIndexer
  )
{
  UINT64  Size;
  UINT32  ResourceNodeIndexer;
  UINT64  MscNodeSize;

  ASSERT (MscNodeList != NULL);

  ResourceNodeIndexer = 0;
  MscNodeSize         = 0;

  Size = 0;
  while (MscNodeCount-- != 0) {
    (*NodeIndexer)->Token  = MscNodeList->Token;
    (*NodeIndexer)->Object = (VOID *)MscNodeList;
    (*NodeIndexer)->Offset = (UINT32)(Size + NodeStartOffset);
    DEBUG ((
      DEBUG_INFO,
      "MPAM: MSC Node Indexer = %p, Token = %p, Object = %p, Offset = 0x%x\n",
      *NodeIndexer,
      (*NodeIndexer)->Token,
      (*NodeIndexer)->Object,
      (*NodeIndexer)->Offset
      ));

    MscNodeSize = GetMscNodeSize (MscNodeList, ResourceNodeList, &ResourceNodeIndexer);
    Size       += MscNodeSize;

    (*NodeIndexer)++;
    MscNodeList++;
  }

  return Size;
}

/** Update the MSC Group Node Information.

    @param [in]     This             Pointer to the table Generator.
    @param [in]     CfgMgrProtocol   Pointer to the Configuration Manager
                                     Protocol Interface.
    @param [in]     Mpam             Pointer to MPAM table structure.
    @param [in]     NodesStartOffset Offset for the start of the Msc Group
                                     Nodes.
    @param [in]     MscNodeList      Pointer to an array of Msc Group Node
                                     Objects.
    @param [in]     NodeCount        Number of Msc Group Node Objects.

    @param [in]     ResourceNodeList Pointer to an array of Resource Nodes

    @param [in]     FuncDepList      Pointer to the Functional Dep list

    @retval EFI_SUCCESS           Table generated successfully.
    @retval EFI_INVALID_PARAMETER A parameter is invalid.

**/
STATIC
EFI_STATUS
AddMscNodes (
  IN      CONST ACPI_TABLE_GENERATOR                                                       *CONST  This,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL                                       *CONST  CfgMgrProtocol,
  IN      CONST EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_HEADER           *Mpam,
  IN      CONST UINT32                                                                             NodesStartOffset,
  IN      CONST CM_ARM_MSC_NODE_INFO                                                               *MscNodeList,
  IN            UINT32                                                                             MscNodeCount,
  IN      CONST CM_ARM_RESOURCE_NODE_INFO                                                          *ResourceNodeList,
  IN      CONST CM_ARM_FUNC_DEP_INFO                                                               *FuncDepList
  )
{
  EFI_STATUS                       Status;
  EFI_ACPI_6_4_MPAM_MSC_NODE       *MscNode;
  EFI_ACPI_6_4_MPAM_RESOURCE_NODE  *ResourceNodeArray;
  EFI_ACPI_6_4_MPAM_FUNC_DEP_NODE  *FuncDepArray;

  UINT64  NodeLength;
  UINT32  NumResNodesInMsc;
  UINT32  ResourceNodeIndexer;
  UINT32  NumFuncDepInRes;

  ASSERT (Mpam != NULL);
  ResourceNodeIndexer = 0;

  MscNode = (EFI_ACPI_6_4_MPAM_MSC_NODE *)((UINT8 *)Mpam + NodesStartOffset);

  while (MscNodeCount-- != 0) {
    NodeLength = GetMscNodeSize (MscNodeList, ResourceNodeList, &ResourceNodeIndexer);

    if (NodeLength > MAX_UINT16) {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: MPAM: MSC Node length 0x%lx > MAX_UINT16. Status = %r\n",
        NodeLength,
        Status
        ));
      return Status;
    }

    // Populate the node header
    MscNode->Length                   = (UINT16)NodeLength;
    MscNode->Reserved                 = EFI_ACPI_RESERVED_WORD;
    MscNode->Identifier               = MscNodeList->Identifier;
    MscNode->BaseAddress              = MscNodeList->BaseAddress;
    MscNode->MmioSize                 = MscNodeList->MmioSize;
    MscNode->OverflowInterrupt        = MscNodeList->OverflowInterrupt;
    MscNode->OverflowInterruptFlags   = MscNodeList->OverflowInterruptFlags;
    MscNode->Reserved1                = EFI_ACPI_RESERVED_DWORD;
    MscNode->OverflowInterruptAff     = MscNodeList->OverflowInterruptAff;
    MscNode->ErrorInterrupt           = MscNodeList->ErrorInterrupt;
    MscNode->ErrorInterruptFlags      = MscNodeList->ErrorInterruptFlags;
    MscNode->Reserved2                = EFI_ACPI_RESERVED_DWORD;
    MscNode->ErrorInterruptAff        = MscNodeList->ErrorInterruptAff;
    MscNode->MaxNRdyUsec              = MscNodeList->MaxNRdyUsec;
    MscNode->LinkedDeviceHwId         = MscNodeList->LinkedDeviceHwId;
    MscNode->LinkedDeviceInstanceHwId = MscNodeList->LinkedDeviceInstanceHwId;
    MscNode->NumResourceNodes         = MscNodeList->NumResourceNodes;

    // ResourceNode List for each MSC
    if (MscNode->NumResourceNodes > 0) {
      NumResNodesInMsc = MscNode->NumResourceNodes;

      // Resource Node array for this Msc node
      ResourceNodeArray = (EFI_ACPI_6_4_MPAM_RESOURCE_NODE *)((UINT8 *)MscNode + sizeof (EFI_ACPI_6_4_MPAM_MSC_NODE));

      // Adding Resource Node content
      while (NumResNodesInMsc-- != 0) {
        ResourceNodeArray->Identifier  = ResourceNodeList->Identifier;
        ResourceNodeArray->RisIndex    = ResourceNodeList->RisIndex;
        ResourceNodeArray->Reserved1   = EFI_ACPI_RESERVED_WORD;
        ResourceNodeArray->LocatorType = ResourceNodeList->LocatorType;
        ResourceNodeArray->Locator1    = ResourceNodeList->Locator1;
        ResourceNodeArray->Locator2    = ResourceNodeList->Locator2;
        ResourceNodeArray->NumFuncDep  = ResourceNodeList->NumFuncDep;

        if (ResourceNodeArray->NumFuncDep > 0) {
          ASSERT (FuncDepList != NULL);
        }

        // Functional Dependencies for each Resource Node
        NumFuncDepInRes = ResourceNodeArray->NumFuncDep;
        FuncDepArray    = (EFI_ACPI_6_4_MPAM_FUNC_DEP_NODE *)((UINT8 *)ResourceNodeArray + sizeof (EFI_ACPI_6_4_MPAM_RESOURCE_NODE));

        // Adding the Func Dep List content
        while (NumFuncDepInRes-- != 0) {
          FuncDepArray->Producer  = FuncDepList->Producer;
          FuncDepArray->Reserved1 = EFI_ACPI_RESERVED_DWORD;

          FuncDepList++;
          FuncDepArray++;
        }

        // Next Resource Node entry
        ResourceNodeArray = (EFI_ACPI_6_4_MPAM_RESOURCE_NODE *)((UINT8 *)ResourceNodeArray +
                                                                sizeof (EFI_ACPI_6_4_MPAM_RESOURCE_NODE) +
                                                                ResourceNodeArray->NumFuncDep * sizeof (EFI_ACPI_6_4_MPAM_FUNC_DEP_NODE));
        ResourceNodeList++;
        // Reset the ResourceNodeIndexer when ResourceNodeList is advanced
        ResourceNodeIndexer = 0;
      }
    }

    // Next MSC Node
    MscNode = (EFI_ACPI_6_4_MPAM_MSC_NODE *)((UINT8 *)MscNode + MscNode->Length);
    MscNodeList++;
  } // Msc Node

  return EFI_SUCCESS;
}

/**
  Construct the MPAM ACPI table.

  This function invokes the Configuration Manager protocol interface
  to get the required hardware information for generating the ACPI
  table.

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

  @param [in]  This                 Pointer to the table generator.
  @param [in]  AcpiTableInfo        Pointer to the ACPI table generator to be used.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol Interface.
  @param [out] Table                Pointer to the constructed ACPI Table.

  @retval EFI_SUCCESS               Table generated successfully.
  @retval EFI_INVALID_PARAMETER     A parameter is invalid.
  @retval EFI_NOT_FOUND             The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE       The size returned by the Configuration
                                    Manager is less than the Object size for
                                    the requested object.
**/
STATIC
EFI_STATUS
EFIAPI
BuildMpamTable (
  IN  CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN  CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  EFI_STATUS  Status;
  UINT64      TableSize;
  UINT64      NodeSize;
  UINT32      MpamNodeCount;

  UINT32  MscNodeCount;
  UINT32  MscNodeOffset;
  UINT32  ResourceNodeCount;
  UINT32  FuncDepCount;

  EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_HEADER  *Mpam;
  ACPI_MPAM_GENERATOR                                                       *Generator;
  CM_ARM_MSC_NODE_INFO                                                      *MscNodeList;
  CM_ARM_RESOURCE_NODE_INFO                                                 *ResourceNodeList;
  CM_ARM_FUNC_DEP_INFO                                                      *FuncDepList;
  MPAM_NODE_INDEXER                                                         *NodeIndexer;

  ASSERT (
    (This != NULL) &&
    (AcpiTableInfo != NULL) &&
    (CfgMgrProtocol != NULL) &&
    (Table != NULL) &&
    (AcpiTableInfo->TableGeneratorId == This->GeneratorID) &&
    (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature)
    );

  DEBUG ((
    DEBUG_INFO,
    "DEBUG PRINT: MPAM: Requested table revision = %d\n",
    AcpiTableInfo->AcpiTableRevision
    ));

  if ((AcpiTableInfo->AcpiTableRevision < This->MinAcpiTableRevision) ||
      (AcpiTableInfo->AcpiTableRevision > This->AcpiTableRevision))
  {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Requested table revision = %d is not supported. "
      "Supported table revisions: Minimum = %d. Maximum = %d\n",
      AcpiTableInfo->AcpiTableRevision,
      This->MinAcpiTableRevision,
      This->AcpiTableRevision
      ));
    return EFI_INVALID_PARAMETER;
  }

  Generator = (ACPI_MPAM_GENERATOR *)This;
  *Table    = NULL;

  // Get the Memory System Controller Node info to update the MPAM table
  Status = GetEArmObjMscNodeInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &MscNodeList,
             &MscNodeCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to get memory system controller node info. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  MpamNodeCount           = MscNodeCount;
  Generator->MscNodeCount = MscNodeCount;

  // Get the Resource Node info to update the MPAM MSC node
  Status = GetEArmObjResNodeInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &ResourceNodeList,
             &ResourceNodeCount
             );
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to get resource nodes info. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Get the Functional Dependency List info to update the Resource node
  Status = GetEArmObjFuncDepInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &FuncDepList,
             &FuncDepCount
             );
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to get functional dependecies info. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Allocate Node Indexer array
  NodeIndexer = (MPAM_NODE_INDEXER *)AllocateZeroPool (
                                       sizeof (MPAM_NODE_INDEXER) *
                                       MpamNodeCount
                                       );
  if (NodeIndexer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to allocate memory for Node Indexer. Status = %r\n ",
      Status
      ));
    goto error_handler;
  }

  DEBUG ((DEBUG_INFO, "MPAM INFO: NodeIndexer = %p\n", NodeIndexer));
  Generator->MpamNodeCount = MpamNodeCount;
  Generator->NodeIndexer   = NodeIndexer;

  // Calculate the size of the MPAM table
  TableSize = sizeof (EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_HEADER);

  // Include the size of MSC Nodes and index them
  if (Generator->MscNodeCount != 0) {
    MscNodeOffset = TableSize;
    // Size of MSC nodes.
    NodeSize = GetSizeofMscGroupNodes (
                 MscNodeOffset,
                 MscNodeList,
                 Generator->MscNodeCount,
                 ResourceNodeList,
                 ResourceNodeCount,
                 FuncDepList,
                 &NodeIndexer
                 );
    if (NodeSize > MAX_UINT32) {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: MPAM: Invalid Size of Group Nodes. Status = %r\n",
        Status
        ));
      goto error_handler;
    }

    TableSize += NodeSize;

    DEBUG ((
      DEBUG_INFO,
      " MscNodeCount = %d\n" \
      " MscNodeOffset = 0x%x\n",
      Generator->MscNodeCount,
      MscNodeOffset
      ));
  }

  DEBUG ((
    DEBUG_INFO,
    "INFO: MPAM:\n" \
    " MpamNodeCount = %d\n" \
    " TableSize = 0x%X\n",
    MpamNodeCount,
    TableSize
    ));

  if (TableSize > MAX_UINT32) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: MPAM Table Size 0x%lx > MAX_UINT32," \
      " Status = %r\n",
      TableSize,
      Status
      ));
    goto error_handler;
  }

  // Allocate the Buffer for the MPAM table
  *Table = (EFI_ACPI_DESCRIPTION_HEADER *)AllocateZeroPool (TableSize);
  if (*Table == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to allocate memory for MPAM Table. " \
      "Size = %d. Status = %r\n",
      TableSize,
      Status
      ));
    goto error_handler;
  }

  Mpam = (EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_HEADER *)*Table;

  DEBUG ((
    DEBUG_INFO,
    "MPAM: Mpam = 0x%p. TableSize = 0x%x\n",
    Mpam,
    TableSize
    ));

  // Add ACPI header
  Status = AddAcpiHeader (
             CfgMgrProtocol,
             This,
             &Mpam->Header,
             AcpiTableInfo,
             TableSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: MPAM: Failed to add ACPI header. Status = %r\n",
      Status
      ));
    goto error_handler;
  }

  // Add MSC Nodes to the generated table
  if (MscNodeCount != 0) {
    Status = AddMscNodes (
               This,
               CfgMgrProtocol,
               Mpam,
               MscNodeOffset,
               MscNodeList,
               MscNodeCount,
               ResourceNodeList,
               FuncDepList
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "ERROR: MPAM: Failed to add MSC Nodes. Status = %r\n",
        Status
        ));
      goto error_handler;
    }
  }

  return Status;

error_handler:
  if (Generator->NodeIndexer != NULL) {
    FreePool (Generator->NodeIndexer);
    Generator->NodeIndexer = NULL;
  }

  if (*Table != NULL) {
    FreePool (*Table);
    *Table = NULL;
  }

  return Status;
}

/** Free any resources allocated for constructing the MPAM Table

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
FreeMpamTableResources (
  IN      CONST ACPI_TABLE_GENERATOR                  *CONST  This,
  IN      CONST CM_STD_OBJ_ACPI_TABLE_INFO            *CONST  AcpiTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN OUT        EFI_ACPI_DESCRIPTION_HEADER          **CONST  Table
  )
{
  ACPI_MPAM_GENERATOR  *Generator;

  ASSERT (This != NULL);
  ASSERT (AcpiTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (AcpiTableInfo->TableGeneratorId == This->GeneratorID);
  ASSERT (AcpiTableInfo->AcpiTableSignature == This->AcpiTableSignature);

  Generator = (ACPI_MPAM_GENERATOR *)This;

  // Free any memory allocated by the generator
  if (Generator->NodeIndexer != NULL) {
    FreePool (Generator->NodeIndexer);
    Generator->NodeIndexer = NULL;
  }

  if ((Table == NULL) || (*Table == NULL)) {
    DEBUG ((DEBUG_ERROR, "ERROR: MPAM: Invalid Table Pointer\n"));
    return EFI_INVALID_PARAMETER;
  }

  FreePool (*Table);
  *Table = NULL;

  return EFI_SUCCESS;
}

/** The MPAM Table Generator revision.
*/
#define MPAM_GENERATOR_REVISION  CREATE_REVISION (1, 0)

/** The interface for the MPAM Table Generator.
*/
STATIC
ACPI_MPAM_GENERATOR  MpamGenerator = {
  // ACPI table generator header
  {
    // Generator ID
    CREATE_STD_ACPI_TABLE_GEN_ID (EStdAcpiTableIdMpam),
    // Generator Description
    L"ACPI.STD.MPAM.GENERATOR",
    // ACPI Table Signature
    EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_STRUCTURE_SIGNATURE,
    // ACPI Table Revision supported by this Generator
    EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_REVISION,
    // Minimum supported ACPI Table Revision
    EFI_ACPI_6_4_MEMORY_SYSTEM_RESOURCE_PARTITIONING_MONITORING_TABLE_REVISION,
    // Creator ID
    TABLE_GENERATOR_CREATOR_ID_ARM,
    // Creator Revision
    MPAM_GENERATOR_REVISION,
    // Build Table function
    BuildMpamTable,
    // Free Resource function
    FreeMpamTableResources,
    // Extended build function not needed
    NULL,
    // Extended build function not implemented by the generator.
    // Hence extended free resource function is not required.
    NULL
  },

  // MPAM Generator private data

  // MPAM node count
  0,
  // MSC node count
  0,

  // Pointer to MPAM Node Indexer
  NULL
};

/**
  Register the Generator with the ACPI Table Factory.

  @param [in]  ImageHandle        The handle to the image.
  @param [in]  SystemTable        Pointer to the System Table.

  @retval EFI_SUCCESS             The Generator is registered.
  @retval EFI_INVALID_PARAMETER   A parameter is invalid.
  @retval EFI_ALREADY_STARTED     The Generator for the Table ID
                                  is already registered.
**/
EFI_STATUS
EFIAPI
AcpiMpamLibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterAcpiTableGenerator (&MpamGenerator.Header);
  DEBUG ((DEBUG_INFO, "MPAM: Register Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Deregister the Generator from the ACPI Table Factory.

  @param [in]  ImageHandle        The handle to the image.
  @param [in]  SystemTable        Pointer to the System Table.

  @retval EFI_SUCCESS             The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER   A parameter is invalid.
  @retval EFI_NOT_FOUND           The Generator is not registered.
**/
EFI_STATUS
EFIAPI
AcpiMpamLibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterAcpiTableGenerator (&MpamGenerator.Header);
  DEBUG ((DEBUG_INFO, "MPAM: Deregister Generator. Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
