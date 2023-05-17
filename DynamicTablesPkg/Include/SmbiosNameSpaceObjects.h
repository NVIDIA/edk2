/** @file

  Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef SMBIOS_NAMESPACE_OBJECTS_H_
#define SMBIOS_NAMESPACE_OBJECTS_H_

#include <IndustryStandard/SmBios.h>

#pragma pack(1)

typedef enum SmbiosObjectID {
  ESmbiosObjReserved,
  ESmbiosObjBaseboardInfo,
  ESmbiosObjSystemSlotInfo,
  ESmbiosObjSystemInfo,
  ESmbiosObjTpmDeviceInfo,
  ESmbiosObjOemStrings,
  ESmbiosObjPortConnectorInfo,
  ESmbiosObjBiosInfo,
  ESmbiosObjOnboardDeviceExInfo,
  ESmbiosObjGroupAssociations,
  ESmbiosObjBiosLanguageInfo,
  ESmbiosObjEnclosureInfo,
  ESmbiosObjMemoryDeviceInfo,
  ESmbiosObjSystemBootInfo,
  ESmbiosObjPhysicalMemoryArray,
  ESmbiosObjMemoryArrayMappedAddress,
  ESmbiosObjPowerSupplyInfo,
  ESmbiosObjFirmwareInventoryInfo,
  ESmbiosObjProcessorInfo,
  ESmbiosObjCacheInfo,
  ESmbiosObjMax
} ESMBIOS_OBJECT_ID;

/** A structure that describes the physical memory device.

  The physical memory devices on the system are described by this object.

  SMBIOS Specification v3.5.0 Type17

  ID: ESmbiosObjMemoryDeviceInfo,
*/
typedef struct CmSmbiosMemoryDeviceInfo {
  /** Size of the device.
    Size of the device in bytes.
  */
  UINT64                       Size;

  /** Device Set */
  UINT8                        DeviceSet;

  /** Speed of the device
    Speed of the device in MegaTransfers/second.
  */
  UINT32                       Speed;

  /** Serial Number of device  */
  CHAR8                        *SerialNum;

  /** AssetTag identifying the device */
  CHAR8                        *AssetTag;

  /** Device Locator String for the device.
   String that describes the slot or position of the device on the board.
   */
  CHAR8                        *DeviceLocator;

  /** Bank locator string for the device.
   String that describes the bank where the device is located.
   */
  CHAR8                        *BankLocator;

  /** Firmware version of the memory device */
  CHAR8                        *FirmwareVersion;

  /** Manufacturer Id.
   2 byte Manufacturer Id as per JEDEC Standard JEP106AV
  */
  UINT16                       ModuleManufacturerId;

  /** Manufacturer Product Id
   2 byte Manufacturer Id as designated by Manufacturer.
  */
  UINT16                       ModuleProductId;

  CM_OBJECT_TOKEN              MemoryDeviceInfoToken;
  CM_OBJECT_TOKEN              PhysicalArrayToken;
  UINT16                       DataWidth;
  UINT16                       TotalWidth;
  UINT8                        Rank;

  MEMORY_DEVICE_TYPE           DeviceType;
  MEMORY_DEVICE_TYPE_DETAIL    TypeDetail;
  MEMORY_DEVICE_TECHNOLOGY     DeviceTechnology;
  MEMORY_FORM_FACTOR           FormFactor;
} CM_SMBIOS_MEMORY_DEVICE_INFO;

typedef struct {
  CM_OBJECT_TOKEN              CmObjToken;
  SMBIOS_TABLE_GENERATOR_ID    GeneratorId;
} CONTAINED_CM_OBJECTS;

