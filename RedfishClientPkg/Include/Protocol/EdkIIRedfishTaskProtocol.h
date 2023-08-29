/** @file
  This file defines the EDKII_REDFISH_TASK_PROTOCOL interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_TASK_PROTOCOL_H_
#define EDKII_REDFISH_TASK_PROTOCOL_H_

#include <Uefi.h>
#include <Protocol/Http.h>
#include <Library/RedfishMessageLib.h>

typedef struct _EDKII_REDFISH_TASK_PROTOCOL EDKII_REDFISH_TASK_PROTOCOL;

///
/// Definition of REDFISH_TASK_STATE.
///
typedef enum {
  RedfishTaskStateNew = 0x0,
  RedfishTaskStateStarting,
  RedfishTaskStateRunning,
  RedfishTaskStateSuspended,
  RedfishTaskStateInterrupted,
  RedfishTaskStatePending,
  RedfishTaskStateStopping,
  RedfishTaskStateCompleted,
  RedfishTaskStateKilled,
  RedfishTaskStateException,
  RedfishTaskStateService,
  RedfishTaskStateCancelling,
  RedfishTaskStateCancelled,
  RedfishTaskStateMax
} REDFISH_TASK_STATE;

///
/// Definition of REDFISH_TASK_STATUS.
///
typedef enum {
  RedfishTaskStatusOk = 0x0,
  RedfishTaskStatusWarning,
  RedfishTaskStatusCritical,
  RedfishTaskStatusMax
} REDFISH_TASK_STATUS;

///
/// Definition of REDFISH_TASK_RESULT.
///
typedef struct {
  REDFISH_TASK_STATE     TaskState;
  REDFISH_TASK_STATUS    TaskStatus;
} REDFISH_TASK_RESULT;

///
/// Definition of REDFISH_TASK_PAYLOAD.
///
typedef struct {
  EFI_HTTP_METHOD    HttpOperation;
  EFI_HTTP_HEADER    *Headers;
  UINTN              HeaderCount;
  CHAR8              *JsonBody;
  CHAR8              *TargetUri;
} REDFISH_TASK_PAYLOAD;

/**
  The callback function provided by Redfish feature driver.

  @param[in]     TaskId              The Task ID.
  @param[in]     JsonText            The context of task resource in JSON format.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] Result              The pointer to REDFISH_TASK_RESULT.
                                     On input, it is "TaskState" and "TaskStatus" in task resource.
                                     On ouput, it is "TaskState" and "TaskStatus" that will be update
                                     to task resource.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_CALLBACK)(
  IN     UINTN                       TaskId,
  IN     CHAR8                       *JsonText,
  IN     VOID                        *Context,
  IN OUT REDFISH_TASK_RESULT         *Result
  );

/**
  The registration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   ListenTaskUri       The URI in "Payload.TargetUri" from task resource that
                                   feature driver likes to listen. If the ListenTaskUri is
                                   matched in any of task resource, the corresponding Callback
                                   is called to serve this task in feature driver.
  @param[in]   PartialMatch        When PartialMatch is TRUE, StrStr() is used to do string matching.
                                   When PartialMatch is FALSE, StrCmp() is used to do string matching.
  @param[in]   Callback            Callback routine associated with this registration that
                                   provided by Redfish feature driver to execute the action
                                   on Redfish resource which is managed by this Redfish
                                   feature driver.
  @param[in]   Context             The context of the registering feature driver. The pointer
                                   to the context is delivered through callback function.

  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_REGISTER)(
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     EFI_STRING                   ListenTaskUri,
  IN     BOOLEAN                      PartialMatch,
  IN     REDFISH_TASK_CALLBACK        Callback,
  IN     VOID                         *Context  OPTIONAL
  );

/**
  The unregistration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   ListenTaskUri       The URI in "Payload.TargetUri" from task resource that
                                   feature driver likes to listen.
  @param[in]   Context             The context used for the previous feature driver
                                   registration.

  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_UNREGISTER)(
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     EFI_STRING                   ListenTaskUri,
  IN     VOID                         *Context OPTIONAL
  );

/**
  The report message function to report information about task.

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   TaskId              The Task ID.
  @param[in]   Message             String message attached to give task.
  @param[in]   MessageSeverity     Message severity.

  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_REPORT_MESSAGE)(
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     UINTN                        TaskId,
  IN     CHAR8                        *Message,
  IN     REDFISH_MESSAGE_SEVERITY     MessageSeverity
  );

/**
  This function read input JsonText and generate payload information in REDFISH_TASK_PAYLOAD.
  It's call responsibility to release returned Payload by calling FreePayload().

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   JsonText            The context of task resource in JSON format.
  @param[out]  Payload             Returned payload information.

  @retval EFI_SUCCESS              Task payload is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_GET_PAYLOAD)(
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     CHAR8                       *JsonText,
  OUT    REDFISH_TASK_PAYLOAD        **Payload
  );

/**
  This function release Payload resource which is returned by GetPayload().

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   Payload             The payload to release.

  @retval EFI_SUCCESS              Payload is released successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_TASK_FREE_PAYLOAD)(
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     REDFISH_TASK_PAYLOAD         *Payload
  );

///
/// Definition of _EDKII_REDFISH_TASK_PROTOCOL.
///
struct _EDKII_REDFISH_TASK_PROTOCOL {
  UINT32                         Version;
  REDFISH_TASK_REGISTER          Register;
  REDFISH_TASK_UNREGISTER        Unregister;
  REDFISH_TASK_REPORT_MESSAGE    ReportMessage;
  REDFISH_TASK_GET_PAYLOAD       GetPayload;
  REDFISH_TASK_FREE_PAYLOAD      FreePayload;
};

#define EDKII_REDFISH_TASK_PROTOCOL_REVISION  0x00001000

extern EFI_GUID  gEdkIIRedfishTaskProtocolGuid;

#endif
