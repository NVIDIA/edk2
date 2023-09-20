/** @file
  Definitions of RedfishTaskServiceJson

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_TASK_JSON_H_
#define EDKII_REDFISH_TASK_JSON_H_

#include <Base.h>
#include <RedfishBase.h>
#include <IndustryStandard/Http11.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/JsonLib.h>
#include <Library/DebugLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishHttpLib.h>
#include <Protocol/EdkIIRedfishTaskProtocol.h>

#define REDFISH_TASK_ID_ATTRIBUTE              "Id"
#define REDFISH_TASK_STATE_ATTRIBUTE           "TaskState"
#define REDFISH_TASK_PAYLOAD_ATTRIBUTE         "Payload"
#define REDFISH_TASK_TARGET_URI_ATTRIBUTE      "TargetUri"
#define REDFISH_TASK_HTTP_OPERATION_ATTRIBUTE  "HttpOperation"
#define REDFISH_TASK_JSON_BODY_ATTRIBUTE       "JsonBody"
#define REDFISH_TASK_HEADERS_ATTRIBUTE         "HttpHeaders"
#define REDFISH_ATTRIBUTE_ODATA_ID             "@odata.id"

/**
  This function read "TaskState" attribute from given task JSON object.

  @param[in]   TaskObject  Task JSON object.
  @param[out]  TaskState   Task state in TaskObject.

  @retval EFI_SUCCESS      Task state is returned successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskGetTaskState (
  IN EDKII_JSON_VALUE     TaskObject,
  OUT REDFISH_TASK_STATE  *TaskState
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  This function release task payload.

  @param[in]   Payload   Payload to release.

**/
VOID
RedfishTaskFreePayload (
  IN REDFISH_TASK_PAYLOAD  *Payload
  );

#endif
