/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BIOS_COMMON_H_
#define EFI_REDFISH_BIOS_COMMON_H_

#include <RedfishJsonStructure/Bios/v1_2_0/EfiBiosV1_2_0.h>
#include <RedfishResourceCommon.h>

//
// Schema information.
//
#define REDFISH_MANAGED_URI      L"Systems/{}/Bios"
#define MAX_URI_LENGTH           256
#define RESOURCE_SCHEMA          "Bios"
#define RESOURCE_SCHEMA_MAJOR    "1"
#define RESOURCE_SCHEMA_MINOR    "2"
#define RESOURCE_SCHEMA_ERRATA   "0"
#define RESOURCE_SCHEMA_VERSION  "v1_2_0"
#define REDPATH_ARRAY_PATTERN    L"/Bios/.*"
#define REDPATH_ARRAY_PREFIX     L"/Bios/"
#define RESOURCE_SCHEMA_FULL     "x-uefi-redfish-Bios.v1_2_0"

#endif
