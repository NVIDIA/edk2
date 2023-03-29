/** @file

  Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef SMBIOS_NAMESPACE_OBJECTS_H_
#define SMBIOS_NAMESPACE_OBJECTS_H_

#pragma pack(1)

typedef enum SmbiosObjectID {
  ESmbiosObjReserved,                  ///< 0  - Reserved
  ESmbiosObjBaseboardInfo,             ///< 1  - Baseboard Information
  ESmbiosObjSystemSlotInfo,            ///< 2  - System Slot Information
  ESmbiosObjSystemInfo,                ///< 3  - System Information
  ESmbiosObjTpmDeviceInfo,             ///< 4  - TPM Device Info
  ESmbiosObjOemStrings,                ///< 5  - OEM Strings
  ESmbiosObjPortConnectorInfo,         ///< 6  - Port connector Information
  ESmbiosObjBiosInfo,                  ///< 7 - Bios Information
  ESmbiosObjOnboardDeviceExInfo,       ///< 8 - Onboard Device Ex Information
  ESmbiosObjGroupAssociations,         ///< 9 - Group Associations
  ESmbiosObjBiosLanguageInfo,          ///< 10 - BIOS Language Information
  ESmbiosObjEnclosureInfo,             ///< 11 - Enclosure Information
  ESmbiosObjMemoryDeviceInfo,          ///< 12 - Memory Device Information
  ESmbiosObjSystemBootInfo,            ///< 13 - System Boot Info
  ESmbiosObjPhysicalMemoryArray,       ///< 14 - Physical Memory Array
  ESmbiosObjMemoryArrayMappedAddress,  ///< 15 - Memory Mapped Address
  ESmbiosObjPowerSupplyInfo,           ///< 16 - Power Supply Info
  ESmbiosObjFirmwareInventoryInfo,     ///< 17 - Firmware Inventory Info
  ESmbiosObjMax
} ESMBIOS_OBJECT_ID;

#pragma pack()

#endif // SMBIOS_NAMESPACE_OBJECTS_H_