/** A structure that describes the Baseboard.

  The Baseboard information are described by this object.

  SMBIOS Specification v3.5.0 Type2

  ID: ESmbiosObjBaseboardInfo,
*/
typedef struct CmSmbiosBaseboardInfo {
  /** CM Object Token of baseboard */
  CM_OBJECT_TOKEN         BaseboardInfoToken;

  /** CM Object Token of Chassis */
  CM_OBJECT_TOKEN         ChassisToken;

  /** Manufacturer of baseboard  */
  CHAR8                   *Manufacturer;

  /** Product Name */
  CHAR8                   *ProductName;

  /** Version of device baseboard */
  CHAR8                   *Version;

  /** Serial Number of baseboard */
  CHAR8                   *SerialNumber;

  /** AssetTag of baseboard  */
  CHAR8                   *AssetTag;

  /** Feature Flag of baseboard  */
  UINT8                   FeatureFlag;

  /** Location in Chassis */
  CHAR8                   *LocationInChassis;

  /** Board Type  */
  UINT8                   BoardType;

  /** Number Of Contained Object Handles  */
  UINT8                   NumberOfContainedObjectHandles;

  /** Contained Object Handles */
  CONTAINED_CM_OBJECTS    *ContainedCmObjects;
} CM_SMBIOS_BASEBOARD_INFO;

/** A structure that describes the enclosure.

  SMBIOS Specification v3.5.0 Type 3

  ID: ESmbiosObjEnclosureInfo,
*/
typedef struct CmSmbiosEnclosureInfo {
  /** CM Object Token of Enclosure  */
  CM_OBJECT_TOKEN      EnclosureInfoToken;
  /** Enclosure Manufacturer Name */
  CHAR8                *Manufacturer;
  /** Enclosure type */
  UINT8                Type;
  /** Enclosure version */
  CHAR8                *Version;
  /** Enclosure Serial Number */
  CHAR8                *SerialNum;
  /** Enclosure Asset Tag */
  CHAR8                *AssetTag;
  /** Enclosure Bootup State */
  UINT8                BootupState;
  /** Enclosure Power Supply State */
  UINT8                PowerSupplyState;
  /** Enclosure Thermal State */
  UINT8                ThermalState;
  /** Enclosure Security Status */
  UINT8                SecurityStatus;
  /** Reserved  for Oem defined */
  UINT8                OemDefined[4];
  /** Enclosure Height */
  UINT8                Height;
  /** Enclosure Number of Power Cords */
  UINT8                NumberofPowerCords;
  /** Enclosure Contained Element Count */
  UINT8                ContainedElementCount;

  /** Enclosure Contained Element Record Length
    If no Contained Elements are included, this
    field is set to 0. For version 2.3.2 and later of
    this specification, this field is set to at least 03h
    when Contained Elements are specified.
  */
  UINT8                ContainedElementRecordLength;

  /** Enclosure contained elements
    Can have 0 to (ContainedElementCount *
    ContainedElementRecordLength)
  */
  CONTAINED_ELEMENT    *ContainedElements;
  /** Enclosure SKU number */
  CHAR8                *SkuNum;
} CM_SMBIOS_ENCLOSURE_INFO;

/** A structure that describes port connector.

  SMBIOS Specification v3.5.0 Type 8

  ID: ESmbiosObjPortConnectorInfo,
*/
typedef struct CmSmbiosPortConnectorInfo {
  CHAR8              *InternalReferenceDesignator;
  UINT8              InternalConnectorType;                 ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  CHAR8              *ExternalReferenceDesignator;
  UINT8              ExternalConnectorType;                 ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  UINT8              PortType;                              ///< The enumeration value from MISC_PORT_TYPE.
  CM_OBJECT_TOKEN    CmObjectToken;
} CM_SMBIOS_PORT_CONNECTOR_INFO;

#define MAX_SLOT_PEER_GROUP  0x05

