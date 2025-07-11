## @file
#  Dsc file for Dynamic Tables Framework.
#
#  Copyright (c) 2019, Linaro Limited. All rights reserved.<BR>
#  Copyright (c) 2019 - 2022, Arm Limited. All rights reserved.<BR>
#  Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME                  = DynamicTables
  PLATFORM_GUID                  = f39096a0-7a0a-442a-9413-cf584ef80cbb
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x0001001a
  OUTPUT_DIRECTORY               = Build/DynamicTables
  SUPPORTED_ARCHITECTURES        = ARM|AARCH64|IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

!include DynamicTables.dsc.inc

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses]
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

[LibraryClasses.ARM, LibraryClasses.AARCH64]
  PL011UartLib|ArmPlatformPkg/Library/PL011UartLib/PL011UartLib.inf

[Components.common]
  DynamicTablesPkg/Library/Common/AcpiHelperLib/AcpiHelperLib.inf
  DynamicTablesPkg/Library/Common/AmlLib/AmlLib.inf
  DynamicTablesPkg/Library/Common/SsdtPcieSupportLib/SsdtPcieSupportLib.inf
  DynamicTablesPkg/Library/Common/SsdtSerialPortFixupLib/SsdtSerialPortFixupLib.inf
  DynamicTablesPkg/Library/Common/TableHelperLib/TableHelperLib.inf
  DynamicTablesPkg/Library/Common/DynamicPlatRepoLib/DynamicPlatRepoLib.inf
  DynamicTablesPkg/Library/Common/SmbiosStringTableLib/SmbiosStringTableLib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType0Lib/SmbiosType0Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType1Lib/SmbiosType1Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType2Lib/SmbiosType2Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType3Lib/SmbiosType3Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType8Lib/SmbiosType8Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType9Lib/SmbiosType9Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType11Lib/SmbiosType11Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType13Lib/SmbiosType13Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType14Lib/SmbiosType14Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType16Lib/SmbiosType16Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType17Lib/SmbiosType17Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType19Lib/SmbiosType19Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType38Lib/SmbiosType38Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType41Lib/SmbiosType41Lib.inf
  DynamicTablesPkg/Library/Smbios/SmbiosType43Lib/SmbiosType43Lib.inf

[Components.ARM, Components.AARCH64]
  DynamicTablesPkg/Library/FdtHwInfoParserLib/FdtHwInfoParserLib.inf

[Components.AARCH64]
  DynamicTablesPkg/Library/DynamicTablesScmiInfoLib/DynamicTablesScmiInfoLib.inf

[BuildOptions]
  *_*_*_CC_FLAGS = -D DISABLE_NEW_DEPRECATED_INTERFACES

!ifdef STATIC_ANALYSIS
  # Check all rules
  # Inhibit C6305: Potential mismatch between sizeof and countof quantities.
  *_VS2017_*_CC_FLAGS = /wd6305 /analyze
!endif
