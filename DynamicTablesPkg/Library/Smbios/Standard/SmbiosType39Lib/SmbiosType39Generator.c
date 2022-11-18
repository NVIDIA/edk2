/** @file
  SMBIOS Type 39 Table Generator.

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

/** This macro expands to a function that retrieves the Power Supply
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjPowerSupplyInfo,
  CM_SMBIOS_POWER_SUPPLY_INFO
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
EFI_STATUS
EFIAPI
FreeSmbiosType39TableEx (
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
EFI_STATUS
EFIAPI
BuildSmbiosType39TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                   Status;
  UINT32                       PsuCount;
  SMBIOS_STRUCTURE             **TableList    = NULL;
  CM_OBJECT_TOKEN              *CmObjectList  = NULL;
  UINT32                       TableListCount = 0;
  CM_SMBIOS_POWER_SUPPLY_INFO  *PsuInfo;
  UINTN                        Index;
  UINT8                        LocationRef;
  UINT8                        DeviceNameRef;
  UINT8                        ManufacturerRef;
  UINT8                        SerialNumberRef;
  UINT8                        AssetTagNumberRef;
  UINT8                        ModelPartNumberRef;
  UINT8                        RevisionLevelRef;
  CHAR8                        *OptionalStrings;
  SMBIOS_TABLE_TYPE39          *SmbiosRecord;
  UINTN                        SmbiosRecordSize;
  STRING_TABLE                 StrTable;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  Status = GetESmbiosObjPowerSupplyInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &PsuInfo,
             &PsuCount
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Failed to get Power Supply Info CM Object %r\n", Status));
    goto ErrorExit;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * PsuCount);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to alloc memory for SMBIOS table 39\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * PsuCount);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to alloc memory for SMBIOS table 39\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  for (Index = 0; Index < PsuCount; Index++) {
    //
    // Copy strings to SMBIOS table
    //
    StringTableInitialize (&StrTable, 7);

    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].Location, LocationRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].DeviceName, DeviceNameRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].Manufacturer, ManufacturerRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].SerialNumber, SerialNumberRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].AssetTagNumber, AssetTagNumberRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].ModelPartNumber, ModelPartNumberRef);
    STRING_TABLE_ADD_STRING (StrTable, PsuInfo[Index].RevisionLevel, RevisionLevelRef);

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE39) +
                       StringTableGetStringSetSize (&StrTable);
    SmbiosRecord = (SMBIOS_TABLE_TYPE39 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SmbiosRecord->Location        = LocationRef;
    SmbiosRecord->DeviceName      = DeviceNameRef;
    SmbiosRecord->Manufacturer    = ManufacturerRef;
    SmbiosRecord->SerialNumber    = SerialNumberRef;
    SmbiosRecord->AssetTagNumber  = AssetTagNumberRef;
    SmbiosRecord->ModelPartNumber = ModelPartNumberRef;
    SmbiosRecord->RevisionLevel   = RevisionLevelRef;

    SmbiosRecord->PowerUnitGroup   = PsuInfo[Index].PowerUnitGroup;
    SmbiosRecord->MaxPowerCapacity = PsuInfo[Index].MaxPowerCapacity;

    SmbiosRecord->InputVoltageProbeHandle = TableFactoryProtocol->GetSmbiosHandleEx (
                                                                    CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType26),
                                                                    PsuInfo[Index].InputVoltageProbeCmToken
                                                                    );
    SmbiosRecord->CoolingDeviceHandle = TableFactoryProtocol->GetSmbiosHandleEx (
                                                                CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType27),
                                                                PsuInfo[Index].CoolingDeviceCmToken
                                                                );
    SmbiosRecord->InputCurrentProbeHandle = TableFactoryProtocol->GetSmbiosHandleEx (
                                                                    CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType29),
                                                                    PsuInfo[Index].InputCurrentProbeCmToken
                                                                    );

    CopyMem (
      &SmbiosRecord->PowerSupplyCharacteristics,
      &PsuInfo[Index].PowerSupplyCharacteristics,
      sizeof (SmbiosRecord->PowerSupplyCharacteristics)
      );

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
    // publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE39))
      );

    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE39);
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      = PsuInfo[Index].PowerSupplyInfoToken;

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

/** The interface for the SMBIOS Type39 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType39Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType39),
  // Generator Description
  L"SMBIOS.TYPE39.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY,
  NULL,
  NULL,
  // Build table function Extended.
  BuildSmbiosType39TableEx,
  // Free function Extended.
  FreeSmbiosType39TableEx
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
SmbiosType39LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType39Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 39: Register Generator. Status = %r\n",
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
SmbiosType39LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType39Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 39: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
