/** @file
  SMBIOS Type9 Table Generator.

  Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  Copyright (c) 2020 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmbiosStringTableLib.h>

// Module specific include files.
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

/** This macro expands to a function that retrieves the System Slot
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjSystemSlotInfo,
  CM_STD_SYSTEM_SLOTS_INFO
  )

STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType9TableEx (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST  This,
  IN      CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL     *CONST  TableFactoryProtocol,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO             *CONST  SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL     *CONST  CfgMgrProtocol,
  IN      SMBIOS_STRUCTURE                             ***CONST  Table,
  IN      CM_OBJECT_TOKEN                                        **CmObjectToken,
  IN      CONST UINTN                                            TableCount
  )
{
  UINTN             Index;
  SMBIOS_STRUCTURE  **TableList;

  TableList = *Table;

  if (TableList != NULL) {
    for (Index = 0; Index < TableCount; Index++) {
      if (TableList[Index] != NULL) {
        FreePool (TableList[Index]);
      }
    }

    FreePool (TableList);
  }

  if (*CmObjectToken != NULL) {
    FreePool (*CmObjectToken);
  }

  return EFI_SUCCESS;
}

/** Construct SMBIOS Type9 Table describing system slots

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

  @param [in]  This            Pointer to the SMBIOS table generator.
  @param [in]  SmbiosTableInfo Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol  Pointer to the Configuration Manager
                               Protocol interface.
  @param [out] Table           Pointer to the SMBIOS table.

  @retval EFI_SUCCESS            Table generated successfully.
  @retval EFI_BAD_BUFFER_SIZE    The size returned by the Configuration
                                 Manager is less than the Object size for
                                 the requested object.
  @retval EFI_INVALID_PARAMETER  A parameter is invalid.
  @retval EFI_NOT_FOUND          Could not find information.
  @retval EFI_OUT_OF_RESOURCES   Could not allocate memory.
  @retval EFI_UNSUPPORTED        Unsupported configuration.
**/
STATIC
EFI_STATUS
EFIAPI
BuildSmbiosType9TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *This,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                   Status;
  UINT32                       NumSystemSlots;
  SMBIOS_STRUCTURE             **TableList;
  CM_OBJECT_TOKEN              *CmObjectList;
  CM_STD_SYSTEM_SLOTS_INFO     *SystemSlotInfo;
  UINTN                        Index;
  UINTN                        Count;
  UINT8                        SlotDesignationRef;
  CHAR8                        *OptionalStrings;
  SMBIOS_TABLE_TYPE9           *SmbiosRecord;
  SMBIOS_TABLE_TYPE9_EXTENDED  *Type9Extended;
  UINTN                        SmbiosRecordSize;
  UINTN                        SmbiosRecordSizeEx;
  STRING_TABLE                 StrTable;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table         = NULL;
  *CmObjectToken = NULL;
  Status         = GetEStdObjSystemSlotInfo (
                     CfgMgrProtocol,
                     CM_NULL_TOKEN,
                     &SystemSlotInfo,
                     &NumSystemSlots
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to get System Slots CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumSystemSlots);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for System slots table\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType9TableEx
  *Table      = TableList;
  *TableCount = NumSystemSlots;
  for (Index = 0; Index < NumSystemSlots; Index++) {
    TableList[Index] = NULL;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumSystemSlots);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for CmObjectList\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType9TableEx
  *CmObjectToken = CmObjectList;

  for (Index = 0; Index < NumSystemSlots; Index++) {
    // Initialize and allocate memory for string table
    Status = StringTableInitialize (&StrTable, 1);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
      return Status;
    }

    SlotDesignationRef = 0;

    if (SystemSlotInfo[Index].SlotDesignation != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 SystemSlotInfo[Index].SlotDesignation,
                 &SlotDesignationRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add SlotDesignation String %r \n", __FUNCTION__, Status));
      }
    }

    if (SystemSlotInfo[Index].PeerGroupingCount > MAX_SLOT_PEER_GROUP) {
      SystemSlotInfo[Index].PeerGroupingCount = MAX_SLOT_PEER_GROUP;
    }

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE9) - sizeof (MISC_SLOT_PEER_GROUP) +
                       sizeof (SMBIOS_TABLE_TYPE9_EXTENDED) +
                       StringTableGetStringSetSize (&StrTable);
    SmbiosRecordSizeEx = SystemSlotInfo[Index].PeerGroupingCount * sizeof (MISC_SLOT_PEER_GROUP);
    SmbiosRecord       = (SMBIOS_TABLE_TYPE9 *)AllocateZeroPool (SmbiosRecordSize + SmbiosRecordSizeEx);
    if (SmbiosRecord == NULL) {
      // Free string table
      StringTableFree (&StrTable);
      return EFI_OUT_OF_RESOURCES;
    }

    SmbiosRecord->SlotDesignation                  = SlotDesignationRef;
    SmbiosRecord->SlotType                         = SystemSlotInfo[Index].SlotType;
    SmbiosRecord->SlotDataBusWidth                 = SystemSlotInfo[Index].SlotDataBusWidth;
    SmbiosRecord->CurrentUsage                     = SystemSlotInfo[Index].CurrentUsage;
    SmbiosRecord->SlotLength                       = SystemSlotInfo[Index].SlotLength;
    SmbiosRecord->SlotID                           = SystemSlotInfo[Index].SlotID;
    *(UINT8 *) &SmbiosRecord->SlotCharacteristics1 = SystemSlotInfo[Index].SlotCharacteristics1;
    *(UINT8 *) &SmbiosRecord->SlotCharacteristics2 = SystemSlotInfo[Index].SlotCharacteristics2;
    SmbiosRecord->SegmentGroupNum                  = SystemSlotInfo[Index].SegmentGroupNum;
    SmbiosRecord->BusNum                           = SystemSlotInfo[Index].BusNum;
    SmbiosRecord->DevFuncNum                       = SystemSlotInfo[Index].DevFuncNum;
    SmbiosRecord->DataBusWidth                     = SystemSlotInfo[Index].DataBusWidth;
    SmbiosRecord->PeerGroupingCount                = SystemSlotInfo[Index].PeerGroupingCount;
    for (Count = 0; Count < SmbiosRecord->PeerGroupingCount; Count++) {
      SmbiosRecord->PeerGroups[Count].SegmentGroupNum = SystemSlotInfo[Index].PeerGroups[Count].SegmentGroupNum;
      SmbiosRecord->PeerGroups[Count].BusNum          = SystemSlotInfo[Index].PeerGroups[Count].BusNum;
      SmbiosRecord->PeerGroups[Count].DevFuncNum      = SystemSlotInfo[Index].PeerGroups[Count].DevFuncNum;
      SmbiosRecord->PeerGroups[Count].DataBusWidth    = SystemSlotInfo[Index].PeerGroups[Count].DataBusWidth;
    }

    Type9Extended = (SMBIOS_TABLE_TYPE9_EXTENDED *)((UINT8 *)SmbiosRecord +
                                                    sizeof (SMBIOS_TABLE_TYPE9) - sizeof (MISC_SLOT_PEER_GROUP) + SmbiosRecordSizeEx);

    Type9Extended->SlotInformation   = SystemSlotInfo[Index].SlotInformation;
    Type9Extended->SlotPhysicalWidth = SystemSlotInfo[Index].SlotPhysicalWidth;
    Type9Extended->SlotPitch         = SystemSlotInfo[Index].SlotPitch;
    Type9Extended->SlotHeight        = SystemSlotInfo[Index].SlotHeight;

    OptionalStrings = (CHAR8 *)(Type9Extended + 1);
    // Publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE9) + sizeof (MISC_SLOT_PEER_GROUP) + SmbiosRecordSizeEx - sizeof (SMBIOS_TABLE_TYPE9_EXTENDED))
      );

    // Setup the header
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE9) - sizeof (MISC_SLOT_PEER_GROUP) + SmbiosRecordSizeEx + sizeof (SMBIOS_TABLE_TYPE9_EXTENDED);
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      = SystemSlotInfo[Index].SystemSlotInfoToken;

    // Free string table
    StringTableFree (&StrTable);
  }

  return EFI_SUCCESS;
}

/** The interface for the SMBIOS Type9 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType9Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType09),
  // Generator Description
  L"SMBIOS.TYPE9.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_SYSTEM_SLOTS,
  NULL,
  NULL,
  // Build table function Extended.
  BuildSmbiosType9TableEx,
  // Free function Extended.
  FreeSmbiosType9TableEx
};

/** Register the Generator with the SMBIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is registered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_ALREADY_STARTED   The Generator for the Table ID
                                is already registered.
**/
EFI_STATUS
EFIAPI
SmbiosType9LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType9Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 9: Register Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/** Deregister the Generator from the SMBIOS Table Factory.

  @param [in]  ImageHandle  The handle to the image.
  @param [in]  SystemTable  Pointer to the System Table.

  @retval EFI_SUCCESS           The Generator is deregistered.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The Generator is not registered.
**/
EFI_STATUS
EFIAPI
SmbiosType9LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType9Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type9: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
