/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BIOS_COMMON_H_
#define EFI_REDFISH_BIOS_COMMON_H_

#include <RedfishJsonStructure/Bios/v1_0_9/EfiBiosV1_0_9.h>
#include <RedfishResourceCommon.h>

//
// Schema information.
//
#define REDFISH_MANAGED_URI      L"Systems/{}/Bios"
#define MAX_URI_LENGTH          256
#define RESOURCE_SCHEMA         "Bios"
#define RESOURCE_SCHEMA_MAJOR   "1"
#define RESOURCE_SCHEMA_MINOR   "0"
#define RESOURCE_SCHEMA_ERRATA  "9"
#define RESOURCE_SCHEMA_VERSION "v1_0_9"
#define REDPATH_ARRAY_PATTERN   L"/Bios/.*"
#define REDPATH_ARRAY_PREFIX    L"/Bios/"
#define RESOURCE_SCHEMA_FULL    "x-uefi-redfish-Bios.v1_0_9"
#define REDFISH_SCHEMA_NAME             "ComputerSystem"

#endif
