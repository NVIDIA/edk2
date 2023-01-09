/** @file
  SMBIOS Type14 Table Generator.
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  Copyright (c) 2020 - 2021, Arm Limited. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmbiosStringTableLib.h>

// Module specific include files.
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <Protocol/DynamicTableFactoryProtocol.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

/** This macro expands to a function that retrieves the Group Associations
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjGroupAssociations,
  CM_STD_GROUP_ASSOCIATIONS
  )

STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType14TableEx (
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

STATIC
EFI_STATUS
AddReferenceHandle (
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN  CM_OBJECT_TOKEN                                      CmObjToken,
  IN  SMBIOS_TABLE_GENERATOR_ID                            GeneratorId,
  OUT UINT16                                               *ReferenceHandle
  )
{
  EFI_STATUS                    Status;
  EFI_SMBIOS_PROTOCOL           *SmbiosProtocol;
  SMBIOS_HANDLE_MAP             *HandleMap;
  CONST SMBIOS_TABLE_GENERATOR  *Generator;

  if (CmObjToken == CM_NULL_TOKEN) {
    return EFI_NOT_FOUND;
  }

  // Check if there is an existing handle.
  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken);
  if (HandleMap == NULL) {
    // If a Handle isn't found, check if there is a generator,
    Status = TableFactoryProtocol->GetSmbiosTableGenerator (TableFactoryProtocol, GeneratorId, &Generator);
    // If there isn't a generator for the required Table, then proceed
    // without it.
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Generator not Found %r\n", __FUNCTION__, Status));
      return Status;
    } else {
      // If there is a generator then create an SmbiosHandle for it.
      Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&SmbiosProtocol);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Could not locate SMBIOS protocol.  %r\n", __FUNCTION__, Status, Status));
        return Status;
      }

      *ReferenceHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status           = TableFactoryProtocol->AddSmbiosHandle (SmbiosProtocol, ReferenceHandle, CmObjToken, GeneratorId);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Smbios Handle %r\n", __FUNCTION__, Status));
        return Status;
      }
    }
  } else {
    *ReferenceHandle = HandleMap->SmbiosTblHandle;
    Status           = EFI_SUCCESS;
    DEBUG ((DEBUG_ERROR, "%a: Re-using Handle %u\n", __FUNCTION__, *ReferenceHandle));
  }

  return Status;
}

/** Construct SMBIOS Type14 Table describing Group Associations.

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
BuildSmbiosType14TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *This,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                 Status;
  UINT32                     NumGroups;
  SMBIOS_STRUCTURE           **TableList;
  CM_OBJECT_TOKEN            *CmObjectList;
  CM_STD_GROUP_ASSOCIATIONS  *GroupAssociations;
  UINT8                      GroupNameRef;
  UINTN                      Index;
  UINTN                      HandleIndex;
  CHAR8                      *OptionalStrings;
  SMBIOS_TABLE_TYPE14        *SmbiosRecord;
  UINTN                      SmbiosRecordSize;
  UINTN                      SmbiosRecordSizeEx;
  STRING_TABLE               StrTable;
  UINT16                     ReferenceHandle;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table         = NULL;
  *CmObjectToken = NULL;
  Status         = GetEStdObjGroupAssociations (
                     CfgMgrProtocol,
                     CM_NULL_TOKEN,
                     &GroupAssociations,
                     &NumGroups
                     );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to get Baseboard CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumGroups);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for Type 14 tables\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType14TableEx
  *Table      = TableList;
  *TableCount = NumGroups;
  for (Index = 0; Index < NumGroups; Index++) {
    TableList[Index] = NULL;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumGroups);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for CmObjectList\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType14TableEx
  *CmObjectToken = CmObjectList;

  for (Index = 0; Index < NumGroups; Index++) {
    // Initialize and allocate memory for string table
    Status = StringTableInitialize (&StrTable, 1);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
      return Status;
    }

    SmbiosRecordSizeEx = 0;
    GroupNameRef       = 0;
    if (GroupAssociations[Index].GroupName != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 GroupAssociations[Index].GroupName,
                 &GroupNameRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Group Name String %r\n", __FUNCTION__, Status));
      }
    }

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE14) + StringTableGetStringSetSize (&StrTable);
    // If there is more than 1 (default size) items, it requires more extra space.
    if (GroupAssociations[Index].NumberOfItems > 1) {
      SmbiosRecordSizeEx = (GroupAssociations[Index].NumberOfItems - 1) * sizeof (GROUP_STRUCT);
    }

    SmbiosRecord = (SMBIOS_TABLE_TYPE14 *)AllocateZeroPool (SmbiosRecordSize + SmbiosRecordSizeEx);
    if (SmbiosRecord == NULL) {
      // Free string table
      StringTableFree (&StrTable);
      return EFI_OUT_OF_RESOURCES;
    }

    SmbiosRecord->GroupName = GroupNameRef;
    // Add the handles of other structures that are contained by this baseboard
    for (HandleIndex = 0; HandleIndex < GroupAssociations[Index].NumberOfItems; HandleIndex++) {
      Status = AddReferenceHandle (
                 TableFactoryProtocol,
                 GroupAssociations[Index].ContainedCmObjects[HandleIndex].CmObjToken,
                 GroupAssociations[Index].ContainedCmObjects[HandleIndex].GeneratorId,
                 &ReferenceHandle
                 );
      if (!EFI_ERROR (Status)) {
        SmbiosRecord->Group[HandleIndex].ItemHandle = ReferenceHandle;
      } else {
        SmbiosRecord->Group[HandleIndex].ItemHandle = 0xFFFF; // Not exist.
      }

      SmbiosRecord->Group[HandleIndex].ItemType =
        GroupAssociations[Index].ContainedCmObjects[HandleIndex].GeneratorId -
        EStdSmbiosTableIdType00;
    }

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
    OptionalStrings = (CHAR8 *)(OptionalStrings + SmbiosRecordSizeEx);

    // Publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE14))
      );

    // Setup the header
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE14) + SmbiosRecordSizeEx;
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      = GroupAssociations[Index].GroupAssociationsToken;

    // Free string table
    StringTableFree (&StrTable);
  }

  return EFI_SUCCESS;
}

/** The interface for the SMBIOS Type14 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType14Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType14),
  // Generator Description
  L"SMBIOS.TYPE14.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_GROUP_ASSOCIATIONS,
  NULL,
  NULL,
  // Build table function Extended.
  BuildSmbiosType14TableEx,
  // Free function Extended.
  FreeSmbiosType14TableEx
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
SmbiosType14LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType14Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 14: Register Generator. Status = %r\n",
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
SmbiosType14LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType14Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type14: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
