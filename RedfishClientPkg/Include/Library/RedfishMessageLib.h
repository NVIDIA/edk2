/** @file
  This file defines the Redfish message library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_MESSAGE_LIB_H_
#define REDFISH_MESSAGE_LIB_H_

#include <Uefi.h>

///
/// Definition of REDFISH_MESSAGE_SEVERITY
///
typedef enum {
  RedfishMessageSeverityOk = 0x0,
  RedfishMessageSeverityWarning,
  RedfishMessageSeverityCritical,
  RedfishMessageSeverityMax
} REDFISH_MESSAGE_SEVERITY;

typedef struct {
  CHAR8                       *Message;
  CHAR8                       *Resolution;
  REDFISH_MESSAGE_SEVERITY    MessageSeverity;
} REDFISH_MESSAGE_DATA;

/**
  Add Redfish message to given resource URI.

  @param[in]    ResourceUri       Report Redfish message to this resource URI.
  @param[in]    Message           String message.
  @param[in]    Resolution        Resolution message. This is optional.
  @param[in]    MessageSeverity   Message severity.

  @retval EFI_SUCCESS             Redfish message is reported.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageReport (
  IN EFI_STRING                ResourceUri,
  IN CHAR8                     *Message,
  IN CHAR8                     *Resolution OPTIONAL,
  IN REDFISH_MESSAGE_SEVERITY  MessageSeverity
  );

/**
  Get all messages reported to given URI. It's caller's responsibility to release
  MessageArray by calling RedfishMessageFree().

  @param[in]    TargetUri       Report Redfish message to this resource URI.
  @param[out]   MessageArray    The array of REDFISH_MESSAGE_DATA.
  @param[out]   MessageCount    The number of REDFISH_MESSAGE_DATA in MessageArray.

  @retval EFI_SUCCESS             Redfish message is reported.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageGet (
  IN  EFI_STRING            TargetUri,
  OUT REDFISH_MESSAGE_DATA  **MessageArray,
  OUT UINTN                 *MessageCount
  );

/**
  Release MessageArray.

  @param[in]   MessageArray    The array of REDFISH_MESSAGE_DATA.
  @param[in]   MessageCount    The number of REDFISH_MESSAGE_DATA in MessageArray.

  @retval EFI_SUCCESS             Redfish message is released.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageFree (
  IN REDFISH_MESSAGE_DATA  *MessageArray,
  IN UINTN                 MessageCount
  );

#endif
