/** @file
  SMBIOS Type 43 Table Generator.

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
#include <IndustryStandard/SmBios.h>

/** This macro expands to a function that retrieves the TPM device
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjTpmDeviceInfo,
  CM_STD_TPM_DEVICE_INFO
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
FreeSmbiosType43Table (
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
BuildSmbiosType43Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                              **Table,
  OUT       CM_OBJECT_TOKEN                               *CmObjToken
  )
{
  EFI_STATUS              Status;
  CM_STD_TPM_DEVICE_INFO  *TpmInfo;
  UINT8                   DescriptionRef;
  CHAR8                   *OptionalStrings;
  SMBIOS_TABLE_TYPE43     *SmbiosRecord;
  UINTN                   SmbiosRecordSize;
  STRING_TABLE            StrTable;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve TPM info from CM object
  //
  *Table = NULL;
  Status = GetEStdObjTpmDeviceInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &TpmInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get TPM CM Object %r\n",
      Status
      ));
    return Status;
  }

  //
  // Copy strings to SMBIOS table
  //
  Status = StringTableInitialize (&StrTable, 1);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
    return Status;
  }

  STRING_TABLE_ADD_STRING (StrTable, TpmInfo->Description, DescriptionRef);

  SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE43) +
                     StringTableGetStringSetSize (&StrTable);
  SmbiosRecord = (SMBIOS_TABLE_TYPE43 *)AllocateZeroPool (SmbiosRecordSize);
  if (SmbiosRecord == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
  // publish the string set
  StringTablePublishStringSet (
    &StrTable,
    OptionalStrings,
    (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE43))
    );

  //
  // Fill in the SMBIOS table fields
  //
  CopyMem (
    &SmbiosRecord->VendorID,
    &TpmInfo->VendorID,
    sizeof (SmbiosRecord->VendorID)
    );
  SmbiosRecord->MajorSpecVersion = TpmInfo->MajorSpecVersion;
  SmbiosRecord->MinorSpecVersion = TpmInfo->MinorSpecVersion;
  SmbiosRecord->FirmwareVersion1 = TpmInfo->FirmwareVersion1;
  SmbiosRecord->FirmwareVersion2 = TpmInfo->FirmwareVersion2;
  SmbiosRecord->Description      = DescriptionRef;
  SmbiosRecord->Characteristics  = TpmInfo->Characteristics;
  SmbiosRecord->OemDefined       = TpmInfo->OemDefined;

  //
  // Setup SMBIOS header
  //
  SmbiosRecord->Hdr.Type   = SMBIOS_TYPE_TPM_DEVICE;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE43);

  *Table      = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjToken = TpmInfo->TpmDeviceInfoToken;
  Status      = EFI_SUCCESS;

ErrorExit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type43 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType43Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType43),
  // Generator Description
  L"SMBIOS.TYPE43.GENERATOR",
  // SMBIOS Table Type
  SMBIOS_TYPE_TPM_DEVICE,
  // Build table function
  BuildSmbiosType43Table,
  // Free function
  FreeSmbiosType43Table,
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
SmbiosType43LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType43Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 43: Register Generator. Status = %r\n",
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
SmbiosType43LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType43Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type43: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
