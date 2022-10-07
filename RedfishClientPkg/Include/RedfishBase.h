/** @file
  Redfish base header file.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_BASE_H_
#define EFI_REDFISH_BASE_H_

#define IS_EMPTY_STRING(a)    ((a) == NULL || (a)[0] == '\0')
#define REDFISH_DEBUG_TRACE   DEBUG_INFO

///
/// This GUID is used for an EFI Variable that stores the Redfish data.
///
EFI_GUID mRedfishVariableGuid = {0x91c46a3d, 0xed1a, 0x477b, {0xa5, 0x33, 0x87, 0x2d, 0xcd, 0xb0, 0xfc, 0xc1}};

#endif
