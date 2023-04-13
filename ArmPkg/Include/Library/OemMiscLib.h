/** @file
*
*  Copyright (c) 2022, Ampere Computing LLC. All rights reserved.
*  Copyright (c) 2021, NUVIA Inc. All rights reserved.
*  Copyright (c) 2015, Hisilicon Limited. All rights reserved.
*  Copyright (c) 2015, Linaro Limited. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef OEM_MISC_LIB_H_
#define OEM_MISC_LIB_H_

#include <Uefi.h>
#include <IndustryStandard/SmBios.h>

typedef enum {
  CpuCacheL1 = 1,
  CpuCacheL2,
  CpuCacheL3,
  CpuCacheL4,
  CpuCacheL5,
  CpuCacheL6,
  CpuCacheL7,
  CpuCacheLevelMax
} OEM_MISC_CPU_CACHE_LEVEL;

typedef struct {
  UINT8     Voltage;       ///< Processor voltage
  UINT16    CurrentSpeed;  ///< Current clock speed in MHz
  UINT16    MaxSpeed;      ///< Maximum clock speed in MHz
  UINT16    ExternalClock; ///< External clock speed in MHz
  UINT16    CoreCount;     ///< Number of cores available
  UINT16    CoresEnabled;  ///< Number of cores enabled
  UINT16    ThreadCount;   ///< Number of threads per processor
} OEM_MISC_PROCESSOR_DATA;

typedef enum {
  BiosVersionType00,
  ProductNameType01,
  SerialNumType01,
  UuidType01,
  SystemManufacturerType01,
  VersionType01,
  SkuNumberType01,
  FamilyType01,
  AssetTagType02,
  SerialNumberType02,
  BoardManufacturerType02,
  ProductNameType02,
  VersionType02,
  SkuNumberType02,
  ChassisLocationType02,
  AssetTagType03,
  SerialNumberType03,
  VersionType03,
  ChassisTypeType03,
  ManufacturerType03,
  SkuNumberType03,
  ProcessorPartNumType04,
  ProcessorSerialNumType04,
  ProcessorSerialNumType04_0 = ProcessorSerialNumType04,
  ProcessorSerialNumType04_1,
  ProcessorSerialNumType04_2,
  ProcessorSerialNumType04_3,
  ProcessorSerialNumType04_4,
  ProcessorSerialNumType04_5,
  ProcessorSerialNumType04_6,
  ProcessorSerialNumType04_7,
  ProcessorSerialNumType04_8,
  ProcessorSerialNumType04_9,
  ProcessorSerialNumType04_10,
  ProcessorSerialNumType04_11,
  ProcessorSerialNumType04_12,
  ProcessorSerialNumType04_13,
  ProcessorSerialNumType04_14,
  ProcessorSerialNumType04_15,
  ProcessorVersionType04,
  ProcessorSocketDesType04_0,
  ProcessorSocketDesType04_1,
  ProcessorSocketDesType04_2,
  ProcessorSocketDesType04_3,
  ProcessorSocketDesType04_4,
  ProcessorSocketDesType04_5,
  ProcessorSocketDesType04_6,
  ProcessorSocketDesType04_7,
  ProcessorSocketDesType04_8,
  ProcessorSocketDesType04_9,
  ProcessorSocketDesType04_10,
  ProcessorSocketDesType04_11,
  ProcessorSocketDesType04_12,
  ProcessorSocketDesType04_13,
  ProcessorSocketDesType04_14,
  ProcessorSocketDesType04_15,
  SmbiosHiiStringFieldMax
} OEM_MISC_SMBIOS_HII_STRING_FIELD;

#define INDEX_TO_FIELD(n, v)  ((n) + (v))
#define FIELD_TO_INDEX(n, v)  ((n) - (v))

STATIC_ASSERT ((ProcessorSerialNumType04_1 - ProcessorSerialNumType04_0) == 1, "Incorrect order for ProcessorSerialNumType04_1");
STATIC_ASSERT ((ProcessorSerialNumType04_2 - ProcessorSerialNumType04_0) == 2, "Incorrect order for ProcessorSerialNumType04_2");
STATIC_ASSERT ((ProcessorSerialNumType04_3 - ProcessorSerialNumType04_0) == 3, "Incorrect order for ProcessorSerialNumType04_3");
STATIC_ASSERT ((ProcessorSerialNumType04_4 - ProcessorSerialNumType04_0) == 4, "Incorrect order for ProcessorSerialNumType04_4");
STATIC_ASSERT ((ProcessorSerialNumType04_5 - ProcessorSerialNumType04_0) == 5, "Incorrect order for ProcessorSerialNumType04_5");
STATIC_ASSERT ((ProcessorSerialNumType04_6 - ProcessorSerialNumType04_0) == 6, "Incorrect order for ProcessorSerialNumType04_6");
STATIC_ASSERT ((ProcessorSerialNumType04_7 - ProcessorSerialNumType04_0) == 7, "Incorrect order for ProcessorSerialNumType04_7");
STATIC_ASSERT ((ProcessorSerialNumType04_8 - ProcessorSerialNumType04_0) == 8, "Incorrect order for ProcessorSerialNumType04_8");
STATIC_ASSERT ((ProcessorSerialNumType04_9 - ProcessorSerialNumType04_0) == 9, "Incorrect order for ProcessorSerialNumType04_9");
STATIC_ASSERT ((ProcessorSerialNumType04_10 - ProcessorSerialNumType04_0) == 10, "Incorrect order for ProcessorSerialNumType04_10");
STATIC_ASSERT ((ProcessorSerialNumType04_11 - ProcessorSerialNumType04_0) == 11, "Incorrect order for ProcessorSerialNumType04_11");
STATIC_ASSERT ((ProcessorSerialNumType04_12 - ProcessorSerialNumType04_0) == 12, "Incorrect order for ProcessorSerialNumType04_12");
STATIC_ASSERT ((ProcessorSerialNumType04_13 - ProcessorSerialNumType04_0) == 13, "Incorrect order for ProcessorSerialNumType04_13");
STATIC_ASSERT ((ProcessorSerialNumType04_14 - ProcessorSerialNumType04_0) == 14, "Incorrect order for ProcessorSerialNumType04_14");
STATIC_ASSERT ((ProcessorSerialNumType04_15 - ProcessorSerialNumType04_0) == 15, "Incorrect order for ProcessorSerialNumType04_15");

STATIC_ASSERT ((ProcessorSocketDesType04_1 - ProcessorSocketDesType04_0) == 1, "Incorrect order for ProcessorSocketDesType04_1");
STATIC_ASSERT ((ProcessorSocketDesType04_2 - ProcessorSocketDesType04_0) == 2, "Incorrect order for ProcessorSocketDesType04_2");
STATIC_ASSERT ((ProcessorSocketDesType04_3 - ProcessorSocketDesType04_0) == 3, "Incorrect order for ProcessorSocketDesType04_3");
STATIC_ASSERT ((ProcessorSocketDesType04_4 - ProcessorSocketDesType04_0) == 4, "Incorrect order for ProcessorSocketDesType04_4");
STATIC_ASSERT ((ProcessorSocketDesType04_5 - ProcessorSocketDesType04_0) == 5, "Incorrect order for ProcessorSocketDesType04_5");
STATIC_ASSERT ((ProcessorSocketDesType04_6 - ProcessorSocketDesType04_0) == 6, "Incorrect order for ProcessorSocketDesType04_6");
STATIC_ASSERT ((ProcessorSocketDesType04_7 - ProcessorSocketDesType04_0) == 7, "Incorrect order for ProcessorSocketDesType04_7");
STATIC_ASSERT ((ProcessorSocketDesType04_8 - ProcessorSocketDesType04_0) == 8, "Incorrect order for ProcessorSocketDesType04_8");
STATIC_ASSERT ((ProcessorSocketDesType04_9 - ProcessorSocketDesType04_0) == 9, "Incorrect order for ProcessorSocketDesType04_9");
STATIC_ASSERT ((ProcessorSocketDesType04_10 - ProcessorSocketDesType04_0) == 10, "Incorrect order for ProcessorSocketDesType04_10");
STATIC_ASSERT ((ProcessorSocketDesType04_11 - ProcessorSocketDesType04_0) == 11, "Incorrect order for ProcessorSocketDesType04_11");
STATIC_ASSERT ((ProcessorSocketDesType04_12 - ProcessorSocketDesType04_0) == 12, "Incorrect order for ProcessorSocketDesType04_12");
STATIC_ASSERT ((ProcessorSocketDesType04_13 - ProcessorSocketDesType04_0) == 13, "Incorrect order for ProcessorSocketDesType04_13");
STATIC_ASSERT ((ProcessorSocketDesType04_14 - ProcessorSocketDesType04_0) == 14, "Incorrect order for ProcessorSocketDesType04_14");
STATIC_ASSERT ((ProcessorSocketDesType04_15 - ProcessorSocketDesType04_0) == 15, "Incorrect order for ProcessorSocketDesType04_15");

/*
 * The following are functions that the each platform needs to
 * implement in its OemMiscLib library.
 */

