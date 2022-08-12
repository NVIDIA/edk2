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

// Module specific include files.
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Protocol/ConfigurationManagerProtocol.h>
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

// Default Values for Memory Device
STATIC SMBIOS_TABLE_TYPE17 MemDeviceInfoTemplate = {
  {                                      // Hdr
    EFI_SMBIOS_TYPE_MEMORY_DEVICE,       // Type
    sizeof (SMBIOS_TABLE_TYPE17),        // Length
    0                                    // Handle
  },
  0,                                     // MemoryArrayHandle
  0,                                     // MemoryErrorInformationHandle
  0xFFFF,                                // TotalWidth
  0xFFFF,                                // DataWIdth
  0x7FFF,                                // Size (always use Extended Size)
  MemoryTypeUnknown,                     // FormFactor
  0xFF,                                  // DeviceSet
  1,                                     // Device Locator
  2,                                     // Bank Locator
  MemoryTypeSdram,                       // MemoryType
  {                                      // TypeDetail
    0
  },
  0xFFFF,                                // Speed (Use Extended Speed)
  0,                                     // Manufacturer
                                         // (Unused Use ModuleManufactuerId)
  3,                                     // SerialNumber
  4,                                     // AssetTag
  0,                                     // PartNumber
                                         // (Unused Use ModuleProductId)
  0,                                     // Attributes
  0,                                     // ExtendedSize
  2000,                                  // ConfiguredMemoryClockSpeed
  0,                                     // MinimumVoltage
  0,                                     // MaximumVoltage
  0,                                     // ConfiguredVoltage
  MemoryTechnologyDram,                  // MemoryTechnology
  {                                      // MemoryOperatingModeCapability
    .Uint16 = 0x000
  },
  5,                                    // FirmwareVersion
  0,                                    // ModuleManufacturerId
  0,                                    // ModuleProductId
  0,                                    // MemorySubsystemContollerManufacturerId
  0,                                    // MemorySybsystemControllerProductId
  0,                                    // NonVolatileSize
  0,                                    // VolatileSize
  0,                                    // CacheSize
  0,                                    // LogicalSize
  0,                                    // ExtendedSpeed
  0,                                    // ExtendedConfiguredMemorySpeed
};

STATIC CHAR8  UnknownStr[] = "Unknown\0";

