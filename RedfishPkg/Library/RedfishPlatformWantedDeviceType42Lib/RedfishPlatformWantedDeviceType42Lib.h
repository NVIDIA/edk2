/** @file
  Header file for RedfishPlatformWantedDeviceLibType42.
  This library assumes that the SMBIOS type 42h record is available before
  ConnectAll() or redfish drivers are attmped to be connected.

  Copyright (c) 2025, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_PLATFORM_WANTED_DEVICE_TYPE42_LIB_H_
#define REDFISH_PLATFORM_WANTED_DEVICE_TYPE42_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/NetLib.h>
#include <Library/RedfishPlatformWantedDeviceLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/Smbios.h>

#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/RedfishHostInterface.h>

#define MAC_ADDRESS_SIZE  6
#define STRUCT_MEMBER_SIZEOF(s, m)  (sizeof(((s*)0)->m))

#define DESCRIPTOR_DEVICE_TYPE_SIZE \
          STRUCT_MEMBER_SIZEOF(REDFISH_INTERFACE_DATA, DeviceType)

#define RECORD_TYPE_DATA_SIZE \
          STRUCT_MEMBER_SIZEOF(MC_HOST_INTERFACE_PROTOCOL_RECORD, ProtocolTypeData)

#define TYPE_SPECIFIC_DATA_SIZE \
          STRUCT_MEMBER_SIZEOF(SMBIOS_TABLE_TYPE42, InterfaceTypeSpecificData)

#endif /* REDFISH_PLATFORM_WANTED_DEVICE_TYPE42_LIB_H_ */
