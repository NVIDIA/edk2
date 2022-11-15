/** @file
  SMBIOS Type0 Table Generator.

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

/** This macro expands to a function that retrieves the System Slot
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceStandard,
  EStdObjBiosInfo,
  CM_STD_BIOS_INFO
  )

/** This function pointer describes the interface to used by the
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
EFI_STATUS
EFIAPI
FreeSmbiosType0Table (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST   This,
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
    Table Manager to build an SMBIOS table.

  @param [in]  Generator             Pointer to the SMBIOS table generator.
  @param [in]  SmbiosTableInfo       Pointer to the SMBIOS table information.
  @param [in]  TableFactoryProtocol  Pointer to the Table Factory protocol.
  @param [in]  CfgMgrProtocol        Pointer to the Configuration Manager
                                     Protocol interface.
  @param [out] Table                 Pointer to the SMBIOS table.
  @param [in]  CmObjectToken         Pointer to the CM object token.

  @return EFI_SUCCESS  If the table is generated successfully or other
                        failure codes as returned by the generator.
 **/
STATIC
EFI_STATUS
EFIAPI
BuildSmbiosType0Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               **Table,
  OUT       CM_OBJECT_TOKEN                                *CmObjectToken
  )
{
  EFI_STATUS          Status;
  CM_OBJECT_TOKEN     CmObject;
  UINT8               VendorRef;
  UINT8               VersionRef;
  UINT8               ReleaseDateRef;
  SMBIOS_TABLE_TYPE0  *SmbiosRecord;
  UINTN               SmbiosRecordSize;
  STRING_TABLE        StrTable;
  CHAR8               *OptionalStrings;
  CM_STD_BIOS_INFO    *BiosInfo;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);
  //
  // Retrieve system info from CM object
  //
  *Table = NULL;
  Status = GetEStdObjBiosInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &BiosInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a:Failed to get Bios Info CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  //
  // Copy strings to SMBIOS table
  //
  Status = StringTableInitialize (&StrTable, 3);
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a:Failed to allocate string table for CM Object, %r\n",
      __FUNCTION__,
      Status
      ));
    goto exit;
  }

  STRING_TABLE_ADD_STRING (StrTable, BiosInfo->BiosVendor, VendorRef);
  STRING_TABLE_ADD_STRING (StrTable, BiosInfo->BiosVersion, VersionRef);
  STRING_TABLE_ADD_STRING (StrTable, BiosInfo->BiosReleaseDate, ReleaseDateRef);

  SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE0) +
                     StringTableGetStringSetSize (&StrTable);
  SmbiosRecord = (SMBIOS_TABLE_TYPE0 *)AllocateZeroPool (SmbiosRecordSize);
  if (SmbiosRecord == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type0 record\n", __FUNCTION__));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  SmbiosRecord->BiosSegment = BiosInfo->BiosSegment;
  SmbiosRecord->BiosSize    = BiosInfo->BiosSize;

  SmbiosRecord->BiosCharacteristics = BiosInfo->BiosCharacteristics;

  SmbiosRecord->BIOSCharacteristicsExtensionBytes[0] = BiosInfo->BIOSCharacteristicsExtensionBytes[0];
  SmbiosRecord->BIOSCharacteristicsExtensionBytes[1] = BiosInfo->BIOSCharacteristicsExtensionBytes[1];

  // Add System Bios Major release
  SmbiosRecord->SystemBiosMajorRelease = BiosInfo->SystemBiosMajorRelease;
  SmbiosRecord->SystemBiosMinorRelease = BiosInfo->SystemBiosMinorRelease;

  SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = BiosInfo->ECFirmwareMajorRelease;
  SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = BiosInfo->ECFirmwareMinorRelease;

  SmbiosRecord->ExtendedBiosSize = BiosInfo->ExtendedBiosSize;

  SmbiosRecord->Vendor          = VendorRef;
  SmbiosRecord->BiosVersion     = VersionRef;
  SmbiosRecord->BiosReleaseDate = ReleaseDateRef;

  OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
  // publish the string set
  StringTablePublishStringSet (
    &StrTable,
    OptionalStrings,
    (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE0))
    );
  // setup the header
  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE0);
  CmObject                 = BiosInfo->BiosInfoToken;

  *Table         = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjectToken = CmObject;

exit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type00 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType0Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType00),
  // Generator Description
  L"SMBIOS.TYPE0.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_BIOS_INFORMATION,
  // Build table function
  BuildSmbiosType0Table,
  // Free function
  FreeSmbiosType0Table,
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
SmbiosType0LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType0Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 0: Register Generator. Status = %r\n",
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
SmbiosType0LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType0Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type0: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
