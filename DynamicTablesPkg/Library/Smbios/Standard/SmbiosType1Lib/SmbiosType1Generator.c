/** @file
  SMBIOS Type 1 Table Generator.

  Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
  EObjNameSpaceStandard,
  EStdObjSystemInfo,
  CM_STD_SYSTEM_INFO
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

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
EFI_STATUS
EFIAPI
FreeSmbiosType1Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN  CONST CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN        SMBIOS_STRUCTURE                              **Table
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
  @param [out] Table                Pointer to the generated SMBIOS table.
  @param [in]  CmObjectToken        Pointer to the CM object token.

  @return EFI_SUCCESS  If the table is generated successfully or other
                        failure codes as returned by the generator.
**/
EFI_STATUS
EFIAPI
BuildSmbiosType1Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                              **Table,
  OUT       CM_OBJECT_TOKEN                               *CmObjToken
  )
{
  EFI_STATUS          Status;
  CM_STD_SYSTEM_INFO  *SystemInfo;
  UINT8               ManufacturerRef;
  UINT8               ProductNameRef;
  UINT8               VersionRef;
  UINT8               SerialNumRef;
  UINT8               SkuNumRef;
  UINT8               FamilyRef;
  CHAR8               *OptionalStrings;
  SMBIOS_TABLE_TYPE1  *SmbiosRecord;
  UINTN               SmbiosRecordSize;
  STRING_TABLE        StrTable;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve system info from CM object
  //
  *Table = NULL;
  Status = GetEStdObjSystemInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &SystemInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get System CM Object %r\n",
      Status
      ));
    return Status;
  }

  //
  // Copy strings to SMBIOS table
  //
  Status = StringTableInitialize (&StrTable, 6);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
    return Status;
  }

  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->Manufacturer, ManufacturerRef);
  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->ProductName, ProductNameRef);
  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->Version, VersionRef);
  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->SerialNum, SerialNumRef);
  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->SkuNum, SkuNumRef);
  STRING_TABLE_ADD_STRING (StrTable, SystemInfo->Family, FamilyRef);

  SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE1) +
                     StringTableGetStringSetSize (&StrTable);
  SmbiosRecord = (SMBIOS_TABLE_TYPE1 *)AllocateZeroPool (SmbiosRecordSize);
  if (SmbiosRecord == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  SmbiosRecord->Manufacturer = ManufacturerRef;
  SmbiosRecord->ProductName  = ProductNameRef;
  SmbiosRecord->Version      = VersionRef;
  SmbiosRecord->SerialNumber = SerialNumRef;
  SmbiosRecord->SKUNumber    = SkuNumRef;
  SmbiosRecord->Family       = FamilyRef;

  OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
  // publish the string set
  StringTablePublishStringSet (
    &StrTable,
    OptionalStrings,
    (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE1))
    );

  //
  // Fill in other fields of SMBIOS table
  //
  CopyGuid (&SmbiosRecord->Uuid, &SystemInfo->Uuid);
  SmbiosRecord->WakeUpType = SystemInfo->WakeUpType;

  //
  // Setup SMBIOS header
  //
  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE1);

  *Table      = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjToken = SystemInfo->SystemInfoToken;
  Status      = EFI_SUCCESS;

ErrorExit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type1 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType1Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType01),
  // Generator Description
  L"SMBIOS.TYPE1.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,
  // Build table function
  BuildSmbiosType1Table,
  // Free function
  FreeSmbiosType1Table,
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
SmbiosType1LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType1Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 1: Register Generator. Status = %r\n",
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
SmbiosType1LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType1Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type1: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
