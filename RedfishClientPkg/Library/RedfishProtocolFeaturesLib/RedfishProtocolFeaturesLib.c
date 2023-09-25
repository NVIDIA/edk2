/** @file
  Redfish protocol features library implementation

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishProtocolFeaturesInternal.h"

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
  )
{
  EFI_STATUS                         Status;
  REDFISH_PROTOCOL_FEATURES_SUPPORT  ProtocolFeaturesSupported;

  if (Service == NULL) {
    return FALSE;
  }

  if (PcdGetBool (PcdHttpExpandQueryDisabled)) {
    return FALSE;
  }

  Status = RedfishGetProtocolFeatures (Service, &ProtocolFeaturesSupported);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return ProtocolFeaturesSupported.ExpandQuery.NoLinks;
}

/**
  Debug print data in REDFISH_PROTOCOL_FEATURES_SUPPORT

  @param[in]  ErrorLevel                 DEBUG macro error level
  @param[in]  ProtocolFeaturesSupported  Protocol features supported data to print.

**/
VOID
RedfishDumpProtocolFeatures (
  IN UINTN                              ErrorLevel,
  IN REDFISH_PROTOCOL_FEATURES_SUPPORT  *ProtocolFeaturesSupported
  )
{
  if (ProtocolFeaturesSupported == NULL) {
    return;
  }

  DEBUG ((ErrorLevel, "DeepOperations\n"));
  DEBUG ((ErrorLevel, "  DeepPATCH: %a\n", ProtocolFeaturesSupported->DeepOperations.DeepPATCH ? "true" : "false"));
  DEBUG ((ErrorLevel, "  DeepPOST: %a\n", ProtocolFeaturesSupported->DeepOperations.DeepPOST ? "true" : "false"));
  DEBUG ((ErrorLevel, "  MaxLevels: %d\n", ProtocolFeaturesSupported->DeepOperations.MaxLevels));
  DEBUG ((ErrorLevel, "Expand\n"));
  DEBUG ((ErrorLevel, "  ExpandAll: %a\n", ProtocolFeaturesSupported->ExpandQuery.ExpandAll ? "true" : "false"));
  DEBUG ((ErrorLevel, "  Levels: %a\n", ProtocolFeaturesSupported->ExpandQuery.Levels ? "true" : "false"));
  DEBUG ((ErrorLevel, "  Links: %a\n", ProtocolFeaturesSupported->ExpandQuery.Links ? "true" : "false"));
  DEBUG ((ErrorLevel, "  NoLinks: %a\n", ProtocolFeaturesSupported->ExpandQuery.NoLinks ? "true" : "false"));
  DEBUG ((ErrorLevel, "  MaxLevels: %d\n", ProtocolFeaturesSupported->ExpandQuery.MaxLevels));
  DEBUG ((ErrorLevel, "ExcerptQuery: %a\n", ProtocolFeaturesSupported->ExcerptQuery ? "true" : "false"));
  DEBUG ((ErrorLevel, "FilterQuery: %a\n", ProtocolFeaturesSupported->FilterQuery ? "true" : "false"));
  DEBUG ((ErrorLevel, "MultipleHTTPRequests: %a\n", ProtocolFeaturesSupported->MultipleHTTPRequests ? "true" : "false"));
  DEBUG ((ErrorLevel, "OnlyMemberQuery: %a\n", ProtocolFeaturesSupported->OnlyMemberQuery ? "true" : "false"));
  DEBUG ((ErrorLevel, "SelectQuery: %a\n", ProtocolFeaturesSupported->SelectQuery ? "true" : "false"));
}

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
  )
{
  EFI_STATUS        Status;
  EFI_STRING        VersionString;
  REDFISH_RESPONSE  Response;
  EDKII_JSON_VALUE  JsonValue;
  EDKII_JSON_VALUE  ProtocolFeatures;
  EDKII_JSON_VALUE  DeepOperations;
  EDKII_JSON_VALUE  ExpandQuery;
  EDKII_JSON_VALUE  Value;

  if ((Service == NULL) || (ProtocolFeaturesSupported == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  VersionString = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));
  ZeroMem (ProtocolFeaturesSupported, sizeof (REDFISH_PROTOCOL_FEATURES_SUPPORT));

  //
  // Find Redfish service root
  //
  VersionString = RedfishGetVersion (Service);
  if (IS_EMPTY_STRING (VersionString)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Get resource from redfish service root.
  //
  Status = RedfishHttpGetResource (
             Service,
             VersionString,
             &Response,
             TRUE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, RedfishHttpGetResource to %s failed: %r\n", __func__, VersionString, Status));
    DumpRedfishResponse (NULL, DEBUG_ERROR, &Response);
    goto ON_RELEASE;
  }

  if (Response.Payload == NULL) {
    DEBUG ((DEBUG_ERROR, "%a, No response received\n", __func__));
    Status = EFI_PROTOCOL_ERROR;
    goto ON_RELEASE;
  }

  JsonValue = RedfishJsonInPayload (Response.Payload);
  if ((JsonValue == NULL) || !JsonValueIsObject (JsonValue)) {
    Status = EFI_PROTOCOL_ERROR;
    goto ON_RELEASE;
  }

  //
  // Get ProtocolFeaturesSupported attribute.
  //
  ProtocolFeatures = JsonObjectGetValue (JsonValueGetObject (JsonValue), REDFISH_ATTRIBUTE_PROTOCOL_FEATURES_SUPPORTED);
  if (!JsonValueIsObject (ProtocolFeatures)) {
    DEBUG ((DEBUG_ERROR, "%a, cannot find %a attribute\n", __func__, REDFISH_ATTRIBUTE_PROTOCOL_FEATURES_SUPPORTED));
    Status = EFI_UNSUPPORTED;
    goto ON_RELEASE;
  }

  //
  // ExcerptQuery
  //
  Value = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_EXCERPT_QUERY);
  if (JsonValueIsBoolean (Value)) {
    ProtocolFeaturesSupported->ExcerptQuery = JsonValueGetBoolean (Value);
  }

  //
  // FilterQuery
  //
  Value = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_FILTER_QUERY);
  if (JsonValueIsBoolean (Value)) {
    ProtocolFeaturesSupported->FilterQuery = JsonValueGetBoolean (Value);
  }

  //
  // MultipleHTTPRequests
  //
  Value = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_MULTIPLE_HTTP_REQUESTS);
  if (JsonValueIsBoolean (Value)) {
    ProtocolFeaturesSupported->MultipleHTTPRequests = JsonValueGetBoolean (Value);
  }

  //
  // OnlyMemberQuery
  //
  Value = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_ONLY_MEMBER_QUERY);
  if (JsonValueIsBoolean (Value)) {
    ProtocolFeaturesSupported->OnlyMemberQuery = JsonValueGetBoolean (Value);
  }

  //
  // SelectQuery
  //
  Value = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_SELECT_QUERY);
  if (JsonValueIsBoolean (Value)) {
    ProtocolFeaturesSupported->SelectQuery = JsonValueGetBoolean (Value);
  }

  //
  // DeepOperations
  //
  DeepOperations = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_DEEP_OPERATIONS);
  if (JsonValueIsObject (DeepOperations)) {
    //
    // DeepPATCH
    //
    Value = JsonObjectGetValue (JsonValueGetObject (DeepOperations), REDFISH_ATTRIBUTE_DEEP_PATCH);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->DeepOperations.DeepPATCH = JsonValueGetBoolean (Value);
    }

    //
    // DeepPOST
    //
    Value = JsonObjectGetValue (JsonValueGetObject (DeepOperations), REDFISH_ATTRIBUTE_DEEP_POST);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->DeepOperations.DeepPOST = JsonValueGetBoolean (Value);
    }

    //
    // MaxLevels
    //
    Value = JsonObjectGetValue (JsonValueGetObject (DeepOperations), REDFISH_ATTRIBUTE_MAX_LEVELS);
    if (JsonValueIsInteger (Value)) {
      ProtocolFeaturesSupported->DeepOperations.MaxLevels = (UINTN)JsonValueGetInteger (Value);
    }
  }

  //
  // ExpandQuery
  //
  ExpandQuery = JsonObjectGetValue (JsonValueGetObject (ProtocolFeatures), REDFISH_ATTRIBUTE_EXPAND_QUERY);
  if (JsonValueIsObject (ExpandQuery)) {
    //
    // ExpandAll
    //
    Value = JsonObjectGetValue (JsonValueGetObject (ExpandQuery), REDFISH_ATTRIBUTE_EXPAND_ALL);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->ExpandQuery.ExpandAll = JsonValueGetBoolean (Value);
    }

    //
    // Levels
    //
    Value = JsonObjectGetValue (JsonValueGetObject (ExpandQuery), REDFISH_ATTRIBUTE_LEVELS);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->ExpandQuery.Levels = JsonValueGetBoolean (Value);
    }

    //
    // Links
    //
    Value = JsonObjectGetValue (JsonValueGetObject (ExpandQuery), REDFISH_ATTRIBUTE_LINKS);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->ExpandQuery.Links = JsonValueGetBoolean (Value);
    }

    //
    // NoLinks
    //
    Value = JsonObjectGetValue (JsonValueGetObject (ExpandQuery), REDFISH_ATTRIBUTE_NO_LINKS);
    if (JsonValueIsBoolean (Value)) {
      ProtocolFeaturesSupported->ExpandQuery.NoLinks = JsonValueGetBoolean (Value);
    }

    //
    // MaxLevels
    //
    Value = JsonObjectGetValue (JsonValueGetObject (ExpandQuery), REDFISH_ATTRIBUTE_MAX_LEVELS);
    if (JsonValueIsInteger (Value)) {
      ProtocolFeaturesSupported->ExpandQuery.MaxLevels = (UINTN)JsonValueGetInteger (Value);
    }
  }

  DEBUG_CODE (
    RedfishDumpProtocolFeatures (REDFISH_PROTOCOL_FEATURES_DEBUG, ProtocolFeaturesSupported);
    );

ON_RELEASE:

  if (VersionString == NULL) {
    FreePool (VersionString);
  }

  if (Response.Payload != NULL) {
    RedfishFreeResponse (
      Response.StatusCode,
      Response.HeaderCount,
      Response.Headers,
      Response.Payload
      );
  }

  return Status;
}
