/** @file
  SMBIOS Type2 Table Generator.
  Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/** This macro expands to a function that retrieves the System Slot
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjBaseboardInfo,
  CM_STD_BASEBOARD_INFO
  )

STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType2TableEx (
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
    *ReferenceHandle = HandleMap->SmbiosTableId;
    DEBUG ((DEBUG_INFO, "%a: Re-using Handle %u\n", __FUNCTION__, *ReferenceHandle));
  }

  return Status;
}

/** Construct SMBIOS Type2 Table describing Baseboard.

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
BuildSmbiosType2TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *This,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS             Status;
  UINT32                 NumBaseboards;
  SMBIOS_STRUCTURE       **TableList;
  CM_OBJECT_TOKEN        *CmObjectList;
  CM_STD_BASEBOARD_INFO  *BaseboardInfo;
  UINTN                  Index;
  UINTN                  HandleIndex;
  UINT8                  ManufacturerRef;
  UINT8                  ProductNameRef;
  UINT8                  VersionRef;
  UINT8                  SerialNumberRef;
  UINT8                  AssetTagRef;
  UINT8                  LocationInChassisRef;
  CHAR8                  *OptionalStrings;
  SMBIOS_TABLE_TYPE2     *SmbiosRecord;
  UINTN                  SmbiosRecordSize;
  UINTN                  SmbiosRecordSizeEx;
  STRING_TABLE           StrTable;
  UINT16                 ReferenceHandle;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table         = NULL;
  *CmObjectToken = NULL;
  Status         = GetEStdObjBaseboardInfo (
                     CfgMgrProtocol,
                     CM_NULL_TOKEN,
                     &BaseboardInfo,
                     &NumBaseboards
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

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumBaseboards);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for System slots table\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType2TableEx
  *Table      = TableList;
  *TableCount = NumBaseboards;
  for (Index = 0; Index < NumBaseboards; Index++) {
    TableList[Index] = NULL;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumBaseboards);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for CmObjectList\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType2TableEx
  *CmObjectToken = CmObjectList;

  for (Index = 0; Index < NumBaseboards; Index++) {
    // Initialize and allocate memory for string table
    Status = StringTableInitialize (&StrTable, 6);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
      return Status;
    }

    ManufacturerRef      = 0;
    ProductNameRef       = 0;
    VersionRef           = 0;
    SerialNumberRef      = 0;
    AssetTagRef          = 0;
    LocationInChassisRef = 0;
    SmbiosRecordSizeEx   = 0;

    if (BaseboardInfo[Index].Manufacturer != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].Manufacturer,
                 &ManufacturerRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Manufacturer String %r\n", __FUNCTION__, Status));
      }
    }

    if (BaseboardInfo[Index].ProductName != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].ProductName,
                 &ProductNameRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add ProductName String %r\n", __FUNCTION__, Status));
      }
    }

    if (BaseboardInfo[Index].Version != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].Version,
                 &VersionRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Version String %r\n", __FUNCTION__, Status));
      }
    }

    if (BaseboardInfo[Index].SerialNumber != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].SerialNumber,
                 &SerialNumberRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add SerialNumber String %r\n", __FUNCTION__, Status));
      }
    }

    if (BaseboardInfo[Index].AssetTag != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].AssetTag,
                 &AssetTagRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add AssetTag String %r\n", __FUNCTION__, Status));
      }
    }

    if (BaseboardInfo[Index].LocationInChassis != NULL) {
      Status = StringTableAddString (
                 &StrTable,
                 BaseboardInfo[Index].LocationInChassis,
                 &LocationInChassisRef
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add LocationInChassis String %r\n", __FUNCTION__, Status));
      }
    }

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE2) + StringTableGetStringSetSize (&StrTable);
    // If there is more than 1 (default size) ContainedObjectHandles, it requires more extra space.
    if (BaseboardInfo[Index].NumberOfContainedObjectHandles > 1) {
      SmbiosRecordSizeEx = (BaseboardInfo[Index].NumberOfContainedObjectHandles - 1) * sizeof (SMBIOS_HANDLE);
    }

    SmbiosRecord = (SMBIOS_TABLE_TYPE2 *)AllocateZeroPool (SmbiosRecordSize + SmbiosRecordSizeEx);
    if (SmbiosRecord == NULL) {
      // Free string table
      StringTableFree (&StrTable);
      return EFI_OUT_OF_RESOURCES;
    }

    *(UINT8 *) &SmbiosRecord->FeatureFlag        = BaseboardInfo[Index].FeatureFlag;
    SmbiosRecord->BoardType                      = BaseboardInfo[Index].BoardType;
    SmbiosRecord->ChassisHandle                  = SMBIOS_HANDLE_PI_RESERVED;
    SmbiosRecord->NumberOfContainedObjectHandles = BaseboardInfo[Index].NumberOfContainedObjectHandles;
    // Add the handles of other structures that are contained by this baseboard
    for (HandleIndex = 0; HandleIndex < SmbiosRecord->NumberOfContainedObjectHandles; HandleIndex++) {
      Status = AddReferenceHandle (
                 TableFactoryProtocol,
                 BaseboardInfo[Index].ContainedCmObjects[HandleIndex].CmObjToken,
                 BaseboardInfo[Index].ContainedCmObjects[HandleIndex].GeneratorId,
                 &ReferenceHandle
                 );
      if (!EFI_ERROR (Status)) {
        SmbiosRecord->ContainedObjectHandles[HandleIndex] = ReferenceHandle;
      } else {
        SmbiosRecord->ContainedObjectHandles[HandleIndex] = SMBIOS_HANDLE_PI_RESERVED;
      }
    }

    // Is there a reference to a ChassisHandle.
    Status = AddReferenceHandle (
               TableFactoryProtocol,
               BaseboardInfo[Index].ChassisToken,
               EStdSmbiosTableIdType03,
               &ReferenceHandle
               );
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "%a: Update Type3 Chassis Reference Handle to %x\n", __FUNCTION__, ReferenceHandle));
      SmbiosRecord->ChassisHandle = ReferenceHandle;
    }

    SmbiosRecord->Manufacturer      = ManufacturerRef;
    SmbiosRecord->ProductName       = ProductNameRef;
    SmbiosRecord->Version           = VersionRef;
    SmbiosRecord->SerialNumber      = SerialNumberRef;
    SmbiosRecord->AssetTag          = AssetTagRef;
    SmbiosRecord->LocationInChassis = LocationInChassisRef;

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
    OptionalStrings = (CHAR8 *)(OptionalStrings + SmbiosRecordSizeEx);

    // Publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE2))
      );

    // Setup the header
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE2) + SmbiosRecordSizeEx;
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      = BaseboardInfo[Index].BaseboardInfoToken;

    // Free string table
    StringTableFree (&StrTable);
  }

  return EFI_SUCCESS;
}

/** The interface for the SMBIOS Type2 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType2Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType02),
  // Generator Description
  L"SMBIOS.TYPE2.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION,
  NULL,
  NULL,
  // Build table function Extended.
  BuildSmbiosType2TableEx,
  // Free function Extended.
  FreeSmbiosType2TableEx
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
SmbiosType2LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType2Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 2: Register Generator. Status = %r\n",
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
SmbiosType2LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType2Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type2: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
