/** @file
  RedfishTaskServiceDxe produces EdkIIRedfishTaskProtocol
  for EDK2 Redfish Feature driver registration.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishTaskServiceJson.h"
#include "RedfishTaskServiceData.h"

/**
  This function issues HTTP GET to query task from given URI. And return
  the task in JSON object format.

  @param[in]   RedfishService  Redfish service instance.
  @param[in]   Uri             URI to query.

  @retval EDKII_JSON_VALUE  Task object is returned successfully.
  @retval NULL              Some error happened.

**/
EDKII_JSON_VALUE
RedfishTaskGetObjectFromUri (
  IN REDFISH_SERVICE  RedfishService,
  IN EFI_STRING       Uri
  )
{
  EFI_STATUS        Status;
  REDFISH_RESPONSE  Response;
  EDKII_JSON_VALUE  TaskObj;
  EDKII_JSON_VALUE  ReturnedObj;

  if ((RedfishService == NULL) || IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  TaskObj     = NULL;
  ReturnedObj = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));
  Status = RedfishHttpGetResource (RedfishService, Uri, &Response, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get resource from: %s :%r\n", __func__, Uri, Status));
    goto ON_RELEASE;
  }

  if (Response.Payload == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: no payload returned from: %s\n", __func__, Uri));
    goto ON_RELEASE;
  }

  TaskObj = RedfishJsonInPayload (Response.Payload);
  if (TaskObj == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: no object returned from: %s\n", __func__, Uri));
    goto ON_RELEASE;
  }

  ReturnedObj = JsonValueClone (TaskObj);
  if (ReturnedObj == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resources\n", __func__));
    goto ON_RELEASE;
  }

ON_RELEASE:

  RedfishFreeResponse (
    Response.StatusCode,
    Response.HeaderCount,
    Response.Headers,
    Response.Payload
    );

  return ReturnedObj;
}

/**
  This function convert HTTP method in string to EFI_HTTP_METHOD.

  @param[in]   HttpMethodStr   HTTP method in string.

  @retval EFI_HTTP_METHOD   Http method.
**/
EFI_HTTP_METHOD
RedfishTaskGetHttpMethod (
  IN CONST CHAR8  *HttpMethodStr
  )
{
  if (IS_EMPTY_STRING (HttpMethodStr)) {
    return HttpMethodMax;
  }

  if (AsciiStrCmp (HTTP_METHOD_GET, HttpMethodStr) == 0) {
    return HttpMethodGet;
  } else if (AsciiStrCmp (HTTP_METHOD_HEAD, HttpMethodStr) == 0) {
    return HttpMethodHead;
  } else if (AsciiStrCmp (HTTP_METHOD_POST, HttpMethodStr) == 0) {
    return HttpMethodPost;
  } else if (AsciiStrCmp (HTTP_METHOD_PUT, HttpMethodStr) == 0) {
    return HttpMethodPut;
  } else if (AsciiStrCmp (HTTP_METHOD_DELETE, HttpMethodStr) == 0) {
    return HttpMethodDelete;
  } else if (AsciiStrCmp (HTTP_METHOD_TRACE, HttpMethodStr) == 0) {
    return HttpMethodTrace;
  } else if (AsciiStrCmp (HTTP_METHOD_CONNECT, HttpMethodStr) == 0) {
    return HttpMethodConnect;
  } else if (AsciiStrCmp (HTTP_METHOD_PATCH, HttpMethodStr) == 0) {
    return HttpMethodPatch;
  }

  return HttpMethodMax;
}

