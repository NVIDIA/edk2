/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_COMPUTERSYSTEM_COMMON_H_
#define EFI_REDFISH_COMPUTERSYSTEM_COMMON_H_

#include <RedfishJsonStructure/ComputerSystem/v1_5_0/EfiComputerSystemV1_5_0.h>
#include <RedfishResourceCommon.h>

//
// Schema information.
//
#define RESOURCE_SCHEMA         "ComputerSystem"
#define RESOURCE_SCHEMA_MAJOR   "1"
#define RESOURCE_SCHEMA_MINOR   "5"
#define RESOURCE_SCHEMA_ERRATA  "0"
#define RESOURCE_SCHEMA_VERSION "v1_5_0"
#define REDPATH_ARRAY_PATTERN   L"/Systems/\\{.*\\}/"
#define REDPATH_ARRAY_PREFIX    L"/Systems/"
#define RESOURCE_SCHEMA_FULL    "x-uefi-redfish-ComputerSystem.v1_5_0"

#endif
