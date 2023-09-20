/** @file

  Redfish feature driver implementation - internal header file

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BOOT_OPTION_COLLECTION_H_
#define EFI_REDFISH_BOOT_OPTION_COLLECTION_H_

#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/RedfishHttpLib.h>
#include <RedfishJsonStructure/BootOptionCollection/EfiBootOptionCollection.h>
#include <RedfishCollectionCommon.h>

#define REDFISH_SCHEMA_NAME                         "BootOption"
#define REDFISH_SCHEMA_MAJOR                        "1"
#define REDFISH_SCHEMA_MINOR                        "0"
#define REDFISH_SCHEMA_ERRATA                       "4"
#define REDFISH_MANAGED_URI                         L"Systems/{}/BootOptions"
#define MAX_URI_LENGTH                              256
#define REDFISH_BOOT_OPTION_REFERENCE_NAME          "BootOptionReference"
#define REDFISH_BOOT_OPTION_PREFIX                  "Boot"
#define REDFISH_BOOT_OPTION_PREFIX_LEN              0x04
#define REDFISH_BOOT_OPTION_NUM_LEN                 0x04
#define REDFISH_BOOT_OPTION_PARAMETER               "?name="
#define REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE  DEBUG_INFO

#endif