/**
  This function read "TaskState" attribute from given task JSON object.

  @param[in]   TaskObject  Task JSON object.
  @param[out]  TaskState   Task state in TaskObject.

  @retval EFI_SUCCESS      Task state is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetTaskState (
  IN  EDKII_JSON_VALUE    TaskObject,
  OUT REDFISH_TASK_STATE  *TaskState
  )
{
  EDKII_JSON_VALUE  TaskStateObj;
  CHAR8             *TaskStateStr;

  if ((TaskObject == NULL) || (TaskState == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *TaskState = RedfishTaskStateMax;

  TaskStateObj = JsonObjectGetValue (JsonValueGetObject (TaskObject), REDFISH_TASK_STATE_ATTRIBUTE);
  if (TaskStateObj == NULL) {
    return EFI_NOT_FOUND;
  }

  TaskStateStr = (CHAR8 *)JsonValueGetAsciiString (TaskStateObj);
  if (TaskStateStr == NULL) {
    return EFI_NOT_FOUND;
  }

  *TaskState = RedfishTaskStateConverter (TaskStateStr);

  return EFI_SUCCESS;
}

/**
  This function read "Id" attribute from given task JSON object.

  @param[in]   TaskObject  Task JSON object.
  @param[out]  TaskId      Task ID in TaskObject.

  @retval EFI_SUCCESS      Task ID is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetTaskId (
  IN  EDKII_JSON_VALUE  TaskObject,
  OUT UINTN             *TaskId
  )
{
  EDKII_JSON_VALUE  TaskIdObj;
  CHAR8             *TaskIdStr;

  if ((TaskObject == NULL) || (TaskId == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *TaskId = 0x00;

  //
  // Check task state first
  //
  TaskIdObj = JsonObjectGetValue (JsonValueGetObject (TaskObject), REDFISH_TASK_ID_ATTRIBUTE);
  if (TaskIdObj == NULL) {
    return EFI_NOT_FOUND;
  }

  TaskIdStr = (CHAR8 *)JsonValueGetAsciiString (TaskIdObj);
  if (TaskIdStr == NULL) {
    return EFI_NOT_FOUND;
  }

  *TaskId = AsciiStrDecimalToUintn (TaskIdStr);

  return EFI_SUCCESS;
}

/**
  This function read "@odata.id" attribute from given task JSON object.
  It's call responsibility to release TaskUri by calling FreePoo().

  @param[in]   TaskObject  Task JSON object.
  @param[out]  TaskUri     Task URI in TaskObject.

  @retval EFI_SUCCESS      Task URI is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetTaskUri (
  IN  EDKII_JSON_VALUE  TaskObject,
  OUT EFI_STRING        *TaskUri
  )
{
  EDKII_JSON_VALUE  OdataIdObj;
  EFI_STRING        TaskUriStr;

  if ((TaskObject == NULL) || (TaskUri == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *TaskUri   = NULL;
  OdataIdObj = JsonObjectGetValue (JsonValueGetObject (TaskObject), REDFISH_ATTRIBUTE_ODATA_ID);
  if (OdataIdObj == NULL) {
    return EFI_NOT_FOUND;
  }

  TaskUriStr = JsonValueGetUnicodeString (OdataIdObj);
  if (TaskUriStr == NULL) {
    return EFI_NOT_FOUND;
  }

  *TaskUri = TaskUriStr;

  return EFI_SUCCESS;
}

/**
  This function read "TargetUri" attribute from given task JSON object.
  It's call responsibility to release TargetUri by calling FreePoo().

  @param[in]   TaskObject  Task JSON object.
  @param[out]  TargetUri   Task target URI in TaskObject.

  @retval EFI_SUCCESS      Target URI is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetTargetUri (
  IN  EDKII_JSON_VALUE  TaskObject,
  OUT EFI_STRING        *TargetUri
  )
{
  EDKII_JSON_VALUE  PayloadObj;
  EDKII_JSON_VALUE  TargetUriObj;
  EFI_STRING        TargetUriStr;

  if ((TaskObject == NULL) || (TargetUri == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *TargetUri = NULL;
  PayloadObj = JsonObjectGetValue (JsonValueGetObject (TaskObject), REDFISH_TASK_PAYLOAD_ATTRIBUTE);
  if (PayloadObj == NULL) {
    return EFI_NOT_FOUND;
  }

  TargetUriObj = JsonObjectGetValue (JsonValueGetObject (PayloadObj), REDFISH_TASK_TARGET_URI_ATTRIBUTE);
  if (TargetUriObj == NULL) {
    return EFI_NOT_FOUND;
  }

  TargetUriStr = JsonValueGetUnicodeString (TargetUriObj);
  if (TargetUriStr == NULL) {
    return EFI_NOT_FOUND;
  }

  *TargetUri = TargetUriStr;

  return EFI_SUCCESS;
}

/**
  This function release task payload.

  @param[in]   Payload   Payload to release.

**/
VOID
RedfishTaskFreePayload (
  IN REDFISH_TASK_PAYLOAD  *Payload
  )
{
  if (Payload == NULL) {
    return;
  }

  if (Payload->JsonBody != NULL) {
    FreePool (Payload->JsonBody);
  }

  if (Payload->TargetUri != NULL) {
    FreePool (Payload->TargetUri);
  }

  FreePool (Payload);
}