/** Gets the CPU frequency of the specified processor.

  @param ProcessorIndex Index of the processor to get the frequency for.

  @return               CPU frequency in Hz
**/
UINTN
EFIAPI
OemGetCpuFreq (
  IN UINT8  ProcessorIndex
  );

/** Gets information about the specified processor and stores it in
    the structures provided.

  @param ProcessorIndex  Index of the processor to get the information for.
  @param ProcessorStatus Processor status.
  @param ProcessorCharacteristics Processor characteritics.
  @param MiscProcessorData        Miscellaneous processor information.

  @return  TRUE on success, FALSE on failure.
**/
BOOLEAN
EFIAPI
OemGetProcessorInformation (
  IN UINTN                               ProcessorIndex,
  IN OUT PROCESSOR_STATUS_DATA           *ProcessorStatus,
  IN OUT PROCESSOR_CHARACTERISTIC_FLAGS  *ProcessorCharacteristics,
  IN OUT OEM_MISC_PROCESSOR_DATA         *MiscProcessorData
  );

/** Gets information about the cache at the specified cache level.

  @param ProcessorIndex The processor to get information for.
  @param CacheLevel     The cache level to get information for.
  @param DataCache  Whether the cache is a data cache.
  @param UnifiedCache Whether the cache is a unified cache.
  @param SmbiosCacheTable The SMBIOS Type7 cache information structure.

  @return TRUE on success, FALSE on failure.
**/
BOOLEAN
EFIAPI
OemGetCacheInformation (
  IN UINT8                   ProcessorIndex,
  IN UINT8                   CacheLevel,
  IN BOOLEAN                 DataCache,
  IN BOOLEAN                 UnifiedCache,
  IN OUT SMBIOS_TABLE_TYPE7  *SmbiosCacheTable
  );

