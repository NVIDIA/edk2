/** @file
  Redfish version library implementation

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <RedfishBase.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/RedfishLib.h>
#include <Library/JsonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishVersionLib.h>
#include <Library/RedfishHttpLib.h>

#define REDFISH_VERSION_DEFAULT_STRING  L"v1"
#define REDFISH_ROOT_URI                L"/redfish"

REDFISH_SERVICE  *mCacheService;
EFI_STRING       mVersionCache;
UINTN            mVersionStringSize;

/**
  Cache the redfish service version for later use so we don't have to query
  HTTP request everytime.

  @param[in]   Service  Redfish service instance
  @param[in]   Version  Version string to cache

  @retval EFI_SUCCESS   Version is saved in cache successfully.
  @retval Others

**/
EFI_STATUS
CacheVersion (
  IN REDFISH_SERVICE  *Service,
  IN EFI_STRING       Version
  )
{
  if ((Service == NULL) || IS_EMPTY_STRING (Version)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mCacheService == Service) {
    return EFI_ALREADY_STARTED;
  }

  mCacheService = Service;
  if (mVersionCache != NULL) {
    FreePool (mVersionCache);
  }

  mVersionStringSize = StrSize (Version);
  mVersionCache      = AllocateCopyPool (mVersionStringSize, Version);
  if (mVersionCache == NULL) {
    mCacheService = NULL;
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Query HTTP request to BMC with given redfish service and return redfish
  version information. If there is troulbe to get Redfish version on BMC,
  The value of PcdDefaultRedfishVersion is returned.

  It's call responsibility to release returned buffer.

  @param[in]   Service  Redfish service instance

  @retval EFI_STRING  Redfish version string. NULL while error occurs.

**/
EFI_STRING
RedfishGetVersion (
  IN REDFISH_SERVICE  *Service
  )
{
  EFI_STATUS        Status;
  EFI_STRING        VersionString;
  REDFISH_RESPONSE  Response;
  EDKII_JSON_VALUE  JsonValue;
  EDKII_JSON_VALUE  N;
  CHAR8             *Key;
  EDKII_JSON_VALUE  Value;

  VersionString = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));

  if (Service == NULL) {
    goto ON_ERROR;
  }

  //
  // Use cache to prevent HTTP connection.
  //
  if (Service == mCacheService) {
    return AllocateCopyPool (mVersionStringSize, mVersionCache);
  }

  //
  // Get resource from redfish service.
  //
  Status = RedfishHttpGetResource (
             Service,
             REDFISH_ROOT_URI,
             &Response,
             TRUE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, RedfishHttpGetResource to %s failed: %r\n", __FUNCTION__, REDFISH_ROOT_URI, Status));
    if (Response.Payload != NULL) {
      RedfishDumpPayload (Response.Payload);
    }

    goto ON_ERROR;
  }

  JsonValue = RedfishJsonInPayload (Response.Payload);
  if ((JsonValue == NULL) || !JsonValueIsObject (JsonValue)) {
    goto ON_ERROR;
  }

  EDKII_JSON_OBJECT_FOREACH_SAFE (JsonValue, N, Key, Value) {
    if ((Key[0] == 'v') && JsonValueIsString (Value)) {
      VersionString = JsonValueGetUnicodeString (Value);
      break;
    }
  }

  if (VersionString != NULL) {
    CacheVersion (Service, VersionString);
    return VersionString;
  }

ON_ERROR:

  VersionString = (CHAR16 *)PcdGetPtr (PcdDefaultRedfishVersion);
  if (VersionString == NULL) {
    VersionString = REDFISH_VERSION_DEFAULT_STRING;
  }

  if (Response.Payload != NULL) {
    RedfishHttpFreeResource (&Response);
  }

  return AllocateCopyPool (StrSize (VersionString), VersionString);
}

/**

  Initial redfish version library instace.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCEESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishVersionLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  mCacheService      = NULL;
  mVersionCache      = NULL;
  mVersionStringSize = 0;

  return EFI_SUCCESS;
}

/**
  Release allocated resource.

  @param[in] ImageHandle       Handle that identifies the image to be unloaded.
  @param[in] SystemTable      The system table.

  @retval EFI_SUCCESS      The image has been unloaded.

**/
EFI_STATUS
EFIAPI
RedfishVersionLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mVersionCache != NULL) {
    FreePool (mVersionCache);
  }

  return EFI_SUCCESS;
}
