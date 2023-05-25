/** @file
  This file defines the Redfish HTTP cache library internal headers.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_HTTP_CACHE_INTERNAL_LIB_H_
#define REDFISH_HTTP_CACHE_INTERNAL_LIB_H_

#include <Uefi.h>
#include <RedfishBase.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/RedfishLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishHttpCacheLib.h>
#include <Library/RedfishDebugLib.h>

#define REDFISH_HTTP_CACHE_LIST_SIZE   0x0F
#define REDFISH_HTTP_GET_RETRY_MAX     0x0F
#define REDFISH_HTTP_RETRY_WAIT        (2 * 1000000U)  ///< 1 second
#define REDFISH_HTTP_CACHE_DEBUG       DEBUG_VERBOSE
#define REDFISH_HTTP_CACHE_DEBUG_DUMP  DEBUG_VERBOSE

///
/// Definition of REDFISH_HTTP_CACHE_DATA
///
typedef struct {
  LIST_ENTRY          List;
  EFI_STRING          Uri;
  UINTN               HitCount;
  REDFISH_RESPONSE    *Response;
} REDFISH_HTTP_CACHE_DATA;

#define REDFISH_HTTP_CACHE_FROM_LIST(a)  BASE_CR (a, REDFISH_HTTP_CACHE_DATA, List)

///
/// Definition of REDFISH_HTTP_CACHE_LIST
///
typedef struct {
  LIST_ENTRY    Head;
  UINTN         Count;
  UINTN         Capacity;
} REDFISH_HTTP_CACHE_LIST;

///
/// Definition of REDFISH_HTTP_CACHE_PRIVATE
///
typedef struct {
  REDFISH_HTTP_CACHE_LIST    CacheList;
} REDFISH_HTTP_CACHE_PRIVATE;

#endif
