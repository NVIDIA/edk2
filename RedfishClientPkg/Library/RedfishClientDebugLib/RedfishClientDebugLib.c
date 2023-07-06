/** @file
  Redfish debug library to debug Redfish application.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

#include <Guid/RedfishClientEventGroup.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishClientDebugLib.h>

#define REDFISH_JSON_STRING_LENGTH  200
#define REDFISH_JSON_OUTPUT_FORMAT  (EDKII_JSON_COMPACT | EDKII_JSON_INDENT(2))

/**

  This function dump the Json string in given error level.

  @param[in]  ErrorLevel  DEBUG macro error level
  @param[in]  JsonValue   Json value to dump.

  @retval     EFI_SUCCESS         Json string is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
DumpJsonValue (
  IN UINTN             ErrorLevel,
  IN EDKII_JSON_VALUE  JsonValue
  )
{
  CHAR8  *String;
  CHAR8  *Runner;
  CHAR8  Buffer[REDFISH_JSON_STRING_LENGTH + 1];
  UINTN  StrLen;
  UINTN  Count;
  UINTN  Index;

  if (JsonValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  String = JsonDumpString (JsonValue, REDFISH_JSON_OUTPUT_FORMAT);
  if (String == NULL) {
    return EFI_UNSUPPORTED;
  }

  StrLen = AsciiStrLen (String);
  if (StrLen == 0) {
    return EFI_UNSUPPORTED;
  }

  Count  = StrLen / REDFISH_JSON_STRING_LENGTH;
  Runner = String;
  for (Index = 0; Index < Count; Index++) {
    AsciiStrnCpyS (Buffer, (REDFISH_JSON_STRING_LENGTH + 1), Runner, REDFISH_JSON_STRING_LENGTH);
    Buffer[REDFISH_JSON_STRING_LENGTH] = '\0';
    DEBUG ((ErrorLevel, "%a", Buffer));
    Runner += REDFISH_JSON_STRING_LENGTH;
  }

  Count = StrLen % REDFISH_JSON_STRING_LENGTH;
  if (Count > 0) {
    DEBUG ((ErrorLevel, "%a", Runner));
  }

  DEBUG ((ErrorLevel, "\n"));

  FreePool (String);
  return EFI_SUCCESS;
}

/**

  This function dump the status code, header and body in given
  Redfish payload.

  @param[in]  ErrorLevel  DEBUG macro error level
  @param[in]  Payload     Redfish payload to dump

  @retval     EFI_SUCCESS         Redfish payload is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
DumpRedfishPayload (
  IN UINTN            ErrorLevel,
  IN REDFISH_PAYLOAD  Payload
  )
{
  EDKII_JSON_VALUE  JsonValue;

  if (Payload == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  JsonValue = RedfishJsonInPayload (Payload);
  if (JsonValue != NULL) {
    DEBUG ((ErrorLevel, "Payload:\n"));
    DumpJsonValue (ErrorLevel, JsonValue);
  }

  return EFI_SUCCESS;
}

/**

  This function dump the status code, header and body in given
  Redfish response.

  @param[in]  Message     Message string
  @param[in]  ErrorLevel  DEBUG macro error level
  @param[in]  Response    Redfish response to dump

  @retval     EFI_SUCCESS         Redfish response is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
DumpRedfishResponse (
  IN CONST CHAR8       *Message,
  IN UINTN             ErrorLevel,
  IN REDFISH_RESPONSE  *Response
  )
{
  UINTN  Index;

  if (Response == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IS_EMPTY_STRING (Message)) {
    DEBUG ((ErrorLevel, "%a\n", Message));
  }

  //
  // status code
  //
  if (Response->StatusCode != NULL) {
    switch (*(Response->StatusCode)) {
      case HTTP_STATUS_100_CONTINUE:
        DEBUG ((ErrorLevel, "Status code: 100 CONTINUE\n"));
        break;
      case HTTP_STATUS_200_OK:
        DEBUG ((ErrorLevel, "Status code: 200 OK\n"));
        break;
      case HTTP_STATUS_201_CREATED:
        DEBUG ((ErrorLevel, "Status code: 201 CREATED\n"));
        break;
      case HTTP_STATUS_202_ACCEPTED:
        DEBUG ((ErrorLevel, "Status code: 202 ACCEPTED\n"));
        break;
      case HTTP_STATUS_304_NOT_MODIFIED:
        DEBUG ((ErrorLevel, "Status code: 304 NOT MODIFIED\n"));
        break;
      case HTTP_STATUS_400_BAD_REQUEST:
        DEBUG ((ErrorLevel, "Status code: 400 BAD REQUEST\n"));
        break;
      case HTTP_STATUS_401_UNAUTHORIZED:
        DEBUG ((ErrorLevel, "Status code: 401 UNAUTHORIZED\n"));
        break;
      case HTTP_STATUS_403_FORBIDDEN:
        DEBUG ((ErrorLevel, "Status code: 403 FORBIDDEN\n"));
        break;
      case HTTP_STATUS_404_NOT_FOUND:
        DEBUG ((ErrorLevel, "Status code: 404 NOT FOUND\n"));
        break;
      case HTTP_STATUS_405_METHOD_NOT_ALLOWED:
        DEBUG ((ErrorLevel, "Status code: 405 METHOD NOT ALLOWED\n"));
        break;
      case HTTP_STATUS_500_INTERNAL_SERVER_ERROR:
        DEBUG ((ErrorLevel, "Status code: 500 INTERNAL SERVER ERROR\n"));
        break;
      default:
        DEBUG ((ErrorLevel, "Status code: 0x%x\n", *(Response->StatusCode)));
        break;
    }
  }

  //
  // header
  //
  if (Response->HeaderCount > 0) {
    DEBUG ((ErrorLevel, "Header: %d\n", Response->HeaderCount));
    for (Index = 0; Index < Response->HeaderCount; Index++) {
      DEBUG ((ErrorLevel, "  %a: %a\n", Response->Headers[Index].FieldName, Response->Headers[Index].FieldValue));
    }
  }

  //
  // Body
  //
  if (Response->Payload != NULL) {
    DumpRedfishPayload (ErrorLevel, Response->Payload);
  }

  return EFI_SUCCESS;
}