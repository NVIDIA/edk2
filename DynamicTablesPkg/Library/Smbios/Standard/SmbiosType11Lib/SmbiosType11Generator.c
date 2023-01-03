/** @file
  SMBIOS Type11 Table Generator.

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

/** This macro expands to a function that retrieves the IPMI Device
    information from the Configuration Manager.
**/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjOemStrings,
  CM_STD_OEM_STRINGS
  )

/** This function pointer describes the interface to use by the
    Table Manager to give the generator an opportunity to free
    any resources allocated for building the SMBIOS table.

  @param [in]  Generator       Pointer to the SMBIOS table generator.
  @param [in]  SmbiosTableInfo Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol  Pointer to the Configuration Manager
                               Protocol interface.
  @param [in]  Table           Pointer to the generated SMBIOS table.

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType11Table (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST   Generator,
  IN      CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL     *CONST   TableFactoryProtocol,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO             *CONST   SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL     *CONST   CfgMgrProtocol,
  IN      SMBIOS_STRUCTURE                               **CONST  Table
  )
{
  if (*Table != NULL) {
    FreePool (*Table);
  }

  return EFI_SUCCESS;
}

/** This function pointer describes the interface to SMBIOS table build
    functions provided by the SMBIOS table generator and called by the
    Table Manager to build an SMBIOS table Type11.
  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.
  @param [in]  Generator       Pointer to the SMBIOS table generator.
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
BuildSmbiosType11Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN  CM_STD_OBJ_SMBIOS_TABLE_INFO                 *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT SMBIOS_STRUCTURE                                     **Table,
  OUT CM_OBJECT_TOKEN                                      *CmObjectToken
  )
{
  EFI_STATUS           Status;
  SMBIOS_TABLE_TYPE11  *SmbiosRecord;
  CM_STD_OEM_STRINGS   *OemStrings;
  UINT8                Index;
  CHAR8                *OptionalStrings;
  UINTN                SmbiosRecordSize;
  STRING_TABLE         StrTable;
  UINT8                StrRef;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  *Table = NULL;
  Status = GetEStdObjOemStrings (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &OemStrings,
             NULL
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to get OEM Strings CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  // Initialize and allocate memory for string table
  Status = StringTableInitialize (&StrTable, OemStrings->StringCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
    return Status;
  }

  StrRef = 0;
  if (OemStrings->StringTable != NULL) {
    for (Index = 0; Index < OemStrings->StringCount; Index++) {
      Status = StringTableAddString (
                 &StrTable,
                 OemStrings->StringTable[Index],
                 &StrRef
                 );
    }
  }

  SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE11) + StringTableGetStringSetSize (&StrTable);
  SmbiosRecord     = (SMBIOS_TABLE_TYPE11 *)AllocateZeroPool (SmbiosRecordSize);
  if (SmbiosRecord == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type11 record\n", __FUNCTION__));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  SmbiosRecord->StringCount = StrRef;
  OptionalStrings           = (CHAR8 *)(SmbiosRecord + 1);
  // publish the string set
  StringTablePublishStringSet (
    &StrTable,
    OptionalStrings,
    (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE11))
    );

  //
  // Setup SMBIOS header
  //
  SmbiosRecord->Hdr.Type   = SMBIOS_TYPE_OEM_STRINGS;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE11);

  *Table         = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjectToken = OemStrings->OemStringsToken;
  Status         = EFI_SUCCESS;

exit:
  return Status;
}

/** The interface for the SMBIOS Type11 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType11Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType11),
  // Generator Description
  L"SMBIOS.TYPE11.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_OEM_STRINGS,
  // Build table function.
  BuildSmbiosType11Table,
  // Free function.
  FreeSmbiosType11Table,
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
SmbiosType11LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType11Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type11: Register Generator. Status = %r\n",
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
SmbiosType11LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType11Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type11: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
