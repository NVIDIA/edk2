/** @file
  SMBIOS Type 3 Table Generator.

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
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <Protocol/DynamicTableFactoryProtocol.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

#define SIZE_OF_SKU_NUMBER  sizeof (SMBIOS_TABLE_STRING)

/** This macro expands to a function that retrieves the System
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjEnclosureInfo,
  CM_SMBIOS_ENCLOSURE_INFO
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
FreeSmbiosType3TableEx (
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
BuildSmbiosType3TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                Status;
  CM_SMBIOS_ENCLOSURE_INFO  *EnclosureInfo;
  UINT8                     ManufacturerRef;
  UINT8                     AssetTagRef;
  UINT8                     VersionRef;
  UINT8                     SerialNumRef;
  UINT8                     SkuNumRef;
  CHAR8                     *SkuNumberField;
  UINTN                     ContainedElementsSize;
  CHAR8                     *OptionalStrings;
  SMBIOS_TABLE_TYPE3        *SmbiosRecord;
  UINTN                     SmbiosRecordSize;
  STRING_TABLE              StrTable;
  UINT32                    CmObjCount;
  UINT32                    Index;
  SMBIOS_STRUCTURE          **TableList;
  CM_OBJECT_TOKEN           *CmObjectList;
  UINT32                    TableListCount;
  UINTN                     TableSizeWithoutContainedElements;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve enclosure info from CM object
  //
  *Table         = NULL;
  TableList      = NULL;
  CmObjectList   = NULL;
  TableListCount = 0;

  //
  // There is no SMBIOS table data, depends on EStdObjEnclosureInfo to establish type 3 record.
  //
  Status = GetESmbiosObjEnclosureInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &EnclosureInfo,
             &CmObjCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Enclosure CM Object %r\n",
      Status
      ));
    goto ErrorExit;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * CmObjCount);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for SMBIOS table 3\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN) * CmObjCount);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for SMBIOS table 3\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  for (Index = 0; Index < CmObjCount; Index++) {
    //
    // Copy strings to SMBIOS table
    //
    Status = StringTableInitialize (&StrTable, 5);
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "Failed to allocate string table for CM Object, %r\n",
        Status
        ));
      goto ErrorExit;
    }

    STRING_TABLE_ADD_STRING (StrTable, EnclosureInfo[Index].Manufacturer, ManufacturerRef);
    STRING_TABLE_ADD_STRING (StrTable, EnclosureInfo[Index].AssetTag, AssetTagRef);
    STRING_TABLE_ADD_STRING (StrTable, EnclosureInfo[Index].Version, VersionRef);
    STRING_TABLE_ADD_STRING (StrTable, EnclosureInfo[Index].SerialNum, SerialNumRef);
    STRING_TABLE_ADD_STRING (StrTable, EnclosureInfo[Index].SkuNum, SkuNumRef);
    ContainedElementsSize = EnclosureInfo[Index].ContainedElementCount *
                            EnclosureInfo[Index].ContainedElementRecordLength;
    //
    // There is ContainedElements defined in SMBIOS_TABLE_TYPE3 struct, but there is no SkuNumber.
    // According to spec define, ContainedElements is varied from 0 to 255.
    //
    TableSizeWithoutContainedElements = sizeof (*SmbiosRecord) -
                                        sizeof (SmbiosRecord->ContainedElements) +
                                        SIZE_OF_SKU_NUMBER;

    if ((TableSizeWithoutContainedElements + ContainedElementsSize) >= 0x100) {
      Status = EFI_UNSUPPORTED;
      DEBUG ((
        DEBUG_ERROR,
        "ContainedElementsSize exceed spec defined, %r\n",
        Status
        ));
      goto ErrorExit;
    }

    SmbiosRecordSize = TableSizeWithoutContainedElements +
                       ContainedElementsSize +
                       StringTableGetStringSetSize (&StrTable);

    SmbiosRecord = (SMBIOS_TABLE_TYPE3 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SmbiosRecord->Manufacturer = ManufacturerRef;
    SmbiosRecord->AssetTag     = AssetTagRef;
    SmbiosRecord->Version      = VersionRef;
    SmbiosRecord->SerialNumber = SerialNumRef;

    //
    // Sku Number
    //
    SkuNumberField = (CHAR8 *)(SmbiosRecord) + TableSizeWithoutContainedElements + ContainedElementsSize - SIZE_OF_SKU_NUMBER;

    *SkuNumberField = SkuNumRef;

    OptionalStrings = (CHAR8 *)(SmbiosRecord) + TableSizeWithoutContainedElements + ContainedElementsSize;
    //
    // publish the string set
    //
    Status = StringTablePublishStringSet (
               &StrTable,
               OptionalStrings,
               (SmbiosRecordSize - TableSizeWithoutContainedElements - ContainedElementsSize)
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "Failed to populate string table for CM Object, %r\n",
        Status
        ));
      goto ErrorExit;
    }

    SmbiosRecord->Type                         = EnclosureInfo[Index].Type;
    SmbiosRecord->BootupState                  = EnclosureInfo[Index].BootupState;
    SmbiosRecord->PowerSupplyState             = EnclosureInfo[Index].PowerSupplyState;
    SmbiosRecord->ThermalState                 = EnclosureInfo[Index].ThermalState;
    SmbiosRecord->SecurityStatus               = EnclosureInfo[Index].SecurityStatus;
    SmbiosRecord->Height                       = EnclosureInfo[Index].Height;
    SmbiosRecord->NumberofPowerCords           = EnclosureInfo[Index].NumberofPowerCords;
    SmbiosRecord->ContainedElementCount        = EnclosureInfo[Index].ContainedElementCount;
    SmbiosRecord->ContainedElementRecordLength = EnclosureInfo[Index].ContainedElementRecordLength;

    //
    // Oem defined
    //
    CopyMem ((CHAR8 *)(SmbiosRecord->OemDefined), EnclosureInfo[Index].OemDefined, sizeof (SmbiosRecord->OemDefined));

    //
    // ContainedElements
    //
    CopyMem ((CHAR8 *)SmbiosRecord->ContainedElements, EnclosureInfo[Index].ContainedElements, ContainedElementsSize);

    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = Generator->Type;
    SmbiosRecord->Hdr.Length = (UINT8)TableSizeWithoutContainedElements + ContainedElementsSize;
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      =  EnclosureInfo[Index].EnclosureInfoToken;

    // Free string table
    StringTableFree (&StrTable);

    TableListCount++;
  }

ErrorExit:
  *Table         = TableList;
  *CmObjectToken = CmObjectList;
  *TableCount    = TableListCount;
  return Status;
}

/** The interface for the SMBIOS Type3 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType3Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType03),
  // Generator Description
  L"SMBIOS.TYPE3.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE,
  NULL,
  NULL,
  // Build table function Extended
  BuildSmbiosType3TableEx,
  // Free function Extended
  FreeSmbiosType3TableEx
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
SmbiosType3LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType3Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 3: Register Generator. Status = %r\n",
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
SmbiosType3LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType3Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type3: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
