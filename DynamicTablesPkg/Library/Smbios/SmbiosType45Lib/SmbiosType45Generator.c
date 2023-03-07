/** @file
  SMBIOS Type 45 Table Generator.

  Copyright (c) 2024, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  Copyright (c) 2020 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmbiosStringTableLib.h>

// Module specific include files.
#include <Library/UefiBootServicesTableLib.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <Protocol/DynamicTableFactoryProtocol.h>
#include <IndustryStandard/SmBios.h>

STATIC
EFI_STATUS
AddReferenceHandle (
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN  CM_OBJECT_TOKEN                                      CmObjToken,
  OUT UINT16                                               *ReferenceHandle
  )
{
  EFI_STATUS         Status;
  SMBIOS_HANDLE_MAP  *HandleMap;

  Status    = EFI_SUCCESS;
  HandleMap = NULL;
  if (CmObjToken == CM_NULL_TOKEN) {
    return EFI_NOT_FOUND;
  }

  // Check if there is an existing handle.
  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken);
  if (HandleMap == NULL) {
    Status = EFI_NOT_FOUND;
  } else {
    *ReferenceHandle = HandleMap->SmbiosTblHandle;
    Status           = EFI_SUCCESS;
    DEBUG ((DEBUG_INFO, "%a: Re-using Handle %u\n", __FUNCTION__, *ReferenceHandle));
  }

  return Status;
}

/** This macro expands to a function that retrieves the TPM device
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjFirmwareInventoryInfo,
  CM_SMBIOS_FIRMWARE_INVENTORY_INFO
  )

/** This function pointer describes the interface to used by the
    Table Manager to give the generator an opportunity to free
    any resources allocated for building the SMBIOS table.

  @param [in]  Generator            Pointer to the SMBIOS table generator.
  @param [in]  TableFactoryProtocol Pointer to the Table Factory protocol.
  @param [in]  SmbiosTableInfo      Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol interface.
  @param [in]  Table                Pointer to the list of generated SMBIOS tables.
  @param [in]  CmObjectToken        Pointer to the list of CM object tokens.
  @param [in]  TableCount           Number of SMBIOS tables generated.

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
STATIC
EFI_STATUS
FreeSmbiosType45TableEx (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST  Generator,
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

/** This function pointer describes the interface to SMBIOS table build
    functions provided by the SMBIOS table generator and called by the
    Table Manager to build an SMBIOS table.

  @param [in]  Generator            Pointer to the SMBIOS table generator.
  @param [in]  TableFactoryProtocol Pointer to the Table Factory protocol.
  @param [in]  SmbiosTableInfo      Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol interface.
  @param [in]  Table                Pointer to the list of generated SMBIOS tables.
  @param [in]  CmObjectToken        Pointer to the list of CM object tokens.
  @param [in]  TableCount           Number of SMBIOS tables generated.

  @return EFI_SUCCESS  If the table is generated successfully or other
                        failure codes as returned by the generator.
**/
STATIC
EFI_STATUS
BuildSmbiosType45TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                         Status;
  CM_SMBIOS_FIRMWARE_INVENTORY_INFO  *FirmwareInventoryInfo;
  UINT8                              FirmwareComponentNameRef;
  UINT8                              FirmwareVersionRef;
  UINT8                              FirmwareIdRef;
  UINT8                              ReleaseDateRef;
  UINT8                              ManufacturerRef;
  UINT8                              LowestSupportedVersionRef;
  CHAR8                              *OptionalStrings;
  SMBIOS_TABLE_TYPE45                *SmbiosRecord;
  UINTN                              SmbiosRecordSize;
  STRING_TABLE                       StrTable;
  UINT32                             CmObjCount;
  UINT32                             Index;
  UINT32                             AssociatedComponentIndex;
  SMBIOS_STRUCTURE                   **TableList;
  CM_OBJECT_TOKEN                    *CmObjectList;
  UINT32                             TableListCount;
  UINTN                              ReferenceHanldesSize;
  UINT16                             ReferenceHandle;
  SMBIOS_HANDLE                      *AssociatedSmbiosHandle;
  SMBIOS_HANDLE                      *AssociatedSmbiosHandleBuffer;
  UINT32                             ValidAssociatedComponentCount;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve enclosure info from CM object
  //
  *Table                       = NULL;
  TableList                    = NULL;
  CmObjectList                 = NULL;
  AssociatedSmbiosHandleBuffer = NULL;
  TableListCount               = 0;

  Status = EFI_SUCCESS;

  //
  // There is no SMBIOS table data, depends on ESmbiosObjFirmwareInventoryInfo to establish type 3 record.
  //
  Status = GetESmbiosObjFirmwareInventoryInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &FirmwareInventoryInfo,
             &CmObjCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Firmware Inventory CM Object %r\n",
      Status
      ));
    goto ErrorExit;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * CmObjCount);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for SMBIOS table 45\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN) * CmObjCount);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for SMBIOS table 45\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  for (Index = 0; Index < CmObjCount; Index++) {
    //
    // Copy strings to SMBIOS table
    //
    Status = StringTableInitialize (&StrTable, 6);
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "Failed to allocate string table for CM Object, %r\n",
        Status
        ));
      goto ErrorExit;
    }

    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].FirmwareComponentName, FirmwareComponentNameRef);
    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].FirmwareVersion, FirmwareVersionRef);
    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].FirmwareId, FirmwareIdRef);
    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].ReleaseDate, ReleaseDateRef);
    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].Manufacturer, ManufacturerRef);
    STRING_TABLE_ADD_STRING (StrTable, FirmwareInventoryInfo[Index].LowestSupportedVersion, LowestSupportedVersionRef);

    // Is there a reference to an associated component handle.
    AssociatedSmbiosHandleBuffer = NULL;
    AssociatedSmbiosHandleBuffer = AllocateZeroPool (
                                     FirmwareInventoryInfo[Index].AssociatedComponentCount * sizeof (*AssociatedSmbiosHandleBuffer)
                                     );
    if (AssociatedSmbiosHandleBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    ValidAssociatedComponentCount = 0;
    for (AssociatedComponentIndex = 0; AssociatedComponentIndex < FirmwareInventoryInfo[Index].AssociatedComponentCount; AssociatedComponentIndex++) {
      Status = AddReferenceHandle (
                 TableFactoryProtocol,
                 FirmwareInventoryInfo[Index].AssociatedComponentHandles[AssociatedComponentIndex],
                 &ReferenceHandle
                 );
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "%a: Update Reference Handle to %x\n", __FUNCTION__, ReferenceHandle));
        AssociatedSmbiosHandleBuffer[ValidAssociatedComponentCount++] = ReferenceHandle;
      }
    }

    ReferenceHanldesSize = sizeof (SMBIOS_HANDLE) * ValidAssociatedComponentCount;

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE45) +
                       ReferenceHanldesSize +
                       StringTableGetStringSetSize (&StrTable);

    SmbiosRecord = (SMBIOS_TABLE_TYPE45 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SmbiosRecord->FirmwareComponentName  = FirmwareComponentNameRef;
    SmbiosRecord->FirmwareVersion        = FirmwareVersionRef;
    SmbiosRecord->FirmwareId             = FirmwareIdRef;
    SmbiosRecord->ReleaseDate            = ReleaseDateRef;
    SmbiosRecord->Manufacturer           = ManufacturerRef;
    SmbiosRecord->LowestSupportedVersion = LowestSupportedVersionRef;

    AssociatedSmbiosHandle = (SMBIOS_HANDLE *)(SmbiosRecord + 1);
    if (ValidAssociatedComponentCount > 0) {
      CopyMem (
        AssociatedSmbiosHandle,
        AssociatedSmbiosHandleBuffer,
        ValidAssociatedComponentCount * sizeof (*AssociatedSmbiosHandleBuffer)
        );
    }

    if (AssociatedSmbiosHandleBuffer != NULL) {
      FreePool (AssociatedSmbiosHandleBuffer);
      AssociatedSmbiosHandleBuffer = NULL;
    }

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1) + ReferenceHanldesSize;

    //
    // publish the string set
    //
    Status = StringTablePublishStringSet (
               &StrTable,
               OptionalStrings,
               (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE45))
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "Failed to populate string table for CM Object, %r\n",
        Status
        ));
      goto ErrorExit;
    }

    SmbiosRecord->FirmwareVersionFormat    = FirmwareInventoryInfo[Index].FirmwareVersionFormat;
    SmbiosRecord->FirmwareIdFormat         = FirmwareInventoryInfo[Index].FirmwareIdFormat;
    SmbiosRecord->ImageSize                = FirmwareInventoryInfo[Index].ImageSize;
    SmbiosRecord->Characteristics          = FirmwareInventoryInfo[Index].Characteristics;
    SmbiosRecord->State                    = FirmwareInventoryInfo[Index].State;
    SmbiosRecord->AssociatedComponentCount = ValidAssociatedComponentCount;

    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = Generator->Type;
    SmbiosRecord->Hdr.Length = sizeof (*SmbiosRecord) + ReferenceHanldesSize;
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      =  FirmwareInventoryInfo[Index].FirmwareInventoryInfoToken;

    // Free string table
    StringTableFree (&StrTable);

    TableListCount++;
  }

ErrorExit:
  if (AssociatedSmbiosHandleBuffer != NULL) {
    FreePool (AssociatedSmbiosHandleBuffer);
  }

  *Table         = TableList;
  *CmObjectToken = CmObjectList;
  *TableCount    = TableListCount;
  return Status;
}

/** The interface for the SMBIOS Type45 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType45Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType45),
  // Generator Description
  L"SMBIOS.TYPE45.GENERATOR",
  // SMBIOS Table Type
  SMBIOS_TYPE_FIRMWARE_INVENTORY_INFORMATION,
  NULL,
  NULL,
  // Build table function Extended
  BuildSmbiosType45TableEx,
  // Free function Extended
  FreeSmbiosType45TableEx
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
SmbiosType45LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType45Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 45: Register Generator. Status = %r\n",
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
SmbiosType45LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType45Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type45: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
