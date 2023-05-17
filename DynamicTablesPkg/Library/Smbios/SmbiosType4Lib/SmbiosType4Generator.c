/** @file
  SMBIOS Type4 Table Generator.

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

#define SMBIOS_TYPE4_MAX_STRINGS  (6)

/** This macro expands to a function that retrieves the Memory Device
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceSmbios,
  ESmbiosObjProcessorInfo,
  CM_SMBIOS_PROCESSOR_INFO
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
FreeSmbiosType4TableEx (
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

STATIC
VOID
AddCacheInfoHandle (
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN  CM_OBJECT_TOKEN                                      CmObjToken1,
  IN  CM_OBJECT_TOKEN                                      CmObjToken2,
  IN  CM_OBJECT_TOKEN                                      CmObjToken3,
  OUT SMBIOS_TABLE_TYPE4                                   *SmbiosRecord
  )
{
  EFI_SMBIOS_HANDLE  CacheInfoHandle;
  SMBIOS_HANDLE_MAP  *HandleMap;

  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken1);
  if (HandleMap == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:Failed to get SMBIOS Handle\n", __FUNCTION__));
    CacheInfoHandle = 0xFFFF;
  } else {
    CacheInfoHandle = HandleMap->SmbiosTblHandle;
  }

  SmbiosRecord->L1CacheHandle = CacheInfoHandle;

  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken2);
  if (HandleMap == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:Failed to get SMBIOS Handle\n", __FUNCTION__));
    CacheInfoHandle = 0xFFFF;
  } else {
    CacheInfoHandle = HandleMap->SmbiosTblHandle;
  }

  SmbiosRecord->L2CacheHandle = CacheInfoHandle;

  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken3);
  if (HandleMap == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:Failed to get SMBIOS Handle\n", __FUNCTION__));
    CacheInfoHandle = 0xFFFF;
  } else {
    CacheInfoHandle = HandleMap->SmbiosTblHandle;
  }

  SmbiosRecord->L3CacheHandle = CacheInfoHandle;
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
BuildSmbiosType4TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                        *Generator,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL  *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO          *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                              ***Table,
  OUT       CM_OBJECT_TOKEN                               **CmObjToken,
  OUT       UINTN                                 *CONST  TableCount
  )
{
  EFI_STATUS                Status;
  CM_SMBIOS_PROCESSOR_INFO  *ProcessorInfo;
  UINT8                     SocketDesignationRef;
  UINT8                     ProcessorManufacturerRef;
  UINT8                     ProcessorVersionRef;
  UINT8                     SerialNumberRef;
  UINT8                     AssetTagRef;
  UINT8                     PartNumberRef;
  CHAR8                     *OptionalStrings;
  SMBIOS_TABLE_TYPE4        *SmbiosRecord;
  UINTN                     SmbiosRecordSize;
  STRING_TABLE              StrTable;
  UINT32                    NumProcessors;
  SMBIOS_STRUCTURE          **TableList;
  CM_OBJECT_TOKEN           *CmObjectList;
  UINTN                     Index;

  ASSERT (Generator != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == Generator->GeneratorID);

  //
  // Retrieve processor info from CM object
  //
  *Table = NULL;
  Status = GetESmbiosObjProcessorInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &ProcessorInfo,
             &NumProcessors
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Processor CM Object %r\n",
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumProcessors);
  if (TableList == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to alloc memory for %u devices table\n",
      __FUNCTION__,
      NumProcessors
      ));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumProcessors);
  if (CmObjectList == NULL) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Failed to alloc memory for %u CMObjects\n",
      __FUNCTION__,
      NumProcessors
      ));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  for (Index = 0; Index < NumProcessors; Index++) {
    //
    // Copy strings to SMBIOS table
    //
    Status = StringTableInitialize (&StrTable, SMBIOS_TYPE4_MAX_STRINGS);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to initialize string table %r\n", __FUNCTION__, Status));
      return Status;
    }

    if (ProcessorInfo[Index].SocketDesignation != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].SocketDesignation, SocketDesignationRef);
    }

    if (ProcessorInfo[Index].ProcessorManufacturer != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].ProcessorManufacturer, ProcessorManufacturerRef);
    }

    if (ProcessorInfo[Index].ProcessorVersion != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].ProcessorVersion, ProcessorVersionRef);
    }

    if (ProcessorInfo[Index].SerialNumber != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].SerialNumber, SerialNumberRef);
    }

    if (ProcessorInfo[Index].AssetTag != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].AssetTag, AssetTagRef);
    }

    if (ProcessorInfo[Index].PartNumber != NULL) {
      STRING_TABLE_ADD_STRING (StrTable, ProcessorInfo[Index].PartNumber, PartNumberRef);
    }

    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE4) +
                       StringTableGetStringSetSize (&StrTable);
    SmbiosRecord = (SMBIOS_TABLE_TYPE4 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: memory allocation failed for smbios type4 record\n", __FUNCTION__));
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }

    SmbiosRecord->Socket                = SocketDesignationRef;
    SmbiosRecord->ProcessorManufacturer = ProcessorManufacturerRef;
    SmbiosRecord->ProcessorVersion      = ProcessorVersionRef;
    SmbiosRecord->SerialNumber          = SerialNumberRef;
    SmbiosRecord->AssetTag              = AssetTagRef;
    SmbiosRecord->PartNumber            = PartNumberRef;

    OptionalStrings = (CHAR8 *)(SmbiosRecord + 1);
    // publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof (SMBIOS_TABLE_TYPE4))
      );

    //
    // Fill in other fields of SMBIOS table
    //
    SmbiosRecord->ProcessorType            = ProcessorInfo[Index].ProcessorType;
    SmbiosRecord->ProcessorFamily          = ProcessorInfo[Index].ProcessorFamily;
    SmbiosRecord->ProcessorId              = ProcessorInfo[Index].ProcessorId;
    SmbiosRecord->Voltage                  = ProcessorInfo[Index].Voltage;
    SmbiosRecord->ExternalClock            = ProcessorInfo[Index].ExternalClock;
    SmbiosRecord->MaxSpeed                 = ProcessorInfo[Index].MaxSpeed;
    SmbiosRecord->CurrentSpeed             = ProcessorInfo[Index].CurrentSpeed;
    SmbiosRecord->Status                   = ProcessorInfo[Index].Status;
    SmbiosRecord->ProcessorUpgrade         = ProcessorInfo[Index].ProcessorUpgrade;
    SmbiosRecord->CoreCount                = ProcessorInfo[Index].CoreCount;
    SmbiosRecord->EnabledCoreCount         = ProcessorInfo[Index].EnabledCoreCount;
    SmbiosRecord->ThreadCount              = ProcessorInfo[Index].ThreadCount;
    SmbiosRecord->ProcessorCharacteristics = ProcessorInfo[Index].ProcessorCharacteristics;
    SmbiosRecord->ProcessorFamily2         = ProcessorInfo[Index].ProcessorFamily2;
    SmbiosRecord->CoreCount2               = ProcessorInfo[Index].CoreCount2;
    SmbiosRecord->EnabledCoreCount2        = ProcessorInfo[Index].EnabledCoreCount2;
    SmbiosRecord->ThreadCount2             = ProcessorInfo[Index].ThreadCount2;
    SmbiosRecord->ThreadEnabled            = ProcessorInfo[Index].ThreadEnabled;

    AddCacheInfoHandle (
      TableFactoryProtocol,
      ProcessorInfo[Index].CacheInfoTokenL1,
      ProcessorInfo[Index].CacheInfoTokenL2,
      ProcessorInfo[Index].CacheInfoTokenL3,
      SmbiosRecord
      );

    //
    // Setup SMBIOS header
    //
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE4);
    TableList[Index]         = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index]      = ProcessorInfo[Index].ProcessorInfoToken;
    StringTableFree (&StrTable);
  }

  *Table      = TableList;
  *CmObjToken = CmObjectList;
  *TableCount = NumProcessors;
  Status      = EFI_SUCCESS;

ErrorExit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type4 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType4Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType04),
  // Generator Description
  L"SMBIOS.TYPE4.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION,
  NULL,
  NULL,
  // Build table function
  BuildSmbiosType4TableEx,
  // Free function
  FreeSmbiosType4TableEx
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
SmbiosType4LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType4Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 4: Register Generator. Status = %r\n",
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
SmbiosType4LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType4Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type4: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
