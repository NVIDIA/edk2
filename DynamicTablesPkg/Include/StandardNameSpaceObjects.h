/** @file

  Copyright (c) 2017 - 2022, Arm Limited. All rights reserved.
  Copyright (c) 2022 - 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
    - Std or STD - Standard
**/

#ifndef STANDARD_NAMESPACE_OBJECTS_H_
#define STANDARD_NAMESPACE_OBJECTS_H_

#include <AcpiTableGenerator.h>
#include <SmbiosTableGenerator.h>

#pragma pack(1)

/** A macro defining a reserved zero/NULL token value that
    does not identify any object.
*/
#define CM_NULL_TOKEN  0

/** A reference token that the Configuration Manager can use
    to identify a Configuration Manager object.

  This can be used to differentiate between instances of
  objects of the same types. The identification scheme is
  implementation defined and is defined by the Configuration
  Manager.

  Typically the token is used to identify a specific instance
  from a set of objects in a call to the GetObject()/SetObject(),
  implemented by the Configuration Manager protocol.

  Note: The token value 0 is reserved for a NULL token and does
        not identify any object.
**/
typedef UINTN CM_OBJECT_TOKEN;

/** The ESTD_OBJECT_ID enum describes the Object IDs
    in the Standard Namespace.
*/
typedef enum StdObjectID {
  EStdObjCfgMgrInfo = 0x00000000, ///< 0  - Configuration Manager Info
  EStdObjAcpiTableList,           ///< 1  - ACPI table Info List
  EStdObjSmbiosTableList,         ///< 2  - SMBIOS table Info List
  EStdObjIpmiDeviceInfo,          ///< 3  - IPMI Device Information
  EStdObjBaseboardInfo,           ///< 4  - Baseboard Information
  EStdObjSystemSlotInfo,          ///< 5  - System Slot Information
  EStdObjSystemInfo,              ///< 6  - System Information
  EStdObjTpmDeviceInfo,           ///< 7  - TPM Device Info
  EStdObjOemStrings,              ///< 8  - OEM Strings
  EStdObjPortConnectorInfo,       ///< 9  - Port connector Information
  EStdObjBiosInfo,                ///< 10 - Bios Information
  EStdObjOnboardDeviceExInfo,     ///< 11 - Onboard Device Ex Information
  EStdObjGroupAssociations,       ///< 12 - Group Associations
  EStdObjMax
} ESTD_OBJECT_ID;

/** A structure that describes the Configuration Manager Information.
*/
typedef struct CmStdObjConfigurationManagerInfo {
  /// The Configuration Manager Revision.
  UINT32    Revision;

  /** The OEM ID. This information is used to
      populate the ACPI table header information.
  */
  UINT8     OemId[6];
} CM_STD_OBJ_CONFIGURATION_MANAGER_INFO;

/** A structure used to describe the ACPI table generators to be invoked.

  The AcpiTableData member of this structure may be used to directly provide
  the binary ACPI table data which is required by the following standard
  generators:
    - RAW
    - DSDT
    - SSDT

  Providing the ACPI table data is optional and depends on the generator
  that is being invoked. If unused, set AcpiTableData to NULL.
*/
typedef struct CmAStdObjAcpiTableInfo {
  /// The signature of the ACPI Table to be installed
  UINT32                         AcpiTableSignature;

  /// The ACPI table revision
  UINT8                          AcpiTableRevision;

  /// The ACPI Table Generator ID
  ACPI_TABLE_GENERATOR_ID        TableGeneratorId;

  /// Optional pointer to the ACPI table data
  EFI_ACPI_DESCRIPTION_HEADER    *AcpiTableData;

  /// An OEM-supplied string that the OEM uses to identify the particular
  /// data table. This field is particularly useful when defining a definition
  /// block to distinguish definition block functions. The OEM assigns each
  /// dissimilar table a new OEM Table ID.
  /// This field could be constructed using the SIGNATURE_64() macro.
  ///   e.g. SIGNATURE_64 ('A','R','M','H','G','T','D','T')
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information using part of the
  /// CM_STD_OBJ_CONFIGURATION_MANAGER_INFO.OemId field and the
  /// ACPI table signature.
  UINT64    OemTableId;

  /// An OEM-supplied revision number. Larger numbers are assumed to be
  /// newer revisions.
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information using the revision of the
  /// Configuration Manager (CM_STD_OBJ_CONFIGURATION_MANAGER_INFO.Revision).
  UINT32    OemRevision;

  /// The minor revision of an ACPI table if required by the table.
  /// Note: If this field is not populated (has value of Zero), then the
  /// Generators shall populate this information based on the latest minor
  /// revision of the table that is supported by the generator.
  /// e.g. This field can be used to specify the minor revision to be set
  /// for the FADT table.
  UINT8     MinorRevision;
} CM_STD_OBJ_ACPI_TABLE_INFO;

