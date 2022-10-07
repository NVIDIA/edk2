/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2021 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_MEMORY_COMMON_H_
#define EFI_REDFISH_MEMORY_COMMON_H_

#include <RedfishJsonStructure/Memory/v1_7_1/EfiMemoryV1_7_1.h>
#include <RedfishResourceCommon.h>

//
// Schema information.
//
#define RESOURCE_SCHEMA         "Memory"
#define RESOURCE_SCHEMA_MAJOR   "1"
#define RESOURCE_SCHEMA_MINOR   "7"
#define RESOURCE_SCHEMA_ERRATA  "1"
#define RESOURCE_SCHEMA_VERSION "v1_7_1"
#define REDPATH_ARRAY_PATTERN   L"/Memory/\\{.*\\}/"
#define REDPATH_ARRAY_PREFIX    L"/Memory/"
#define RESOURCE_SCHEMA_FULL    "x-uefi-redfish-Memory.v1_7_1"

#endif