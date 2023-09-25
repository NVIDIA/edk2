/** @file
  This file defines the Redfish protocol features Library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_PROTOCOL_FEATURES_LIB_H_
#define REDFISH_PROTOCOL_FEATURES_LIB_H_

//
// Redfish HTTP query parameter definition.
//
#define REDFISH_EXPAND_PARAMETER  L"?$expand=."

///
/// Definitions of REDFISH_DEEP_OPERATIONS
///
typedef struct {
  BOOLEAN    DeepPATCH;
  BOOLEAN    DeepPOST;
  UINTN      MaxLevels;
} REDFISH_DEEP_OPERATIONS;

///
/// Definitions of REDFISH_EXPAND
///
typedef struct {
  BOOLEAN    ExpandAll;
  BOOLEAN    Levels;
  BOOLEAN    Links;
  UINTN      MaxLevels;
  BOOLEAN    NoLinks;
} REDFISH_EXPAND;

///
/// Definitions of REDFISH_PROTOCOL_FEATURES_SUPPORT
///
typedef struct {
  REDFISH_DEEP_OPERATIONS    DeepOperations;
  BOOLEAN                    ExcerptQuery;
  REDFISH_EXPAND             ExpandQuery;
  BOOLEAN                    FilterQuery;
  BOOLEAN                    MultipleHTTPRequests;
  BOOLEAN                    OnlyMemberQuery;
  BOOLEAN                    SelectQuery;
} REDFISH_PROTOCOL_FEATURES_SUPPORT;

/**
  This function query service root at BMC to get ProtocolFeaturesSupported
  attribute and return the data in REDFISH_PROTOCOL_FEATURES_SUPPORT
  structure.

  @param[in]   Service                    Redfish service instance
  @param[out]  ProtocolFeaturesSupported  Protocol features supported data on return.

  @retval EFI_SUCCESS   Protocol feature data is returned successfully.
  @retval Others        Error occurs.

**/
EFI_STATUS
EFIAPI
RedfishGetProtocolFeatures (
  IN  REDFISH_SERVICE                    Service,
  OUT REDFISH_PROTOCOL_FEATURES_SUPPORT  *ProtocolFeaturesSupported
  );

/**
  This function query service root at BMC to get ProtocolFeaturesSupported
  attribute and returns "NoLinks" attribute value.
  If the use of expand parameter is disabled by PcdHttpExpandQueryDisabled,
  this function always return FALSE no matter Redfish service supports it or not.

  @param[in]   Service  Redfish service instance

  @retval TRUE    "NoLinks" is TRUE and PcdHttpExpandQueryDisabled is FALSE.
  @retval FALSe   "NoLinks" is FALSE or PcdHttpExpandQueryDisabled is TRUE.

**/
BOOLEAN
RedfishNoLinksSupported (
  IN  REDFISH_SERVICE  Service
  );

#endif
