## @file
# Redfish DSC include file for [LibraryClasses*] section of all Architectures.
#
# This file can be included to the [LibraryClasses*] section(s) of a platform DSC file
# by using "!include RedfishPkg/RedfisLibs.dsc.inc" to specify the library instances
# of EDKII network library classes.
#
# (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
# Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
!if $(REDFISH_CLIENT) == TRUE
  !include RedfishClientPkg/RedfishJsonStructureLib.dsc.inc
!endif

  #
  # Below modules should be pulled in by build tool.
  #
  MemoryV1_7_1Lib|RedfishClientPkg/ConverterLib/edk2library/Memory/v1_7_1/Lib.inf
  MemoryCollectionLib|RedfishClientPkg/ConverterLib/edk2library/MemoryCollection/Lib.inf
  ComputerSystemV1_5_0Lib|RedfishClientPkg/ConverterLib/edk2library/ComputerSystem/v1_5_0/Lib.inf
  ComputerSystemCollectionLib|RedfishClientPkg/ConverterLib/edk2library/ComputerSystemCollection/Lib.inf
  BiosV1_0_9Lib|RedfishClientPkg/ConverterLib/edk2library/Bios/v1_0_9/Lib.inf
  #
  # Above modules should be pulled in by build tool.
  #

  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf
  HttpLib|NetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf
  RedfishLib|RedfishClientPkg/PrivateLibrary/RedfishLib/RedfishLib.inf
  RedfishFeatureUtilityLib|RedfishClientPkg/Library/RedfishFeatureUtilityLib/RedfishFeatureUtilityLib.inf
  RedfishPlatformConfigLib|RedfishPkg/Library/RedfishPlatformConfigLib/RedfishPlatformConfigLib.inf
  RedfishContentCodingLib|RedfishPkg/Library/RedfishContentCodingLibNull/RedfishContentCodingLibNull.inf
  ConverterCommonLib|RedfishClientPkg/ConverterLib/edk2library/ConverterCommonLib/ConverterCommonLib.inf
  RedfishResourceIdentifyLib|RedfishClientPkg/Library/RedfishResourceIdentifyLibNull/RedfishResourceIdentifyLibNull.inf
  EdkIIRedfishResourceConfigLib|RedfishClientPkg/Library/EdkIIRedfishResourceConfigLib/EdkIIRedfishResourceConfigLib.inf
  RedfishEventLib|RedfishClientPkg/Library/RedfishEventLib/RedfishEventLib.inf
  RedfishVersionLib|RedfishClientPkg/Library/RedfishVersionLib/RedfishVersionLib.inf
  RedfishAddendumLib|RedfishClientPkg/Library/RedfishAddendumLib/RedfishAddendumLib.inf