/** A structure that describes the physical system slot.

  The physical system slot on the system are described by this object.

  SMBIOS Specification v3.5.0 Type9

  ID: ESmbiosObjSystemSlotInfo,
*/
typedef struct CmSmbiosSystemSlotInfo {
  /** CM Object Token of SystemSlot  */
  CM_OBJECT_TOKEN         SystemSlotInfoToken;

  /** Slot Designation */
  CHAR8                   *SlotDesignation;

  /** Slot Type */
  UINT8                   SlotType;

  /** Slot Data Bus Width */
  UINT8                   SlotDataBusWidth;

  /** Current Usage */
  UINT8                   CurrentUsage;

  /** Slot Length */
  UINT8                   SlotLength;

  /** Slot ID */
  UINT16                  SlotID;

  /** Slot Characteristics 1 */
  UINT8                   SlotCharacteristics1;

  /** Slot Characteristics 2 */
  UINT8                   SlotCharacteristics2;

  /** Segment Group Number (Base) */
  UINT16                  SegmentGroupNum;

  /** Bus Number (Base) */
  UINT8                   BusNum;

  /** Device/Function Number (Base) */
  UINT8                   DevFuncNum;

  /** Data Bus Width (Base) */
  UINT8                   DataBusWidth;

  /** Slot Information */
  UINT8                   SlotInformation;

  /** Slot Physical Width */
  UINT8                   SlotPhysicalWidth;

  /** Slot Pitch */
  UINT16                  SlotPitch;

  /** Slot Height */
  UINT8                   SlotHeight;

  /** Peer (S/B/D/F/Width) grouping count (n) */
  UINT8                   PeerGroupingCount;

  /** Peer (S/B/D/F/Width) groups */
  MISC_SLOT_PEER_GROUP    PeerGroups[MAX_SLOT_PEER_GROUP];
} CM_SMBIOS_SYSTEM_SLOTS_INFO;

/** A structure that describes the system.

  SMBIOS Specification v3.5.0 Type 1

  ID: ESmbiosObjSystemInfo,
*/
typedef struct CmSmbiosSystemInfo {
  /** Manufacturer of the system */
  CHAR8              *Manufacturer;

  /** Product name of the system */
  CHAR8              *ProductName;

  /** Version of the system */
  CHAR8              *Version;

  /** Serial number of the system */
  CHAR8              *SerialNum;

  /** Universal unique ID of the system */
  GUID               Uuid;

  /** Identifies the event that caused the system to power up. */
  UINT8              WakeUpType;

  /** SKU number of the system */
  CHAR8              *SkuNum;

  /** Family that the system belongs */
  CHAR8              *Family;

  /** Token of this System information CM Object */
  CM_OBJECT_TOKEN    SystemInfoToken;
} CM_SMBIOS_SYSTEM_INFO;

/** A structure that describes the TPM device.

  SMBIOS Specification v3.5.0 Type 43

  ID: ESmbiosObjTpmDeviceInfo,
*/
typedef struct CmSmbiosTpmDeviceInfo {
  /** TPM Vendor ID */
  UINT8              VendorID[4];

  /** TPM Major Spec Version */
  UINT8              MajorSpecVersion;

  /** TPM Minor Spec Version */
  UINT8              MinorSpecVersion;

  /** TPM Firmware Version 1 */
  UINT32             FirmwareVersion1;

  /** TPM Firmware Version 2 */
  UINT32             FirmwareVersion2;

  /** TPM Description */
  CHAR8              *Description;

  /** TPM Characteristics */
  UINT64             Characteristics;

  /** TPM OEM defined */
  UINT32             OemDefined;

  /** Token of this TPM device information CM Object */
  CM_OBJECT_TOKEN    TpmDeviceInfoToken;
} CM_SMBIOS_TPM_DEVICE_INFO;

/** A structure that describes the OEM Strings
 *
 *  The OEM strings information is described by this object.
 *
 *  ID: ESmbiosObjOemStrings
**/
typedef struct CmSmbiosOemStrings {
  /** Number of strings */
  UINT8              StringCount;

  /** String Table */
  CHAR8              **StringTable;

  /** CM Object Token of OEM Strings */
  CM_OBJECT_TOKEN    OemStringsToken;
} CM_SMBIOS_OEM_STRINGS;

