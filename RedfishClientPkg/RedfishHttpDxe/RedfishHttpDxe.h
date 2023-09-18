/** @file
  Definitions of RedfishHttpDxe

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_HTTP_DXE_H_
#define EDKII_REDFISH_HTTP_DXE_H_

#include <Uefi.h>
#include <IndustryStandard/Http11.h>
#include <RedfishBase.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/RedfishLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishDebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>

#include <Protocol/Http.h>
#include <Protocol/EdkIIRedfishHttpProtocol.h>

#define REDFISH_HTTP_CACHE_LIST_SIZE      0x80
#define REDFISH_ERROR_MSG_MAX             128
#define REDFISH_HTTP_ERROR_REPORT         "Redfish HTTP %a failure(0x%x): %a"
#define REDFISH_HTTP_CACHE_DEBUG          DEBUG_MANAGEABILITY
#define REDFISH_HTTP_CACHE_DEBUG_DUMP     DEBUG_MANAGEABILITY
#define REDFISH_HTTP_CACHE_DEBUG_REQUEST  DEBUG_MANAGEABILITY

#endif
