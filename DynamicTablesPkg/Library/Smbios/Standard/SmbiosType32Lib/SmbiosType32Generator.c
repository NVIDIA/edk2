/** @file
  SMBIOS Type 32 Table Generator.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  Copyright (c) 2020 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

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
  ESmbiosObjSystemBootInfo,
  CM_SMBIOS_SYSTEM_BOOT_INFO
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
EFI_STATUS
EFIAPI
FreeSmbiosType32Table (
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
EFI_STATUS
EFIAPI
BuildSmbiosType32Table (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               **Table,
  OUT       CM_OBJECT_TOKEN                                *CmObjectToken
  )
{
  EFI_STATUS                  Status;
  CM_SMBIOS_SYSTEM_BOOT_INFO  *SystemBootInfo;
  SMBIOS_TABLE_TYPE32         *SmbiosRecord;
  UINT32                      CmObjCount;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve System Boot Info from CM object
  //
  *Table = NULL;

  Status = EFI_SUCCESS;

  Status = GetESmbiosObjSystemBootInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &SystemBootInfo,
             &CmObjCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get System Boot Info CM Object %r\n",
      Status
      ));
    return Status;
  }

  SmbiosRecord = (SMBIOS_TABLE_TYPE32 *)AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE32) + 2);
  if (SmbiosRecord == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type32 record\n", __FUNCTION__));
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  // Per the SMBIOS specification, there are 6 reserved bytes to be set to 0x00
  SmbiosRecord->Reserved[0] = 0x00;
  SmbiosRecord->Reserved[1] = 0x00;
  SmbiosRecord->Reserved[2] = 0x00;
  SmbiosRecord->Reserved[3] = 0x00;
  SmbiosRecord->Reserved[4] = 0x00;
  SmbiosRecord->Reserved[5] = 0x00;

  SmbiosRecord->BootStatus = SystemBootInfo->BootStatus;
  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE32);

  *Table         = (SMBIOS_STRUCTURE *)SmbiosRecord;
  *CmObjectToken = SystemBootInfo->SystemBootInfoToken;

  return Status;
}

/** The interface for the SMBIOS Type32 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType32Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType32),
  // Generator Description
  L"SMBIOS.TYPE32.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION,
  // Build table function
  BuildSmbiosType32Table,
  // Free function
  FreeSmbiosType32Table,
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
SmbiosType32LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType32Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 32: Register Generator. Status = %r\n",
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
SmbiosType32LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType32Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type32: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