/** A structure that describes the Bios Vendor Information.

  The Bios information on the system is described by this object.

  SMBIOS Specification v3.5.0 Type00

  ID: ESmbiosObjBiosInfo
**/
typedef struct CmSmbiosBiosInfo {
  /** System BIOS Vendor string */
  CHAR8                        *BiosVendor;

  /** BIOS Uefi version */
  CHAR8                        *BiosVersion;

  /** BIOS starting address segment */
  UINT16                       BiosSegment;

  /** BIOS release date string */
  CHAR8                        *BiosReleaseDate;

  /** BIOS ROM size */
  UINT8                        BiosSize;

  /** Defines the functions BIOS supports */
  MISC_BIOS_CHARACTERISTICS    BiosCharacteristics;

  /** Optional set of functions that BIOS supports */
  UINT8                        BIOSCharacteristicsExtensionBytes[2];

  /** System BIOS firmware major version */
  UINT8                        SystemBiosMajorRelease;

  /** System BIOS firmware minor version */
  UINT8                        SystemBiosMinorRelease;

  /** Identifies the Embedded Controller/BMC firmware major release */
  UINT8                        ECFirmwareMajorRelease;

  /** Identifies the Embedded Controller/BMC firmware minor release */
  UINT8                        ECFirmwareMinorRelease;

  /** Extended BIOS ROM size, rounded up if needed */
  EXTENDED_BIOS_ROM_SIZE       ExtendedBiosSize;

  /** CM Object Token of Bios information */
  CM_OBJECT_TOKEN              BiosInfoToken;
} CM_SMBIOS_BIOS_INFO;

/** A structure that describes port connector.

  SMBIOS Specification v3.5.0 Type 41

  ID: ESmbiosObjPortConnectorInfo,
*/
typedef struct CmSmbiosOnboardDeviceExtendedInfo {
  CHAR8              *ReferenceDesignation;
  UINT8              DeviceType;                            ///< The enumeration value from ONBOARD_DEVICE_EXTENDED_INFO_TYPE
  UINT8              DeviceTypeInstance;
  UINT16             SegmentGroupNum;
  UINT8              BusNum;
  UINT8              DevFuncNum;
  CM_OBJECT_TOKEN    CmObjectToken;
} CM_SMBIOS_ONBOARD_DEVICE_EXTENDED_INFO;

/** A structure that describes the Group Associations.

  The Group Associations information are described by this object.

  ID: CmSmbiosObjGroupAssociations,
*/
typedef struct CmSmbiosObjGroupAssociations {
  /** CM Object Token of Group Associations */
  CM_OBJECT_TOKEN         GroupAssociationsToken;

  /** Group Name  */
  CHAR8                   *GroupName;

  /** Number Of Items  */
  UINT8                   NumberOfItems;

  /** Contained Object Handles */
  CONTAINED_CM_OBJECTS    *ContainedCmObjects;
} CM_SMBIOS_GROUP_ASSOCIATIONS;

/** A structure that describes the BIOS language information.

  SMBIOS Specification v3.5.0 Type 13

  ID: ESmbiosObjBiosLanguageInfo
**/
typedef struct CmSmbiosBiosLanguageInfo {
  /** Number of Installable Languages */
  UINT8              InstallableLanguages;

  /** Flags to indicate if abbreviated or long formatted Language string is provided */
  UINT8              Flags;

  /** Current Bios Language index(1 based) in the list of installable languages */
  UINT8              CurrentLanguage;

  /** Installable Languages supported by the platform */
  CHAR8              **SupportedLanguages;

  /** Token of this Bios language information CM Object */
  CM_OBJECT_TOKEN    BiosLanguageInfoToken;
} CM_SMBIOS_BIOS_LANGUAGE_INFO;

/** A structure that describes the System Boot Information.

  SMBIOS Specification v3.5.0 Type 32

  ID: ESmbiosObjSystemBootInfo
**/
typedef struct CmSmbiosSystemBootInfo {
  /** System boot status */
  UINT8              BootStatus;

  /** Token of this Bios language information CM Object */
  CM_OBJECT_TOKEN    SystemBootInfoToken;
} CM_SMBIOS_SYSTEM_BOOT_INFO;

