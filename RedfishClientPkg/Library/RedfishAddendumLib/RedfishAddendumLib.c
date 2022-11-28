/** @file
  Redfish addendum library helps Redfish application to get addendum data and OEM
  data from platform driver.

  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <RedfishBase.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishAddendumLib.h>

/**

  Convert Unicode string to ASCII string. It's call responsibility to release returned buffer.

  @param[in]  UnicodeStr      Unicode string to convert.

  @retval     CHAR8 *         ASCII string returned.
  @retval     NULL            Errors occur.

**/
CHAR8 *
UnicodeToAscii (
  IN EFI_STRING  UnicodeStr
  )
{
  CHAR8       *AsciiStr;
  UINTN       AsciiStrSize;
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (UnicodeStr)) {
    return NULL;
  }

  AsciiStrSize = StrLen (UnicodeStr) + 1;
  AsciiStr     = AllocatePool (AsciiStrSize);
  if (AsciiStr == NULL) {
    return NULL;
  }

  Status = UnicodeStrToAsciiStrS (UnicodeStr, AsciiStr, AsciiStrSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UnicodeStrToAsciiStrS failed: %r\n", Status));
    FreePool (AsciiStr);
    return NULL;
  }

  return AsciiStr;
}

/**
  Provide redfish resource with addendum data in given schema.

  @param[in]   Uri              Uri of input resource.
  @param[in]   Schema           Redfish schema string.
  @param[in]   Version          Schema version string.
  @param[in]   JsonText         Input resource in JSON format string.
  @param[out]  JsonWithAddendum The input resource with addendum value attached.

  @retval EFI_SUCCESS              Addendum data is attached.
  @retval EFI_NOT_FOUND            No addendum protocol is found in system.
  @retval EFI_UNSUPPORTED          No addendum data is required in given schema.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishGetAddendumData (
  IN     EFI_STRING  Uri,
  IN     CHAR8       *Schema,
  IN     CHAR8       *Version,
  IN     CHAR8       *JsonText,
  OUT    CHAR8       **JsonWithAddendum
  )
{
  REDFISH_RESOURCE_SCHEMA_INFO              SchemaInfo;
  EDKII_JSON_VALUE                          JsonValue;
  EFI_STATUS                                Status;
  EFI_HANDLE                                *HandleBuffer;
  UINTN                                     NumberOfHandles;
  UINTN                                     Index;
  EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL  *Protocol;

  if (IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (JsonText) || (JsonWithAddendum == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *JsonWithAddendum  = NULL;
  SchemaInfo.Uri     = UnicodeToAscii (Uri);
  SchemaInfo.Schema  = Schema;
  SchemaInfo.Version = Version;
  NumberOfHandles    = 0;
  HandleBuffer       = NULL;

  JsonValue = JsonLoadString (JsonText, 0, NULL);
  if ((JsonValue == NULL) || !JsonValueIsObject (JsonValue)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEdkIIRedfishResourceAddendumProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto ON_RELEASE;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEdkIIRedfishResourceAddendumProtocolGuid,
                    (VOID **)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->ProvisioningCallback (Protocol, &SchemaInfo, JsonValue);
    if (!EFI_ERROR (Status)) {
      *JsonWithAddendum = JsonDumpString (JsonValue, EDKII_JSON_COMPACT);
      break;
    }
  }

ON_RELEASE:

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (JsonValue != NULL) {
    JsonValueFree (JsonValue);
  }

  if (SchemaInfo.Uri != NULL) {
    FreePool (SchemaInfo.Uri);
  }

  return Status;
}

/**
  Provide redfish OEM resource with given schema information.

  @param[in]   Uri             Uri of input resource.
  @param[in]   Schema          Redfish schema string.
  @param[in]   Version         Schema version string.
  @param[in]   JsonText        Input resource in JSON format string.
  @param[out]  JsonWithOem     The input resource with OEM value attached.

  @retval EFI_SUCCESS              OEM data is attached.
  @retval EFI_NOT_FOUND            No addendum protocol is found in system.
  @retval EFI_UNSUPPORTED          No OEM data is required in given schema.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishGetOemData (
  IN  EFI_STRING  Uri,
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  CHAR8       *JsonText,
  OUT CHAR8       **JsonWithOem
  )
{
  REDFISH_RESOURCE_SCHEMA_INFO              SchemaInfo;
  EDKII_JSON_VALUE                          JsonValue;
  EDKII_JSON_VALUE                          JsonValueWithOem;
  EFI_STATUS                                Status;
  EFI_HANDLE                                *HandleBuffer;
  UINTN                                     NumberOfHandles;
  UINTN                                     Index;
  EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL  *Protocol;

  if (IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (JsonText) || (JsonWithOem == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *JsonWithOem       = NULL;
  SchemaInfo.Uri     = UnicodeToAscii (Uri);
  SchemaInfo.Schema  = Schema;
  SchemaInfo.Version = Version;
  JsonValue          = NULL;
  JsonValueWithOem   = NULL;
  NumberOfHandles    = 0;
  HandleBuffer       = NULL;

  JsonValue = JsonLoadString (JsonText, 0, NULL);
  if ((JsonValue == NULL) || !JsonValueIsObject (JsonValue)) {
    return EFI_OUT_OF_RESOURCES;
  }

  JsonValueWithOem = JsonValueInitObject ();
  if ((JsonValueWithOem == NULL) || !JsonValueIsObject (JsonValueWithOem)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEdkIIRedfishResourceAddendumProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto ON_RELEASE;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEdkIIRedfishResourceAddendumProtocolGuid,
                    (VOID **)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->OemCallback (Protocol, &SchemaInfo, JsonValueWithOem);
    if (!EFI_ERROR (Status)) {
      Status = JsonObjectSetValue (JsonValue, "Oem", JsonValueWithOem);
      if (!EFI_ERROR (Status)) {
        *JsonWithOem = JsonDumpString (JsonValue, EDKII_JSON_COMPACT);
      }

      break;
    }
  }

ON_RELEASE:

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (JsonValue != NULL) {
    JsonValueFree (JsonValue);
  }

  if (JsonValueWithOem != NULL) {
    JsonValueFree (JsonValueWithOem);
  }

  if (SchemaInfo.Uri != NULL) {
    FreePool (SchemaInfo.Uri);
  }

  return Status;
}