/**
  This function read "Payload" attribute from given task JSON object.
  It's call responsibility to release Payload by calling RedfishTaskFreePayload().

  @param[in]   TaskObject  Task JSON object.
  @param[out]  Payload     Task payload in TaskObject.

  @retval EFI_SUCCESS      Target payload is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetPayload (
  IN  EDKII_JSON_VALUE      TaskObject,
  OUT REDFISH_TASK_PAYLOAD  **Payload
  )
{
  EFI_STATUS            Status;
  EDKII_JSON_VALUE      PayloadObject;
  EDKII_JSON_VALUE      OperationObject;
  EDKII_JSON_VALUE      JsonBodyObject;
  EDKII_JSON_VALUE      TargetUriObject;
  CONST CHAR8           *BufferStr;
  REDFISH_TASK_PAYLOAD  *NewPayload;

  if ((TaskObject == NULL) || (Payload == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Payload  = NULL;
  BufferStr = NULL;

  PayloadObject = JsonObjectGetValue (JsonValueGetObject (TaskObject), REDFISH_TASK_PAYLOAD_ATTRIBUTE);
  if (PayloadObject == NULL) {
    return EFI_NOT_FOUND;
  }

  OperationObject = JsonObjectGetValue (JsonValueGetObject (PayloadObject), REDFISH_TASK_HTTP_OPERATION_ATTRIBUTE);
  if (OperationObject == NULL) {
    return EFI_NOT_FOUND;
  }

  JsonBodyObject = JsonObjectGetValue (JsonValueGetObject (PayloadObject), REDFISH_TASK_JSON_BODY_ATTRIBUTE);
  if (JsonBodyObject == NULL) {
    return EFI_NOT_FOUND;
  }

  TargetUriObject = JsonObjectGetValue (JsonValueGetObject (PayloadObject), REDFISH_TASK_TARGET_URI_ATTRIBUTE);
  if (TargetUriObject == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // TODO: handle headers
  //

  NewPayload = AllocateZeroPool (sizeof (REDFISH_TASK_PAYLOAD));
  if (NewPayload == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // JsonBody
  //
  BufferStr = JsonValueGetAsciiString (JsonBodyObject);
  if (BufferStr == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_ERROR;
  }

  NewPayload->JsonBody = AllocateCopyPool (AsciiStrSize (BufferStr), BufferStr);
  if (NewPayload->JsonBody == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  //
  // HttpOperation
  //
  BufferStr = JsonValueGetAsciiString (OperationObject);
  if (BufferStr == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_ERROR;
  }

  NewPayload->HttpOperation = RedfishTaskGetHttpMethod (BufferStr);

  //
  // TargetUri
  //
  BufferStr = JsonValueGetAsciiString (TargetUriObject);
  if (BufferStr == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_ERROR;
  }

  NewPayload->TargetUri = AllocateCopyPool (AsciiStrSize (BufferStr), BufferStr);
  if (NewPayload->TargetUri == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  *Payload = NewPayload;

  return EFI_SUCCESS;

ON_ERROR:

  RedfishTaskFreePayload (NewPayload);

  return Status;
}
