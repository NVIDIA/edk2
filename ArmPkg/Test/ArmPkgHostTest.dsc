## @file
# ArmPkg DSC file used to build host-based unit tests.
#
# Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME           = ArmPkgHostTest
  PLATFORM_GUID           = e76b6095-4218-48e7-8d32-bdef9ac97e6d
  PLATFORM_VERSION        = 0.1
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/ArmPkg/HostTest
  SUPPORTED_ARCHITECTURES = IA32|X64
  BUILD_TARGETS           = NOOPT
  SKUID_IDENTIFIER        = DEFAULT

!include UnitTestFrameworkPkg/UnitTestFrameworkPkgHost.dsc.inc

[LibraryClasses]

[Components]
  ArmPkg/Library/ImageInfoLib/GoogleTest/ImageInfoLibGoogleTest.inf {
    <LibraryClasses>
      ImageInfoLib|ArmPkg/Library/ImageInfoLib/ImageInfoLib.inf
      PeCoffGetEntryPointLib|ArmPkg/Library/ImageInfoLib/GoogleTest/PeCoffGetEntryPointStubLib.inf
  }