/** Gets the maximum number of processors supported by the platform.

  @return The maximum number of processors.
**/
UINT8
EFIAPI
OemGetMaxProcessors (
  VOID
  );

/** Gets the type of chassis for the system.

  @retval The type of the chassis.
**/
MISC_CHASSIS_TYPE
EFIAPI
OemGetChassisType (
  VOID
  );

/** Returns whether the specified processor is present or not.

  @param ProcessIndex The processor index to check.

  @return TRUE is the processor is present, FALSE otherwise.
**/
BOOLEAN
EFIAPI
OemIsProcessorPresent (
  IN UINTN  ProcessorIndex
  );

/** Updates the HII string for the specified field.

  @param HiiHandle     The HII handle.
  @param TokenToUpdate The string to update.
  @param Field         The field to get information about.
**/
VOID
EFIAPI
OemUpdateSmbiosInfo (
  IN EFI_HII_HANDLE                    HiiHandle,
  IN EFI_STRING_ID                     TokenToUpdate,
  IN OEM_MISC_SMBIOS_HII_STRING_FIELD  Field
  );

/** Fetches the Type 32 boot information status.

  @return Boot status.
**/
MISC_BOOT_INFORMATION_STATUS_DATA_TYPE
EFIAPI
OemGetBootStatus (
  VOID
  );

/** Fetches the chassis status when it was last booted.

 @return Chassis status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisBootupState (
  VOID
  );

/** Fetches the chassis power supply/supplies status when last booted.

 @return Chassis power supply/supplies status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisPowerSupplyState (
  VOID
  );

/** Fetches the chassis thermal status when last booted.

 @return Chassis thermal status.
**/
MISC_CHASSIS_STATE
EFIAPI
OemGetChassisThermalState (
  VOID
  );

/** Fetches the chassis security status when last booted.

 @return Chassis security status.
**/
MISC_CHASSIS_SECURITY_STATE
EFIAPI
OemGetChassisSecurityStatus (
  VOID
  );

/** Fetches the chassis height in RMUs (Rack Mount Units).

  @return The height of the chassis.
**/
UINT8
EFIAPI
OemGetChassisHeight (
  VOID
  );

/** Fetches the number of power cords.

  @return The number of power cords.
**/
UINT8
EFIAPI
OemGetChassisNumPowerCords (
  VOID
  );

/**
  Fetches the system UUID.

  @param[out] SystemUuid     The pointer to the buffer to store the System UUID.

**/
VOID
EFIAPI
OemGetSystemUuid (
  OUT GUID  *SystemUuid
  );

/** Fetches the BIOS release.

  @return The BIOS release.
**/
UINT16
EFIAPI
OemGetBiosRelease (
  VOID
  );

/** Fetches the embedded controller firmware release.

  @return The embedded controller firmware release.
**/
UINT16
EFIAPI
OemGetEmbeddedControllerFirmwareRelease (
  VOID
  );

#endif // OEM_MISC_LIB_H_