STATIC
EFI_STATUS
EFIAPI
FreeSmbiosType17TableEx (
  IN      CONST SMBIOS_TABLE_GENERATOR                   *CONST    This,
  IN      CONST CM_STD_OBJ_SMBIOS_TABLE_INFO             *CONST    SmbiosTableInfo,
  IN      CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL     *CONST    CfgMgrProtocol,
  IN OUT        SMBIOS_STRUCTURE                         ***CONST  Table,
  IN      CONST UINTN                                              TableCount
  )
{
  return EFI_SUCCESS;
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
  IN        CM_STD_OBJ_SMBIOS_TABLE_INFO           *CONST  SmbiosTableInfo,
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL   *CONST  CfgMgrProtocol,
  OUT       SMBIOS_STRUCTURE                               ***Table,
  OUT       UINTN                                  *CONST  TableCount
  )
{
  EFI_STATUS                 Status;
  UINT32                     NumMemDevices;
  SMBIOS_STRUCTURE           **TableList;
  CM_ARM_MEMORY_DEVICE_INFO  *MemoryDevicesInfo;
  UINTN                      Index;
  UINTN                      SerialNumLen;
  CHAR8                      *SerialNum;
  UINTN                      AssetTagLen;
  CHAR8                      *AssetTag;
  UINTN                      DeviceLocatorLen;
  CHAR8                      *DeviceLocator;
  UINTN                      BankLocatorLen;
  CHAR8                      *BankLocator;
  UINTN                      FirmwareVersionLen;
  CHAR8                      *FirmwareVersion;
  CHAR8                      *OptionalStrings;
  SMBIOS_TABLE_TYPE17        *SmbiosRecord;

  ASSERT (This != NULL);
  ASSERT (SmbiosTableInfo != NULL);
  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Table != NULL);
  ASSERT (TableCount != NULL);
  ASSERT (SmbiosTableInfo->TableGeneratorId == This->GeneratorID);

  DEBUG ((DEBUG_ERROR, "%a : Start \n", __FUNCTION__));
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

  for (Index = 0; Index < NumMemDevices; Index++) {
    if (MemoryDevicesInfo[Index].SerialNum != NULL) {
      SerialNumLen = AsciiStrLen (MemoryDevicesInfo[Index].SerialNum);
      SerialNum    = MemoryDevicesInfo[Index].SerialNum;
    } else {
      SerialNumLen = AsciiStrLen (UnknownStr);
      SerialNum    = UnknownStr;
    }

    if (MemoryDevicesInfo[Index].AssetTag != NULL) {
      AssetTagLen = AsciiStrLen (MemoryDevicesInfo[Index].AssetTag);
      AssetTag    = MemoryDevicesInfo[Index].AssetTag;
    } else {
      AssetTagLen = AsciiStrLen (UnknownStr);
      AssetTag    = UnknownStr;
    }

    if (MemoryDevicesInfo[Index].DeviceLocator != NULL) {
      DeviceLocatorLen = AsciiStrLen (MemoryDevicesInfo[Index].DeviceLocator);
      DeviceLocator    = MemoryDevicesInfo[Index].DeviceLocator;
    } else {
      DeviceLocatorLen = AsciiStrLen (UnknownStr);
      DeviceLocator    = UnknownStr;
    }

    if (MemoryDevicesInfo[Index].BankLocator != NULL) {
      BankLocatorLen = AsciiStrLen (MemoryDevicesInfo[Index].BankLocator);
      BankLocator    = MemoryDevicesInfo[Index].BankLocator;
    } else {
      BankLocatorLen = AsciiStrLen (UnknownStr);
      BankLocator    = UnknownStr;
    }

    if (MemoryDevicesInfo[Index].FirmwareVersion != NULL) {
      FirmwareVersionLen = AsciiStrLen (MemoryDevicesInfo[Index].FirmwareVersion);
      FirmwareVersion    = MemoryDevicesInfo[Index].FirmwareVersion;
    } else {
      FirmwareVersionLen = AsciiStrLen (UnknownStr);
      FirmwareVersion    = UnknownStr;
    }

    SmbiosRecord = (SMBIOS_TABLE_TYPE17 *)AllocateZeroPool (
                                            sizeof (SMBIOS_TABLE_TYPE17) +
                                            SerialNumLen + 1 +
                                            AssetTagLen + 1 + DeviceLocatorLen + 1 +
                                            BankLocatorLen + 1 + FirmwareVersionLen + 1 + 1
                                            );
    if (SmbiosRecord == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto exit;
    }

    CopyMem (SmbiosRecord, &MemDeviceInfoTemplate, sizeof (SMBIOS_TABLE_TYPE17));
    SmbiosRecord->ExtendedSize         = MemoryDevicesInfo[Index].Size;
    SmbiosRecord->DeviceSet            = MemoryDevicesInfo[Index].DeviceSet;
    SmbiosRecord->ModuleManufacturerID =
      MemoryDevicesInfo[Index].ModuleManufacturerId;
    SmbiosRecord->ModuleProductID =
      MemoryDevicesInfo[Index].ModuleProductId;
    SmbiosRecord->Attributes =
      MemoryDevicesInfo[Index].Attributes;
    SmbiosRecord->ExtendedSpeed = MemoryDevicesInfo[Index].Speed;
    OptionalStrings             = (CHAR8 *)(SmbiosRecord + 1);
    AsciiSPrint (OptionalStrings, DeviceLocatorLen + 1, DeviceLocator);
    OptionalStrings = OptionalStrings + DeviceLocatorLen + 1;
    AsciiSPrint (OptionalStrings, BankLocatorLen + 1, BankLocator);
    OptionalStrings = OptionalStrings + BankLocatorLen + 1;
    AsciiSPrint (OptionalStrings, SerialNumLen + 1, SerialNum);
    OptionalStrings = OptionalStrings + SerialNumLen + 1;
    AsciiSPrint (OptionalStrings, AssetTagLen + 1, AssetTag);
    OptionalStrings = OptionalStrings + AssetTagLen + 1;
    AsciiSPrint (OptionalStrings, FirmwareVersionLen + 1, FirmwareVersion);
    OptionalStrings  = OptionalStrings + FirmwareVersionLen + 1;
    TableList[Index] = (SMBIOS_STRUCTURE *)SmbiosRecord;
  }

  *Table      = TableList;
  *TableCount = NumMemDevices;

exit:
  DEBUG ((DEBUG_ERROR, "%a : Done \n", __FUNCTION__));
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
