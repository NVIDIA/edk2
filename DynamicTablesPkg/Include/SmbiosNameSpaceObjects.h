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

#include <IndustryStandard/SmBios.h>

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
  ESmbiosObjIpmiDeviceInfo,            ///< 18 - IPMI Device Information
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
  UINT64                                     Size;

  /** Device Set */
  UINT8                                      DeviceSet;

  /** Speed of the device
    Speed of the device in MegaTransfers/second.
  */
  UINT32                                     Speed;

  /** Configured speed of the memory device
    Configured speed of the memory device in MegaTransfers/second.
  */
  UINT32                                     ConfiguredMemorySpeed;

  /** Serial Number of device  */
  CHAR8                                      *SerialNum;

  /** AssetTag identifying the device */
  CHAR8                                      *AssetTag;

  /** Device Locator String for the device.
   String that describes the slot or position of the device on the board.
   */
  CHAR8                                      *DeviceLocator;

  /** Bank locator string for the device.
   String that describes the bank where the device is located.
   */
  CHAR8                                      *BankLocator;

  /** Firmware version of the memory device */
  CHAR8                                      *FirmwareVersion;

  /** Part number of the memory device */
  CHAR8                                      *PartNum;

  /** Manufacturer Id.
   2 byte Manufacturer Id as per JEDEC Standard JEP106AV
  */
  UINT16                                     ModuleManufacturerId;

  /** Manufacturer Product Id
   2 byte Manufacturer Id as designated by Manufacturer.
  */
  UINT16                                     ModuleProductId;

  /** Minimum voltage
   Minimum operating voltage for this device, in millivolts.
  */
  UINT16                                     MinVolt;

  /** Maximum voltage
   Maximum operating voltage for this device, in millivolts.
  */
  UINT16                                     MaxVolt;

  /** Configured voltage
   Configured voltage for this device, in millivolts.
  */
  UINT16                                     ConfVolt;

  CM_OBJECT_TOKEN                            MemoryDeviceInfoToken;
  CM_OBJECT_TOKEN                            PhysicalArrayToken;
  UINT16                                     DataWidth;
  UINT16                                     TotalWidth;
  UINT8                                      Rank;

  MEMORY_DEVICE_TYPE                         DeviceType;
  MEMORY_DEVICE_TYPE_DETAIL                  TypeDetail;
  MEMORY_DEVICE_TECHNOLOGY                   DeviceTechnology;
  MEMORY_FORM_FACTOR                         FormFactor;
  MEMORY_DEVICE_OPERATING_MODE_CAPABILITY    MemoryOperatingModeCapability;
  UINT16                                     MemoryErrorInformationHandle;
} CM_SMBIOS_MEMORY_DEVICE_INFO;

typedef struct {
  CM_OBJECT_TOKEN              CmObjToken;
  SMBIOS_TABLE_GENERATOR_ID    GeneratorId;
} CONTAINED_CM_OBJECTS;

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

/** A structure that describes the Baseboard.

  The Baseboard information are described by this object.

  SMBIOS Specification v3.5.0 Type2

  ID: EStdObjBaseboardInfo,
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

#define MAX_SLOT_PEER_GROUP  0x05

/** A structure that describes the physical system slot.

   The physical system slot on the system are described by this object.

   SMBIOS Specification v3.5.0 Type9

   ID: EStdObjSystemSlotInfo,
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

/** A structure that describes the IPMI Device Information.
 *
 *  The IPMI device information on the system is described by this object.
 *
 *  SMBIOS Specification v3.5.0 Type38
 *  IPMI Specification v2.0 r1.1 SPMI Description Table
 *
 *  ID: EArmObjIpmiDeviceInfo
**/
typedef struct CmSmbiosIpmiDeviceInfo {
  /** IPMI Interface Type */
  UINT8              IpmiIntfType;

  /** IPMI Specification Revision */
  UINT8              IpmiSpecRevision;

  /** IPMI I2C Slave Address */
  UINT8              IpmiI2CSlaveAddress;

  /** IPMI NV Storage Device Address */
  UINT8              IpmiNVStorageDevAddress;

  /** IPMI Base Address */
  UINT64             IpmiBaseAddress;

  /** IPMI Base Address Modifier Interrupt Information */
  UINT8              IpmiBaseAddModIntInfo;

  /** IPMI Interrupt Number */
  UINT8              IpmiInterruptNum;

  /** IPMI Device's ACPI _UID */
  UINT32             IpmiUid;

  /** CM Object Token of Ipmi Device information */
  CM_OBJECT_TOKEN    IpmiDeviceInfoToken;
} CM_SMBIOS_IPMI_DEVICE_INFO;

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

#pragma pack()

#endif // SMBIOS_NAMESPACE_OBJECTS_H_
