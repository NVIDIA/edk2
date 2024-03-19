/** @file
  SMBIOS Type7 Table Generator.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define SMBIOS_TYPE7_MAX_STRINGS  (1)

/** This macro expands to a function that retrieves the Memory Device
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjCacheInfo,
  CM_SMBIOS_CACHE_INFO
  )

/** This function pointer describes the interface to used by the
    Table Manager to give the generator an opportunity to free
    any resources allocated for building the SMBIOS table.

  @param [in]  Generator            Pointer to the SMBIOS table generator.
  @param [in]  TableFactoryProtocol Pointer to the Table Factory protocol.
  @param [in]  SmbiosTableInfo      Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol interface.
  @param [in]  Table                Pointer to the generated SMBIOS table.
  @param [in]  CmObjectToken        Pointer to the CM ObjectToken Array.
  @param [in]  TableCount           Number of SMBIOS tables.

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
EFI_STATUS
FreeSmbiosType7TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                          *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST    TableFactoryProtocol,
  IN  CONST CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST    SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST    CfgMgrProtocol,
  IN        SMBIOS_STRUCTURE                      ***CONST  Table,
  IN        CM_OBJECT_TOKEN                                 **CmObjectToken,
  IN        CONST UINTN                                     TableCount
  )
{
  UINTN             Index;
  SMBIOS_STRUCTURE  **TableList;

  TableList = *Table;
  for (Index = 0; Index < TableCount; Index++) {
    if (TableList[Index] != NULL) {
      FreePool (TableList[Index]);
    }
  }

  if (*CmObjectToken != NULL) {
    FreePool (*CmObjectToken);
  }

  if (TableList != NULL) {
    FreePool (TableList);
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
  @param [out] Table                Pointer to the generated SMBIOS table.
  @param [in]  CmObjectToken        Pointer to the CM object token.

  @return EFI_SUCCESS  If the table is generated successfully or other
                        failure codes as returned by the generator.
**/
EFI_STATUS
BuildSmbiosType7TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                              ***Table,
  OUT       CM_OBJECT_TOKEN                               **CmObjToken,
  OUT       UINTN                                 *CONST  TableCount
  )
{
  EFI_STATUS            Status;
  CM_SMBIOS_CACHE_INFO  *CacheInfo;
  UINT8                 SocketDesignationRef;
  CHAR8                 *OptionalStrings;
  SMBIOS_TABLE_TYPE7    *SmbiosRecord;
  UINTN                 SmbiosRecordSize;
  STRING_TABLE          StrTable;
  UINT32                NumCacheTable;
  SMBIOS_STRUCTURE      **TableList;
  CM_OBJECT_TOKEN       *CmObjectList;
  UINTN                 Index;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve cache info from CM object
  //
  *Table = NULL;
  Status = GetESmbiosObjCacheInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &CacheInfo,
             &NumCacheTable
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Cache CM Object %r\n",
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumCacheTable);
  if (TableList == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to alloc memory for %u devices table\n",
      __FUNCTION__,
      NumCacheTable
      ));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumCacheTable);
  if (CmObjectList == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to alloc memory for %u CMObjects\n",
      __FUNCTION__,
      NumCacheTable
      ));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  for (Index = 0; Index < NumCacheTable; Index++) {
    //
    // Copy strings to SMBIOS table
    //
    Status = StringTableInitialize (&StrTable, SMBIOS_TYPE7_MAX_STRINGS);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
      return Status;
    }

    STRING_TABLE_ADD_STRING (StrTable, CacheInfo[Index].SocketDesignation, SocketDesignationRef);

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE7) +
                       StringTableGetStringSetSize (&StrTable);
    SmbiosRecord = (SMBIOS_TABLE_TYPE7 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type7 record\n", __FUNCTION__));
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SmbiosRecord->SocketDesignation = SocketDesignationRef;

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
    // publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE7))
      );

    //
    // Fill in other fields of SMBIOS table
    //
    SmbiosRecord->CacheConfiguration  = CacheInfo[Index].CacheConfiguration;
    SmbiosRecord->MaximumCacheSize    = CacheInfo[Index].MaximumCacheSize;
    SmbiosRecord->InstalledSize       = CacheInfo[Index].InstalledSize;
    SmbiosRecord->SupportedSRAMType   = CacheInfo[Index].SupportedSRAMType;
    SmbiosRecord->CurrentSRAMType     = CacheInfo[Index].CurrentSRAMType;
    SmbiosRecord->CacheSpeed          = CacheInfo[Index].CacheSpeed;
    SmbiosRecord->ErrorCorrectionType = CacheInfo[Index].ErrorCorrectionType;
    SmbiosRecord->SystemCacheType     = CacheInfo[Index].SystemCacheType;
    SmbiosRecord->Associativity       = CacheInfo[Index].Associativity;
    SmbiosRecord->MaximumCacheSize2   = CacheInfo[Index].MaximumCacheSize2;
    SmbiosRecord->InstalledSize2      = CacheInfo[Index].InstalledSize2;
    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE7);

    TableList[Index]    = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index] = CacheInfo[Index].CacheInfoToken;
    StringTableFree (&StrTable);
  }

  *Table      = TableList;
  *CmObjToken = CmObjectList;
  *TableCount = NumCacheTable;
  Status      = EFI_SUCCESS;

ErrorExit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type7 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType7Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType07),
  // Generator Description
  L"SMBIOS.TYPE7.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_CACHE_INFORMATION,
  NULL,
  NULL,
  // Build table function
  BuildSmbiosType7TableEx,
  // Free function
  FreeSmbiosType7TableEx
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
SmbiosType7LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType7Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 7: Register Generator. Status = %r\n",
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
SmbiosType7LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType7Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type7: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
