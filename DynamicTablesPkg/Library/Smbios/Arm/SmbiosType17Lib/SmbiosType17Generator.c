/** @file
  SMBIOS Type17 Table Generator.

  Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/** This macro expands to a function that retrieves the Memory Device
    information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjMemoryDeviceInfo,
  CM_ARM_MEMORY_DEVICE_INFO
  )

#define EXTENDED_SIZE_THRESHOLD     (0x7FFF00000LL)
#define SIZE_GRANULARITY_THRESHOLD  (0x100000L)
#define SIZE_GRANULARITY_BITMASK    (0x8000)
#define EXTENDED_SPEED_THRESHOLD    (0xFFFF)

STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType17TableEx (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST    This,
  IN      CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL     *CONST  TableFactoryProtocol,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO             *CONST    SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL     *CONST    CfgMgrProtocol,
  IN      SMBIOS_STRUCTURE                             ***CONST    Table,
  IN      CM_OBJECT_TOKEN                                          **CmObjectToken,
  IN      CONST UINTN                                              TableCount
  )
{
  UINTN Index;
  SMBIOS_STRUCTURE           **TableList;

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
EFI_STATUS
AddPhysArrHandle (
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN  CM_OBJECT_TOKEN                                      CmObjToken,
  OUT SMBIOS_TABLE_TYPE17                                  *SmbiosRecord
  )
{
  EFI_STATUS                       Status;
  EFI_SMBIOS_PROTOCOL              *SmbiosProtocol;
  SMBIOS_HANDLE                    PhysMemArrHandle;
  SMBIOS_HANDLE_MAP                *HandleMap;
  CONST SMBIOS_TABLE_GENERATOR     *Generator;

  PhysMemArrHandle = 0;
  // Check if there is an existing handle.
  HandleMap = TableFactoryProtocol->GetSmbiosHandle (CmObjToken);
  if (HandleMap == NULL) {
    // If a Handle isn't found, check if there is a generator for Type 16,
    Status = TableFactoryProtocol->GetSmbiosTableGenerator (TableFactoryProtocol, EStdSmbiosTableIdType16, &Generator);
    // If there isn't a generator for the required Table, then proceed
    // without it.
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Type16 Generator not Found %r\n",
                           __FUNCTION__, Status));
      Status = EFI_SUCCESS;
    } else {
      // If there is a generator for Type16 then create an SmbiosHandle for it.
      Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&SmbiosProtocol);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Could not locate SMBIOS protocol.  %r\n", Status));
        return Status;
      }
      PhysMemArrHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status = TableFactoryProtocol->AddSmbiosHandle (SmbiosProtocol, &PhysMemArrHandle, CmObjToken, EStdSmbiosTableIdType16);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Smbios Handle %r\n", __FUNCTION__, Status));
        return Status;
      }
    }
  } else {
    PhysMemArrHandle = HandleMap->SmbiosTableId;
    DEBUG ((DEBUG_INFO, "%a: Re-using Handle %u\n" , __FUNCTION__, PhysMemArrHandle));
  }

  SmbiosRecord->MemoryArrayHandle = PhysMemArrHandle;
  return Status;
}

STATIC
VOID
UpdateSmbiosType17Size (
  IN  UINT64                Size,
  OUT SMBIOS_TABLE_TYPE17   *SmbiosRecord
  )
{
  if (Size < SIZE_GRANULARITY_THRESHOLD) {
    SmbiosRecord->Size = Size / 1024;
    SmbiosRecord->Size |= SIZE_GRANULARITY_BITMASK;
  } else if (Size >= EXTENDED_SIZE_THRESHOLD) {
    SmbiosRecord->Size = 0x7FFF;
    SmbiosRecord->ExtendedSize = (Size / 1024 / 1024);
  } else {
    SmbiosRecord->Size = (Size / 1024 / 1024);
  }
}

STATIC
VOID
UpdateSmbiosType17Speed (
  IN  UINT32                Speed,
  OUT SMBIOS_TABLE_TYPE17   *SmbiosRecord
  )
{
  if (Speed >- EXTENDED_SPEED_THRESHOLD) {
    SmbiosRecord->Speed = EXTENDED_SPEED_THRESHOLD;
    SmbiosRecord->ExtendedSpeed = Speed;
  } else {
    SmbiosRecord->Speed = Speed;
  }
}

/** Construct SMBIOS Type17 Table describing memory devices.

  If this function allocates any resources then they must be freed
  in the FreeXXXXTableResources function.

  @param [in]  This            Pointer to the SMBIOS table generator.
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
BuildSmbiosType17TableEx (
  IN  CONST SMBIOS_TABLE_GENERATOR                         *This,
  IN  CONST EDKII_DYNAMIC_TABLE_FACTORY_PROTOCOL   *CONST  TableFactoryProtocol,
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       CM_OBJECT_TOKEN                                **CmObjectToken,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                 Status;
  UINT32                     NumMemDevices;
  SMBIOS_STRUCTURE           **TableList;
  CM_OBJECT_TOKEN            *CmObjectList;
  CM_ARM_MEMORY_DEVICE_INFO  *MemoryDevicesInfo;
  UINTN                      Index;
  UINT8                      SerialNumRef;
  UINT8                      AssetTagRef;
  UINT8                      DeviceLocatorRef;
  UINT8                      BankLocatorRef;
  UINT8                      FirmwareVersionRef;
  CHAR8                      *OptionalStrings;
  SMBIOS_TABLE_TYPE17        *SmbiosRecord;
  UINTN                      SmbiosRecordSize;
  STRING_TABLE               StrTable;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  *Table = NULL;
  Status = GetEArmObjMemoryDeviceInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &MemoryDevicesInfo,
             &NumMemDevices
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "Failed to get Memory Devices CM Object %r\n",
      Status
      ));
    return Status;
  }

  TableList = (SMBIOS_STRUCTURE **)AllocateZeroPool (sizeof (SMBIOS_STRUCTURE *) * NumMemDevices);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to alloc memory for %u devices table\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  CmObjectList = (CM_OBJECT_TOKEN *)AllocateZeroPool (sizeof (CM_OBJECT_TOKEN *) * NumMemDevices);
  if (TableList == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to alloc memory for %u devices table\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto exit;
  }

  for (Index = 0; Index < NumMemDevices; Index++) {
    StringTableInitialize (&StrTable, 7);

    SerialNumRef = 0;
    AssetTagRef = 0;
    DeviceLocatorRef = 0;
    BankLocatorRef = 0;
    FirmwareVersionRef = 0;

    if (MemoryDevicesInfo[Index].DeviceLocator != NULL) {
      Status = StringTableAddString (&StrTable,
                                  MemoryDevicesInfo[Index].DeviceLocator,
                                  &DeviceLocatorRef);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to add DeviceLocator String %r \n", Status));
      }
    }
    if (MemoryDevicesInfo[Index].BankLocator != NULL) {
      Status = StringTableAddString (&StrTable,
                                  MemoryDevicesInfo[Index].BankLocator,
                                  &BankLocatorRef);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to BankLocator String %r \n", Status));
      }
    }

    if (MemoryDevicesInfo[Index].SerialNum != NULL) {
      Status = StringTableAddString (&StrTable,
                                  MemoryDevicesInfo[Index].SerialNum,
                                  &SerialNumRef);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to add SerialNum String %r \n", Status));
      }
    }

    if (MemoryDevicesInfo[Index].AssetTag != NULL) {
      Status = StringTableAddString (&StrTable,
                                  MemoryDevicesInfo[Index].AssetTag,
                                  &AssetTagRef);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to add Asset Tag String %r \n", Status));
      }
    }

    if (MemoryDevicesInfo[Index].FirmwareVersion != NULL) {
      Status = StringTableAddString (&StrTable,
                                  MemoryDevicesInfo[Index].FirmwareVersion,
                                  &FirmwareVersionRef);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to add Asset Tag String %r \n", Status));
      }
    }


    SmbiosRecordSize = sizeof (SMBIOS_TABLE_TYPE17) +
                       StringTableGetStringSetSize(&StrTable);
    SmbiosRecord = (SMBIOS_TABLE_TYPE17 *)AllocateZeroPool (SmbiosRecordSize);
    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto exit;
    }

    UpdateSmbiosType17Size (MemoryDevicesInfo[Index].Size, SmbiosRecord);
    UpdateSmbiosType17Speed (MemoryDevicesInfo[Index].Speed, SmbiosRecord);
    SmbiosRecord->ExtendedSize         = MemoryDevicesInfo[Index].Size;
    SmbiosRecord->DeviceSet            = MemoryDevicesInfo[Index].DeviceSet;
    SmbiosRecord->ModuleManufacturerID =
      MemoryDevicesInfo[Index].ModuleManufacturerId;
    SmbiosRecord->ModuleProductID =
      MemoryDevicesInfo[Index].ModuleProductId;
    SmbiosRecord->Attributes =
      MemoryDevicesInfo[Index].Attributes;
    SmbiosRecord->ExtendedSpeed = MemoryDevicesInfo[Index].Speed;
    // Is there a reference to a Physical Array Device.
    if (MemoryDevicesInfo[Index].PhysicalArrayToken != CM_NULL_TOKEN) {
      Status = AddPhysArrHandle (TableFactoryProtocol,
                                 MemoryDevicesInfo[Index].PhysicalArrayToken,
                                 SmbiosRecord);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to add Type16 Handle %r\n", __FUNCTION__, Status));
        return Status;
      }
    }

    SmbiosRecord->DeviceLocator   = DeviceLocatorRef;
    SmbiosRecord->BankLocator     = BankLocatorRef;
    SmbiosRecord->AssetTag        = AssetTagRef;
    SmbiosRecord->SerialNumber    = SerialNumRef;
    SmbiosRecord->FirmwareVersion = FirmwareVersionRef;
    OptionalStrings             = (CHAR8 *)(SmbiosRecord + 1);
    // publish the string set
    StringTablePublishStringSet (
      &StrTable,
      OptionalStrings,
      (SmbiosRecordSize - sizeof(SMBIOS_TABLE_TYPE17))
    );

    // setup the header
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE17);
    TableList[Index] = (SMBIOS_STRUCTURE *)SmbiosRecord;
    CmObjectList[Index] = MemoryDevicesInfo[Index].MemoryDeviceInfoToken;
  }

  *Table      = TableList;
  *CmObjectToken = CmObjectList;
  *TableCount = NumMemDevices;

exit:
  // free string table
  StringTableFree (&StrTable);
  return Status;
}

/** The interface for the SMBIOS Type17 Table Generator.
*/
STATIC
CONST
SMBIOS_TABLE_GENERATOR  SmbiosType17Generator = {
  // Generator ID
  CREATE_STD_SMBIOS_TABLE_GEN_ID (EStdSmbiosTableIdType17),
  // Generator Description
  L"SMBIOS.TYPE17.GENERATOR",
  // SMBIOS Table Type
  EFI_SMBIOS_TYPE_MEMORY_DEVICE,
  NULL,
  NULL,
  // Build table function Extended.
  BuildSmbiosType17TableEx,
  // Free function Extended.
  FreeSmbiosType17TableEx
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
SmbiosType17LibConstructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = RegisterSmbiosTableGenerator (&SmbiosType17Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type 17: Register Generator. Status = %r\n",
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
SmbiosType17LibDestructor (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = DeregisterSmbiosTableGenerator (&SmbiosType17Generator);
  DEBUG ((
    DEBUG_INFO,
    "SMBIOS Type17: Deregister Generator. Status = %r\n",
    Status
    ));
  ASSERT_EFI_ERROR (Status);
  return Status;
}
