/** @file
  Redfish message library internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_MESSAGE_INTERNAL_H_
#define REDFISH_MESSAGE_INTERNAL_H_

#include <Uefi.h>
#include <RedfishBase.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishMessageLib.h>

#define REDFISH_MESSAGE_DEBUG  DEBUG_ERROR
#define REDFISH_MESSAGE_MAX    0xFF

///
/// Definition of REDFISH_MESSAGE_RECORD
///
typedef struct {
  LIST_ENTRY                  List;
  CHAR8                       *Message;
  CHAR8                       *Resolution;
  REDFISH_MESSAGE_SEVERITY    MessageSeverity;
} REDFISH_MESSAGE_RECORD;

#define REDFISH_MESSAGE_RECORD_FROM_LIST(a)  BASE_CR (a, REDFISH_MESSAGE_RECORD, List)

///
/// Definition of REDFISH_MESSAGE_ENTRY
///
typedef struct {
  LIST_ENTRY    List;
  EFI_STRING    Uri;
  LIST_ENTRY    MessageHead;
  UINTN         Count;
} REDFISH_MESSAGE_ENTRY;

#define REDFISH_MESSAGE_ENTRY_FROM_LIST(a)  BASE_CR (a, REDFISH_MESSAGE_ENTRY, List)

///
/// Definition of REDFISH_MESSAGE_INFO
///
typedef struct {
  LIST_ENTRY    ListHead;
  UINTN         Count;
} REDFISH_MESSAGE_PRIVATE;

#endif
