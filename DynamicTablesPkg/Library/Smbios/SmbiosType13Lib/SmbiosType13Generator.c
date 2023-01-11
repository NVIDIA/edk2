/** @file
  SMBIOS Type 13 Table Generator.

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

/** This macro expands to a function that retrieves the System
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjBiosLanguageInfo,
  CM_SMBIOS_BIOS_LANGUAGE_INFO
  )

/** This function pointer describes the interface to used by the
    Table Manager to give the generator an opportunity to free
    any resources allocated for building the SMBIOS table.

  @param [in]  Generator            Pointer to the SMBIOS table generator.
  @param [in]  TableFactoryProtocol Pointer to the Table Factory protocol.
  @param [in]  SmbiosTableInfo      Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol interface.
  @param [in]  Table                Pointer to the generated SMBIOS tables.

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
STATIC
EFI_STATUS
FreeSmbiosType13Table (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST  Generator,
  IN      CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL     *CONST  TableFactoryProtocol,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO             *CONST  SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL     *CONST  CfgMgrProtocol,
  IN      SMBIOS_STRUCTURE                              **CONST  Table
  )
{
  if (*Table != NULL) {
    FreePool (*Table);
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
  @param [in]  Table                Pointer to the SMBIOS table.
  @param [in]  CmObjectToken        Pointer to the CM object token.

  @return EFI_SUCCESS  If the table is generated successfully or other
                        failure codes as returned by the generator.
**/
STATIC
EFI_STATUS
BuildSmbiosType13Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               **Table,
  OUT       CM_OBJECT_TOKEN                                *CmObjectToken
  )
{
  EFI_STATUS                    Status;
  CM_SMBIOS_BIOS_LANGUAGE_INFO  *BiosLanguageInfo;
  SMBIOS_TABLE_TYPE13           *SmbiosRecord;
  UINTN                         SmbiosRecordSize;
  UINTN                         Index;
  STRING_TABLE                  StrTable;
  UINT32                        CmObjCount;
  CM_OBJECT_TOKEN               CmObject;
  CHAR8                         *OptionalStrings;
  UINT8                         LanguageRef;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve Bios Language Info from CM object
  //
  *Table = NULL;

  Status = EFI_SUCCESS;

  Status = GetESmbiosObjBiosLanguageInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &BiosLanguageInfo,
             &CmObjCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Bios Language Info CM Object %r\n",
      Status
      ));
    goto exit;
  }

  //
  // Copy strings to SMBIOS table
  //
  Status = StringTableInitialize (&StrTable, BiosLanguageInfo->InstallableLanguages);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to allocate string table for CM Object, %r\n",
      Status
      ));
    goto exit;
  }

  ASSERT (BiosLanguageInfo->SupportedLanguages != NULL);
  for (Index = 0; Index < BiosLanguageInfo->InstallableLanguages; Index++) {
    if (BiosLanguageInfo->SupportedLanguages[Index] != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, BiosLanguageInfo->SupportedLanguages[Index], LanguageRef);
    }
  }

  SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE13) +
                     StringTableGetStringSetSize (&StrTable);

  SmbiosRecord = (SMBIOS_TABLE_TYPE13 *)AllocateZeroPool (SmbiosRecordSize);
  if (SmbiosRecord == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type13 record\n", __FUNCTION__));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  SmbiosRecord->InstallableLanguages = BiosLanguageInfo->InstallableLanguages;
  SmbiosRecord->Flags                = BiosLanguageInfo->Flags;
  SmbiosRecord->CurrentLanguages     = BiosLanguageInfo->CurrentLanguage;

  OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
  // publish the string set
  StringTablePublishStringSet (
    &StrTable,
    OptionalStrings,
    (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE13))
    );
  // setup the header
  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE13);
  CmObject                 = BiosLanguageInfo->BiosLanguageInfoToken;

  *Table         = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjectToken = CmObject;

exit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type13 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType13Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType13),
  // Generator Description
  L"SMBIOS.TYPE13.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION,
  // Build table function
  BuildSmbiosType13Table,
  // Free function
  FreeSmbiosType13Table,
  NULL,
  NULL
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
SmbiosType13LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType13Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 13: Register Generator. Status = %r\n",
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
SmbiosType13LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType13Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type13: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