/** A structure that describes the Physical Memory Array.

  SMBIOS Specification v3.6.0 Type 16

  ID: ESmbiosObjPhysicalMemoryArray
**/
typedef struct CmSmbiosPhysicalMemoryArray {
  UINT8              Use;
  UINT8              Location;
  UINT16             MemoryErrorCorrection;
  UINT16             MemoryErrorInformationHandle;
  UINT16             NumberOfMemoryDevices;
  UINT8              MemoryErrorCorrectionType;
  UINT64             Size;
  UINT8              NumMemDevices;
  CM_OBJECT_TOKEN    MemoryErrInfoToken;
  CM_OBJECT_TOKEN    PhysMemArrayToken;
} CM_SMBIOS_PHYSICAL_MEMORY_ARRAY;

/** A structure that describes the Memory Array Mapped Address.

  SMBIOS Specification v3.6.0 Type 19

  ID: ESmbiosObjPhysicalMemoryArray
**/
typedef struct CmSmbiosMemArrayMappedAddress {
  EFI_PHYSICAL_ADDRESS    StartingAddress;
  EFI_PHYSICAL_ADDRESS    EndingAddress;
  CM_OBJECT_TOKEN         MemoryArrayMappedAddressToken;
  UINT8                   NumMemDevices;
  CM_OBJECT_TOKEN         PhysMemArrayToken;
} CM_SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS;

/** A structure that describes the system power supply.

  SMBIOS Specification v3.5.0 Type 39

  ID: ESmbiosObjPowerSupplyInfo,
*/
typedef struct CmSmbiosPowerSupplyInfo {
  /** Power Unit Group */
  UINT8              PowerUnitGroup;

  /** Location */
  CHAR8              *Location;

  /** Device Name */
  CHAR8              *DeviceName;

  /** Manufacturer */
  CHAR8              *Manufacturer;

  /** Serial Number */
  CHAR8              *SerialNumber;

  /** Asset Tag Number */
  CHAR8              *AssetTagNumber;

  /** Model Part Number */
  CHAR8              *ModelPartNumber;

  /** Revision Level */
  CHAR8              *RevisionLevel;

  /** Max Power Capacity */
  UINT16             MaxPowerCapacity;

  /** Power Supply Characteristics */
  UINT16             PowerSupplyCharacteristics;

  /** CM Object Token to locate Input Voltage Probe Handle */
  CM_OBJECT_TOKEN    InputVoltageProbeCmToken;

  /** CM Object Token to locate Cooling Device Handle */
  CM_OBJECT_TOKEN    CoolingDeviceCmToken;

  /** CM Object Token to locate Input Current Probe Handle */
  CM_OBJECT_TOKEN    InputCurrentProbeCmToken;

  /** Token of this Power Supply Info  CM Object */
  CM_OBJECT_TOKEN    PowerSupplyInfoToken;
} CM_SMBIOS_POWER_SUPPLY_INFO;

/** A structure that describes the Firmware Inventory Information.

  SMBIOS Specification v3.5.0 Type 45

  ID: ESmbiosObjFirmwareInventoryInfo,
*/
typedef struct CmStdFirmwareInventoryInfo {
  /** CM Object Token of Enclosure  */
  CM_OBJECT_TOKEN              FirmwareInventoryInfoToken;
  CHAR8                        *FirmwareComponentName;
  CHAR8                        *FirmwareVersion;
  UINT8                        FirmwareVersionFormat;   ///< The enumeration value from FIRMWARE_INVENTORY_VERSION_FORMAT_TYPE
  CHAR8                        *FirmwareId;
  UINT8                        FirmwareIdFormat;        ///< The enumeration value from FIRMWARE_INVENTORY_FIRMWARE_ID_FORMAT_TYPE.
  CHAR8                        *ReleaseDate;
  CHAR8                        *Manufacturer;
  CHAR8                        *LowestSupportedVersion;
  UINT64                       ImageSize;
  FIRMWARE_CHARACTERISTICS     Characteristics;
  UINT8                        State;                   ///< The enumeration value from FIRMWARE_INVENTORY_STATE.
  UINT8                        AssociatedComponentCount;
  CM_OBJECT_TOKEN              *AssociatedComponentHandles;
  SMBIOS_TABLE_GENERATOR_ID    GeneratorId;
} CM_SMBIOS_FIRMWARE_INVENTORY_INFO;