/** A structure used to describe the SMBIOS table generators to be invoked.

  The SmbiosTableData member of this structure is used to provide
  the SMBIOS table data which is required by the following standard
  generator(s):
    - RAW

  Providing the SMBIOS table data is optional and depends on the
  generator that is being invoked. If unused, set the SmbiosTableData
  to NULL.
*/
typedef struct CmStdObjSmbiosTableInfo {
  /// The SMBIOS Table Generator ID
  SMBIOS_TABLE_GENERATOR_ID    TableGeneratorId;

  /// Optional pointer to the SMBIOS table data
  SMBIOS_STRUCTURE             *SmbiosTableData;
} CM_STD_OBJ_SMBIOS_TABLE_INFO;

/** A structure that describes the IPMI Device Information.
 *
 *  The IPMI device information on the system is described by this object.
 *
 *  SMBIOS Specification v3.5.0 Type38
 *  IPMI Specification v2.0 r1.1 SPMI Description Table
 *
 *  ID: EArmObjIpmiDeviceInfo
**/
typedef struct CmStdIpmiDeviceInfo {
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
} CM_STD_IPMI_DEVICE_INFO;

typedef struct {
  CM_OBJECT_TOKEN              CmObjToken;
  SMBIOS_TABLE_GENERATOR_ID    GeneratorId;
} CONTAINED_CM_OBJECTS;

/** A structure that describes the Baseboard.

  The Baseboard information are described by this object.

  SMBIOS Specification v3.5.0 Type2

  ID: EStdObjBaseboardInfo,
*/
typedef struct CmStdBaseboardInfo {
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
  CONTAINED_CM_OBJECTS    ContainedCmObjects[1];
} CM_STD_BASEBOARD_INFO;

/** A structure that describes port connector.

  SMBIOS Specification v3.5.0 Type 8

  ID: EStdObjPortConnectorInfo,
*/
typedef struct CmStdPortConnectorInfo {
  CHAR8              *InternalReferenceDesignator;
  UINT8              InternalConnectorType;                 ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  CHAR8              *ExternalReferenceDesignator;
  UINT8              ExternalConnectorType;                 ///< The enumeration value from MISC_PORT_CONNECTOR_TYPE.
  UINT8              PortType;                              ///< The enumeration value from MISC_PORT_TYPE.
  CM_OBJECT_TOKEN    CmObjectToken;
} CM_STD_PORT_CONNECTOR_INFO;

#define MAX_SLOT_PEER_GROUP  0x05

/** A structure that describes the physical system slot.

  The physical system slot on the system are described by this object.

  SMBIOS Specification v3.5.0 Type9

  ID: EStdObjSystemSlotInfo,
*/
typedef struct CmStdSystemSlotInfo {
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
} CM_STD_SYSTEM_SLOTS_INFO;

/** A structure that describes the system.

  SMBIOS Specification v3.5.0 Type 1

  ID: EStdObjSystemInfo,
*/
typedef struct CmStdSystemInfo {
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
} CM_STD_SYSTEM_INFO;

/** A structure that describes the TPM device.

  SMBIOS Specification v3.5.0 Type 43

  ID: EStdObjTpmDeviceInfo,
*/
typedef struct CmStdTpmDeviceInfo {
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
} CM_STD_TPM_DEVICE_INFO;

/** A structure that describes the OEM Strings
 *
 *  The OEM strings information is described by this object.
 *
 *  ID: EStdObjOemStrings
**/
typedef struct CmStdOemStrings {
  /** Number of strings */
  UINT8              StringCount;

  /** String Table */
  CHAR8              **StringTable;

  /** CM Object Token of OEM Strings */
  CM_OBJECT_TOKEN    OemStringsToken;
} CM_STD_OEM_STRINGS;

/** A structure that describes the Bios Vendor Information.

  The Bios information on the system is described by this object.

  SMBIOS Specification v3.5.0 Type00

  ID: EStdObjBiosInfo
**/
typedef struct CmStdBiosInfo {
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
} CM_STD_BIOS_INFO;

/** A structure that describes port connector.

  SMBIOS Specification v3.5.0 Type 41

  ID: EStdObjPortConnectorInfo,
*/
typedef struct CmStdOnboardDeviceExtendedInfo {
  CHAR8              *ReferenceDesignation;
  UINT8              DeviceType;                            ///< The enumeration value from ONBOARD_DEVICE_EXTENDED_INFO_TYPE
  UINT8              DeviceTypeInstance;
  UINT16             SegmentGroupNum;
  UINT8              BusNum;
  UINT8              DevFuncNum;
  CM_OBJECT_TOKEN    CmObjectToken;
} CM_STD_ONBOARD_DEVICE_EXTENDED_INFO;

/** A structure that describes the Group Associations.

  The Group Associations information are described by this object.

  ID: CmStdObjGroupAssociations,
*/
typedef struct CmStdObjGroupAssociations {
  /** CM Object Token of Group Associations */
  CM_OBJECT_TOKEN         GroupAssociationsToken;

  /** Group Name  */
  CHAR8                   *GroupName;

  /** Number Of Items  */
  UINT8                   NumberOfItems;

  /** Contained Object Handles */
  CONTAINED_CM_OBJECTS    *ContainedCmObjects;
} CM_STD_GROUP_ASSOCIATIONS;

#pragma pack()

#endif // STANDARD_NAMESPACE_OBJECTS_H_
