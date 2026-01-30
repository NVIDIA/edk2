/** @file
  SMBIOS Type38 Table Generator.

  Copyright (c) 2024 - 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
  EObjNameSpaceSmbios,
  ESmbiosObjIpmiDeviceInfo,
  CM_SMBIOS_IPMI_DEVICE_INFO
  )

/** This function pointer describes the interface to use by the
    Table Manager to give the generator an opportunity to free
    any resources allocated for building the SMBIOS table.

  @param [in]  Generator       Pointer to the SMBIOS table generator.
  @param [in]  SmbiosTableInfo Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol  Pointer to the Configuration Manager
                               Protocol interface.
  @param [in]  Table           Pointer to the array of generated SMBIOS tables.
  @param [in]  CmObjectToken   Pointer to the array of CM object tokens.
  @param [in]  TableCount      Number of tables in the array.

  @return  EFI_SUCCESS If freed successfully or other failure codes
                        as returned by the generator.
**/
STATIC
EFI_STATUS
FreeSmbiosType38TableEx (
  IN      CONST SMBIOS_TABLE_GENERATOR                *CONST    Generator,
  IN      CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST    TableFactoryProtocol,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST    SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST    CfgMgrProtocol,
  IN      SMBIOS_STRUCTURE                            ***CONST  Table,
  IN      CM_OBJECT_TOKEN                                       **CmObjectToken,
  IN      CONST UINTN                                           TableCount
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
    Table Manager to build an SMBIOS table Type38.
  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.
  @param [in]  Generator       Pointer to the SMBIOS table generator.
  @param [in]  SmbiosTableInfo Pointer to the SMBIOS table information.
  @param [in]  CfgMgrProtocol  Pointer to the Configuration Manager
                               Protocol interface.
  @param [out] Table           Pointer to the array of SMBIOS tables.
  @param [out] CmObjectToken   Pointer to the array of CM object tokens.
  @param [out] TableCount      Number of tables generated.
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
BuildSmbiosType38TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                              ***Table,
  OUT       CM_OBJECT_TOKEN                               **CmObjectToken,
  OUT       UINTN                                 *CONST  TableCount
  )
{
  EFI_STATUS                  Status;
  UINT32                      NumIpmiDevices;
  SMBIOS_STRUCTURE            **TableList;
  CM_OBJECT_TOKEN             *CmObjectList;
  CM_SMBIOS_IPMI_DEVICE_INFO  *IpmiDeviceInfo;
  UINTN                       Index;
  SMBIOS_TABLE_TYPE38         *SmbiosRecord;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  *Table         = NULL;
  *CmObjectToken = NULL;

  Status = GetESmbiosObjIpmiDeviceInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &IpmiDeviceInfo,
             &NumIpmiDevices
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to get IPMI Device CM Object %r\n",
      __FUNCTION__,
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumIpmiDevices);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for IPMI device table list\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType38TableEx
  *Table      = TableList;
  *TableCount = NumIpmiDevices;
  for (Index = 0; Index < NumIpmiDevices; Index++) {
    TableList[Index] = NULL;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN) * NumIpmiDevices);
  if (CmObjectList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for CmObjectList\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  // Init pointers for FreeSmbiosType38TableEx
  *CmObjectToken = CmObjectList;

  for (Index = 0; Index < NumIpmiDevices; Index++) {
    //
    // Two zeros following the last string.
    //
    SmbiosRecord = AllocateZeroPool (sizeof (SMBIOS_TABLE_TYPE38) + 2);
    if (SmbiosRecord == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type38 record\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE38);

    //
    // Fill in other fields of SMBIOS table
    //
    SmbiosRecord->InterfaceType                     = IpmiDeviceInfo[Index].IpmiIntfType;
    SmbiosRecord->IPMISpecificationRevision         = IpmiDeviceInfo[Index].IpmiSpecRevision;
    SmbiosRecord->I2CSlaveAddress                   = IpmiDeviceInfo[Index].IpmiI2CSlaveAddress;
    SmbiosRecord->NVStorageDeviceAddress            = IpmiDeviceInfo[Index].IpmiNVStorageDevAddress;
    SmbiosRecord->BaseAddress                       = IpmiDeviceInfo[Index].IpmiBaseAddress;
    SmbiosRecord->BaseAddressModifier_InterruptInfo = IpmiDeviceInfo[Index].IpmiBaseAddModIntInfo;
    SmbiosRecord->InterruptNumber                   = IpmiDeviceInfo[Index].IpmiInterruptNum;

    TableList[Index]    = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index] = IpmiDeviceInfo[Index].IpmiDeviceInfoToken;
  }

  return EFI_SUCCESS;
}

/** The interface for the SMBIOS Type38 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType38Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType38),
  // Generator Description
  L"SMBIOS.TYPE38.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION,
  // Build table function (unused - using Extended).
  NULL,
  // Free function (unused - using Extended).
  NULL,
  // Build table function Extended.
  BuildSmbiosType38TableEx,
  // Free function Extended.
  FreeSmbiosType38TableEx
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
SmbiosType38LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType38Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type38: Register Generator. Status = %r\n",
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
SmbiosType38LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType38Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type38: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