/** A structure that describes the Processor Information.

  The Processor Information on the system is described by this object.

  SMBIOS Specification v3.6.0 Type4

  ID: EStdObjProcessorInfo
**/
typedef struct CmSmbiosProcessorInfo {
  /** Socket Designation string */
  CHAR8                *SocketDesignation;

  /** Processor Type */
  UINT8                ProcessorType;

  /** Processor Family */
  UINT8                ProcessorFamily;

  /** Processor Manufacturer string */
  CHAR8                *ProcessorManufacturer;

  /** Processor ID */
  PROCESSOR_ID_DATA    ProcessorId;

  /** Processor Version string */
  CHAR8                *ProcessorVersion;

  /** Voltage */
  PROCESSOR_VOLTAGE    Voltage;

  /** External Clock */
  UINT16               ExternalClock;

  /** Max Speed*/
  UINT16               MaxSpeed;

  /** Current Speed */
  UINT16               CurrentSpeed;

  /** Status */
  UINT8                Status;

  /** Processor Upgrade */
  UINT8                ProcessorUpgrade;

  /** L1 Cache Handle */
  UINT16               L1CacheHandle;

  /** L2 Cache Handle */
  UINT16               L2CacheHandle;

  /** L3 Cache Handle */
  UINT16               L3CacheHandle;

  /** Serial Number string */
  CHAR8                *SerialNumber;

  /** Asset Tag string */
  CHAR8                *AssetTag;

  /** Part Number string */
  CHAR8                *PartNumber;

  /** Core Count */
  UINT8                CoreCount;

  /** Enabled Core Count */
  UINT8                EnabledCoreCount;

  /** Thread Count */
  UINT8                ThreadCount;

  /** Processor Characteristics */
  UINT16               ProcessorCharacteristics;

  /** Processor Family 2 */
  UINT16               ProcessorFamily2;

  /** Core Count 2 */
  UINT16               CoreCount2;

  /** Enabled Core Count 2 */
  UINT16               EnabledCoreCount2;

  /** Thread Count 2 */
  UINT16               ThreadCount2;

  /** Thread Enabled */
  UINT16               ThreadEnabled;

  /** CM Object Token of Processor information */
  CM_OBJECT_TOKEN      ProcessorInfoToken;

  /** CM Object Token of L1 Cache information */
  CM_OBJECT_TOKEN      CacheInfoTokenL1;

  /** CM Object Token of L2 Cache information */
  CM_OBJECT_TOKEN      CacheInfoTokenL2;

  /** CM Object Token of L3 Cache information */
  CM_OBJECT_TOKEN      CacheInfoTokenL3;
} CM_SMBIOS_PROCESSOR_INFO;

/** A structure that describes the Cache Information.

  The Cache Information on the system is described by this object.

  SMBIOS Specification v3.6.0 Type7

  ID: EStdObjCacheInfo
**/
typedef struct CmSmbiosCacheInfo {
  /** Socket Designation string */
  CHAR8                   *SocketDesignation;

  /** Cache Configuration */
  UINT16                  CacheConfiguration;

  /** Maximum Cache Size */
  UINT16                  MaximumCacheSize;

  /** Installed Size */
  UINT16                  InstalledSize;

  /** Supported SRAM Type */
  CACHE_SRAM_TYPE_DATA    SupportedSRAMType;

  /** Current SRAM Type */
  CACHE_SRAM_TYPE_DATA    CurrentSRAMType;

  /** Cache Speed */
  UINT8                   CacheSpeed;

  /** Error Correction Type */
  UINT8                   ErrorCorrectionType;

  /** System Cache Type*/
  UINT8                   SystemCacheType;

  /** Associativity */
  UINT8                   Associativity;

  /** Maximum Cache Size 2*/
  UINT32                  MaximumCacheSize2;

  /** Installed Size 2*/
  UINT32                  InstalledSize2;

  /** CM Object Token of Cache information */
  CM_OBJECT_TOKEN         CacheInfoToken;
} CM_SMBIOS_CACHE_INFO;

#pragma pack()

#endif // SMBIOS_NAMESPACE_OBJECTS_H_
