/** @file
  Redfish base header file.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BASE_H_
#define EFI_REDFISH_BASE_H_

#define IS_EMPTY_STRING(a)  ((a) == NULL || (a)[0] == '\0')

//
// Debug trace definitions
//
#define REDFISH_DEBUG_TRACE    DEBUG_INFO
#define CONFIG_LANG_MAP_TRACE  DEBUG_INFO
#define ETAG_DEBUG_TRACE       DEBUG_INFO

///
/// This GUID is used for an EFI Variable that stores the Redfish data.
///
extern EFI_GUID  gEfiRedfishClientVariableGuid;

#endif
