/** @file
  Common header file for RedfishFeatureUtilityLib driver.

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_FEATURE_INTERNAL_H_
#define REDFISH_FEATURE_INTERNAL_H_

#include <Uefi.h>
#include <RedfishBase.h>

#include <IndustryStandard/Http11.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/JsonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/RedfishPlatformConfigLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/HttpLib.h>
#include <Library/RedfishDebugLib.h>

#include <Guid/VariableFormat.h>

#include <Protocol/EdkIIRedfishETagProtocol.h>
#include <Protocol/EdkIIRedfishConfigLangMapProtocol.h>

#define INDEX_VARIABLE_SIZE        64
#define INDEX_STRING_SIZE          16
#define INDEX_STRING               L"{%d}"
#define SCHEMA_NAME_PREFIX         "x-uefi-redfish-"
#define SCHEMA_NAME_PREFIX_OFFSET  (AsciiStrLen (SCHEMA_NAME_PREFIX))
#define REDFISH_ROOT_PATH          "/v1"
#define REDFISH_ROOT_PATH_UNICODE  L"/v1"
#define MAX_CONF_LANG_LEN          128
#define MAX_REDFISH_URL_LEN        255
#define REGULAR_EXPRESSION_ARRAY   L"\\[.*\\]/.*"

#define BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_START_SIGNATURE  L"{"
#define BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_END_SIGNATURE    L"}"
#define BIOS_CONFIG_TO_REDFISH_REDPATH_POOL_SIZE              64

#endif
