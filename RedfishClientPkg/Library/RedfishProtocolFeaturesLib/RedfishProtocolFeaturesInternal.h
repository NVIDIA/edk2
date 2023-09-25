/** @file
  Redfish Protocol Features library internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_PROTOCOL_FEATURES_INTERNAL_H_
#define REDFISH_PROTOCOL_FEATURES_INTERNAL_H_

#include <Uefi.h>
#include <RedfishBase.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/JsonLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishHttpLib.h>
#include <Library/RedfishVersionLib.h>
#include <Library/RedfishDebugLib.h>
#include <Library/RedfishProtocolFeaturesLib.h>

#define REDFISH_PROTOCOL_FEATURES_DEBUG  DEBUG_MANAGEABILITY
//
// Attributes defined in #ServiceRoot.v1_16_0.ServiceRoot
//
#define REDFISH_ATTRIBUTE_PROTOCOL_FEATURES_SUPPORTED  "ProtocolFeaturesSupported"
#define REDFISH_ATTRIBUTE_DEEP_OPERATIONS              "DeepOperations"
#define REDFISH_ATTRIBUTE_EXCERPT_QUERY                "ExcerptQuery"
#define REDFISH_ATTRIBUTE_EXPAND_QUERY                 "ExpandQuery"
#define REDFISH_ATTRIBUTE_FILTER_QUERY                 "FilterQuery"
#define REDFISH_ATTRIBUTE_MULTIPLE_HTTP_REQUESTS       "MultipleHTTPRequests"
#define REDFISH_ATTRIBUTE_ONLY_MEMBER_QUERY            "OnlyMemberQuery"
#define REDFISH_ATTRIBUTE_SELECT_QUERY                 "SelectQuery"
#define REDFISH_ATTRIBUTE_DEEP_PATCH                   "DeepPATCH"
#define REDFISH_ATTRIBUTE_DEEP_POST                    "DeepPOST"
#define REDFISH_ATTRIBUTE_MAX_LEVELS                   "MaxLevels"
#define REDFISH_ATTRIBUTE_EXPAND_ALL                   "ExpandAll"
#define REDFISH_ATTRIBUTE_LEVELS                       "Levels"
#define REDFISH_ATTRIBUTE_LINKS                        "Links"
#define REDFISH_ATTRIBUTE_NO_LINKS                     "NoLinks"

#endif
