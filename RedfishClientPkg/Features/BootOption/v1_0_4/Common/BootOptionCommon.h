/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BOOT_OPTION_COMMON_H_
#define EFI_REDFISH_BOOT_OPTION_COMMON_H_

#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseLib.h>
#include <Library/RedfishHttpCacheLib.h>
#include <RedfishJsonStructure/BootOption/v1_0_4/EfiBootOptionV1_0_4.h>
#include <RedfishResourceCommon.h>

//
// Schema information.
//
#define RESOURCE_SCHEMA                  "BootOption"
#define RESOURCE_SCHEMA_MAJOR            "1"
#define RESOURCE_SCHEMA_MINOR            "0"
#define RESOURCE_SCHEMA_ERRATA           "4"
#define RESOURCE_SCHEMA_VERSION          "v1_0_4"
#define REDPATH_ARRAY_PATTERN            L"/BootOptions/\\{.*\\}/"
#define REDPATH_ARRAY_PREFIX             L"/BootOptions/"
#define RESOURCE_SCHEMA_FULL             "x-uefi-redfish-BootOption.v1_0_4"
#define REDFISH_BOOT_OPTION_PARAMETER    L"?name="
#define REDFISH_BOOT_OPTION_DEBUG_TRACE  DEBUG_INFO
#endif
