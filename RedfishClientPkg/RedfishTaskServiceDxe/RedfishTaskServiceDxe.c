/** @file
  RedfishTaskServiceDxe produces EdkIIRedfishTaskProtocol
  for EDK2 Redfish Feature driver registration.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishTaskServiceDxe.h"
#include "RedfishTaskServiceData.h"
#include "RedfishTaskServiceJson.h"

REDFISH_TASK_SERVICE_PRIVATE  *mRedfishTaskServicePrivate = NULL;

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
EFI_STATUS
EFIAPI
RedfishTaskServiceRegister (
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     EFI_STRING                   ListenTaskUri,
  IN     BOOLEAN                      PartialMatch,
  IN     REDFISH_TASK_CALLBACK        Callback,
  IN     VOID                         *Context  OPTIONAL
  )
{
  REDFISH_TASK_SERVICE_PRIVATE   *Private;
  REDFISH_TASK_REGISTERED_ENTRY  *NewEntry;

  if ((This == NULL) || IS_EMPTY_STRING (ListenTaskUri) || (Callback == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_THIS (This);

  //
  // Check to see if this callback is registered or not
  //
  NewEntry = RedfishTaskFindRegisteredEntry (Private, ListenTaskUri, Context);
  if (NewEntry != NULL) {
    return EFI_ALREADY_STARTED;
  }

  return RedfishTaskAddRegisteredEntry (
           Private,
           ListenTaskUri,
           PartialMatch,
           Callback,
           Context,
           NULL
           );
}

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
EFI_STATUS
EFIAPI
RedfishTaskServiceUnregister (
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     EFI_STRING                   ListenTaskUri,
  IN     VOID                         *Context OPTIONAL
  )
{
  REDFISH_TASK_SERVICE_PRIVATE   *Private;
  REDFISH_TASK_REGISTERED_ENTRY  *Entry;

  if ((This == NULL) || IS_EMPTY_STRING (ListenTaskUri)) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_THIS (This);

  Entry = RedfishTaskFindRegisteredEntry (Private, ListenTaskUri, Context);
  if (Entry == NULL) {
    return EFI_NOT_FOUND;
  }

  return RedfishTaskRemoveRegisteredEntry (Private, Entry);
}

/**
  The report message function to report information about task.

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   TaskId              The Task ID.
  @param[in]   Message             String message attached to give task.
  @param[in]   MessageSeverity     Message severity.

  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceReportMessage (
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     UINTN                        TaskId,
  IN     CHAR8                        *Message,
  IN     REDFISH_MESSAGE_SEVERITY     MessageSeverity
  )
{
  REDFISH_TASK_SERVICE_PRIVATE  *Private;
  EFI_STATUS                    Status;
  REDFISH_TASK_ENTRY            *TargetEntry;

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_THIS (This);

  TargetEntry = RedfishTaskFindEntry (Private, TaskId);
  if (TargetEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = RedfishMessageReport (TargetEntry->TaskUri, Message, NULL, MessageSeverity);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: report message to task %d failed: %r\n", __func__, TaskId, Status));
  }

  return Status;
}

/**
  This function read input JsonText and generate payload information in REDFISH_TASK_PAYLOAD.
  It's call responsibility to release returned Payload by calling FreePayload().

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   JsonText            The context of task resource in JSON format.
  @param[out]  Payload             Returned payload information.

  @retval EFI_SUCCESS              Task payload is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceGetPayload (
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     CHAR8                        *JsonText,
  OUT    REDFISH_TASK_PAYLOAD         **Payload
  )
{
  REDFISH_TASK_SERVICE_PRIVATE  *Private;
  EDKII_JSON_VALUE              JsonObject;
  EFI_STATUS                    Status;

  if ((This == NULL) || (Payload == NULL) || IS_EMPTY_STRING (JsonText)) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_THIS (This);

  JsonObject = JsonLoadString (JsonText, 0, NULL);
  if (JsonObject == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: invalid JSON string\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishTaskGetPayload (JsonObject, Payload);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get payload: %r\n", __func__, Status));
  }

  if (JsonObject != NULL) {
    JsonValueFree (JsonObject);
  }

  return Status;
}

/**
  This function release Payload resource which is returned by GetPayload().

  @param[in]   This                Pointer to EDKII_REDFISH_TASK_PROTOCOL instance.
  @param[in]   Payload             The payload to release.

  @retval EFI_SUCCESS              Payload is released successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceFreePayload (
  IN     EDKII_REDFISH_TASK_PROTOCOL  *This,
  IN     REDFISH_TASK_PAYLOAD         *Payload
  )
{
  if ((This == NULL) || (Payload == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RedfishTaskFreePayload (Payload);

  return EFI_SUCCESS;
}

/**
  This function handle invidual task request downloaded from BMC.

  @param[in]   Private     Pointer to private instance
  @param[in]   TaskObject  The Task JSON object.

  @retval EFI_SUCCESS      This task request is handled without error return.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskHandler (
  IN REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN EDKII_JSON_VALUE              TaskObject
  )
{
  EFI_STATUS             Status;
  CHAR8                  *JsonText;
  REDFISH_TASK_STATE     TaskState;
  UINTN                  TaskId;
  REDFISH_TASK_CALLBACK  CallbackFunc;
  VOID                   *CallbackContext;
  REDFISH_TASK_ENTRY     *TaskEntry;
  EFI_STRING             TargetUri;
  EFI_STRING             Uri;

  if ((Private == NULL) || (TaskObject == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Uri             = NULL;
  TargetUri       = NULL;
  TaskEntry       = NULL;
  CallbackFunc    = NULL;
  CallbackContext = NULL;
  JsonText        = NULL;

  //
  // Get task URI
  //
  Status = RedfishTaskGetTaskUri (TaskObject, &Uri);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get task URI: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_TASK_DEBUG, "%a: handle task: %s\n", __func__, Uri));

  //
  // Check task state first
  //
  Status =  RedfishTaskGetTaskState (TaskObject, &TaskState);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get task state from: %s :%r\n", __func__, Uri, Status));
    goto ON_RELEASE;
  }

  //
  // We only handle below task state
  //
  if ((TaskState != RedfishTaskStateNew) &&
      (TaskState != RedfishTaskStatePending) &&
      (TaskState != RedfishTaskStateRunning) &&
      (TaskState != RedfishTaskStateStarting))
  {
    DEBUG ((REDFISH_TASK_DEBUG, "%a: %s Task state: 0x%x, ignore\n", __func__, Uri, TaskState));
    Status = EFI_UNSUPPORTED;
    goto ON_RELEASE;
  }

  //
  // Get target URI
  //
  Status = RedfishTaskGetTargetUri (TaskObject, &TargetUri);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get target URI from: %s :%r\n", __func__, Uri, Status));
    goto ON_RELEASE;
  }

  //
  // Check and see if we have callback registered for this target URI or not.
  //
  Status = RedfishTaskFindCallback (Private, TargetUri, &CallbackFunc, &CallbackContext);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_TASK_DEBUG, "%a: no callback handler for task: %s\n", __func__, TargetUri));
      return EFI_SUCCESS;
    }

    DEBUG ((DEBUG_ERROR, "%a: failed to find callback handler for task: %s:%r\n", __func__, TargetUri, Status));
    return Status;
  }

  //
  // callback found. Prepare data for callback.
  //

  //
  // Get task ID
  //
  Status = RedfishTaskGetTaskId (TaskObject, &TaskId);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get task ID from: %s :%r\n", __func__, Uri, Status));
    goto ON_RELEASE;
  }

  //
  // Context
  //
  JsonText = JsonDumpString (TaskObject, EDKII_JSON_COMPACT);
  if (JsonText == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_RELEASE;
  }

  //
  // Track this task.
  //
  Status = RedfishTaskAddEntry (Private, TaskId, Uri, JsonText, TaskState, RedfishTaskStatusCritical, &TaskEntry);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not add task entry: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  //
  // Invoke registered callback
  //
  Status = CallbackFunc (
             TaskEntry->TaskId,
             TaskEntry->JsonText,
             CallbackContext,
             &TaskEntry->Result
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %s callback return: %r\n", __func__, TaskEntry->TaskUri, Status));
  }

  //
  // Update task state and task status back to task. We still do this when callback function
  // returns failure.
  //
  Status = RedfishTaskUpdate (Private->RedfishService, TaskEntry->TaskUri, TaskEntry->Result);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %s task update failure: %r\n", __func__, TaskEntry->TaskUri, Status));
    goto ON_RELEASE;
  }

ON_RELEASE:

  if (JsonText != NULL) {
    FreePool (JsonText);
  }

  if (TargetUri != NULL) {
    FreePool (TargetUri);
  }

  if (Uri != NULL) {
    FreePool (Uri);
  }

  return Status;
}

/**
  This is Redfish Task dispatcher. This function download all tasks from Redfish
  Task Service and dispatch them to corresponding task handler.

  @param[in]   Private     Pointer to private instance.
  @param[in]   Uri         The URI of Task Service collection.

  @retval EFI_SUCCESS      Tasks are dispatched successfully.
  @retval Others           Some error happened.

**/
EFI_STATUS
RedfishTaskServiceDispatch (
  IN REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN EFI_STRING                    Uri
  )
{
  EFI_STATUS        Status;
  EDKII_JSON_VALUE  TaskCollection;
  EDKII_JSON_VALUE  TaskArrayObj;
  EDKII_JSON_ARRAY  TaskArray;
  EDKII_JSON_VALUE  Value;
  UINTN             Index;

 #if !REDFISH_PARAMETER_EXPAND_ENABLED
  EDKII_JSON_VALUE  OdataId;
  EDKII_JSON_VALUE  TaskObj;
  EFI_STRING        TaskUri;
 #endif

  if ((Private == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_TASK_DEBUG, "%a: dispatch tasks at: %s\n", __func__, Uri));

  //
  // Get tasks from task collection
  //
  TaskCollection = RedfishTaskGetObjectFromUri (Private->RedfishService, Uri);
  if (TaskCollection == NULL) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Go through Members array
  //
  TaskArrayObj = JsonObjectGetValue (JsonValueGetObject (TaskCollection), REDFISH_ATTRIBUTE_MEMBERS);
  if (TaskArrayObj == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_RELEASE;
  }

  TaskArray = JsonValueGetArray (TaskArrayObj);
  if (TaskArray == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_RELEASE;
  }

  if (JsonArrayCount (TaskArray) == 0) {
    DEBUG ((REDFISH_TASK_DEBUG, "%a: No task available\n", __func__));
    Status = EFI_SUCCESS;
    goto ON_RELEASE;
  }

  //
  // Go through task array
  //
  EDKII_JSON_ARRAY_FOREACH (TaskArray, Index, Value) {
 #if REDFISH_PARAMETER_EXPAND_ENABLED
    Status = RedfishTaskHandler (Private, Value);
 #else
    OdataId = JsonObjectGetValue (JsonValueGetObject (Value), REDFISH_ATTRIBUTE_ODATA_ID);
    if (OdataId == NULL) {
      continue;
    }

    TaskUri = JsonValueGetUnicodeString (OdataId);
    if (TaskUri == NULL) {
      continue;
    }

    TaskObj = RedfishTaskGetObjectFromUri (Private->RedfishService, TaskUri);
    if (TaskObj == NULL) {
      continue;
    }

    Status = RedfishTaskHandler (Private, TaskObj);

    //
    // Release resource
    //
    FreePool (TaskUri);
    JsonValueFree (TaskObj);
 #endif
    if (EFI_ERROR (Status)) {
      if (Status != EFI_UNSUPPORTED) {
        DEBUG ((DEBUG_ERROR, "%a: Redfish task handler failure: %r\n", __func__, Status));
      } else {
        Status = EFI_SUCCESS;
      }
    }
  }

ON_RELEASE:

  if (TaskCollection != NULL) {
    JsonValueFree (TaskCollection);
  }

  return Status;
}

/**
  The callback function provided by Redfish Feature driver.

  @param[in]     This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]     FeatureAction       The action Redfish feature driver should take.
  @param[in]     Uri                 The collection URI.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] InformationExchange The pointer to RESOURCE_INFORMATION_EXCHANGE

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceFeatureCallback (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN     FEATURE_CALLBACK_ACTION         FeatureAction,
  IN     VOID                            *Context,
  IN OUT RESOURCE_INFORMATION_EXCHANGE   *InformationExchange
  )
{
  EFI_STATUS                    Status;
  REDFISH_TASK_SERVICE_PRIVATE  *Private;
  EFI_STRING                    RedfishVersion;
  EFI_STRING                    ResourceUri;
  UINTN                         BufferSize;

  if (FeatureAction != CallbackActionStartOperation) {
    return EFI_UNSUPPORTED;
  }

  Private = (REDFISH_TASK_SERVICE_PRIVATE *)Context;
  if (Private->RedfishService == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: no Redfish service configured\n", __func__));
    return EFI_NOT_READY;
  }

  RedfishVersion = NULL;
  ResourceUri    = NULL;
  BufferSize     = MAX_URI_LENGTH * sizeof (CHAR16);

  //
  // Find Redfish version on BMC
  //
  RedfishVersion = RedfishGetVersion (Private->RedfishService);

  //
  // Create the full URI from Redfish service root.
  //
  ResourceUri = (EFI_STRING)AllocateZeroPool (BufferSize);
  if (ResourceUri == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for full URI.\n", __func__));
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

 #if REDFISH_PARAMETER_EXPAND_ENABLED
  UnicodeSPrint (ResourceUri, BufferSize, L"%s%s/%s%s", RedfishVersion, InformationExchange->SendInformation.FullUri, REDFISH_TASK_COLLECTION_URI, REDFISH_EXPAND_PARAMETER);
 #else
  UnicodeSPrint (ResourceUri, BufferSize, L"%s%s/%s", RedfishVersion, InformationExchange->SendInformation.FullUri, REDFISH_TASK_COLLECTION_URI);
 #endif
  Status = RedfishTaskServiceDispatch (Private, ResourceUri);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to dispatch Redfish tasks: %r\n", __func__, Status));
  }

ON_RELEASE:

  if (RedfishVersion != NULL) {
    FreePool (RedfishVersion);
  }

  if (ResourceUri != NULL) {
    FreePool (ResourceUri);
  }

  return Status;
}

/**
  Initialize a Redfish configure handler.

  This function will be called by the Redfish config driver to initialize each Redfish configure
  handler.

  @param[in]   This                     Pointer to EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL instance.
  @param[in]   RedfishConfigServiceInfo Redfish service information.

  @retval EFI_SUCCESS                  The handler has been initialized successfully.
  @retval EFI_DEVICE_ERROR             Failed to create or configure the REST EX protocol instance.
  @retval EFI_ALREADY_STARTED          This handler has already been initialized.
  @retval Other                        Error happens during the initialization.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceInit (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This,
  IN  REDFISH_CONFIG_SERVICE_INFORMATION     *RedfishConfigServiceInfo
  )
{
  REDFISH_TASK_SERVICE_PRIVATE  *Private;

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_CONFIG_PROTOCOL (This);

  Private->RedfishService = RedfishCreateService (RedfishConfigServiceInfo);
  if (Private->RedfishService == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Stop a Redfish configure handler.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceStop (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This
  )
{
  REDFISH_TASK_SERVICE_PRIVATE  *Private;

  Private = REDFISH_TASK_SERVICE_PRIVATE_FROM_CONFIG_PROTOCOL (This);

  if (Private->RedfishService != NULL) {
    RedfishCleanupService (Private->RedfishService);
    Private->RedfishService = NULL;
  }

  if (Private->FeatureProtocol != NULL) {
    Private->FeatureProtocol->Unregister (
                                Private->FeatureProtocol,
                                REDFISH_TASK_SERVICE_URI,
                                (VOID *)mRedfishTaskServicePrivate
                                );
  }

  return EFI_SUCCESS;
}

/**
  Callback function when gEdkIIRedfishFeatureProtocolGuid is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
EdkIIRedfishFeatureProtocolIsReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                      Status;
  EDKII_REDFISH_FEATURE_PROTOCOL  *FeatureProtocol;

  if (mRedfishTaskServicePrivate == NULL) {
    return;
  }

  if (mRedfishTaskServicePrivate->FeatureProtocol != NULL) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEdkIIRedfishFeatureProtocolGuid,
                  NULL,
                  (VOID **)&FeatureProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate Redfish feature protocol: %r\n", __func__, Status));
    return;
  }

  Status = FeatureProtocol->Register (
                              FeatureProtocol,
                              REDFISH_TASK_SERVICE_URI,
                              RedfishTaskServiceFeatureCallback,
                              (VOID *)mRedfishTaskServicePrivate
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to register %s: %r\n", __func__, REDFISH_TASK_SERVICE_URI, Status));
  }

  mRedfishTaskServicePrivate->FeatureProtocol = FeatureProtocol;

  gBS->CloseEvent (Event);
}

/**
  Callback function executed when the AfterProvisioning event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[out]  Context  Pointer to the Context buffer

**/
VOID
EFIAPI
RedfishTaskServiceOnRedfishAfterProvisioning (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  EFI_STATUS                     Status;
  LIST_ENTRY                     *Node;
  LIST_ENTRY                     *NextNode;
  REDFISH_TASK_REGISTERED_ENTRY  *RegisteredEntry;
  REDFISH_TASK_ENTRY             *TaskEntry;

  if (mRedfishTaskServicePrivate == NULL) {
    return;
  }

  gBS->CloseEvent (Event);
  mRedfishTaskServicePrivate->RedfishEvent = NULL;

  //
  // Release all resources.
  //
  mRedfishTaskServicePrivate->ConfigHandler.Stop (&mRedfishTaskServicePrivate->ConfigHandler);
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  &mRedfishTaskServicePrivate->ImageHandle,
                  &gEdkIIRedfishTaskProtocolGuid,
                  &mRedfishTaskServicePrivate->TaskProtocol,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  &mRedfishTaskServicePrivate->ConfigHandler,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot uninstall protocols: %r\n", __func__, Status));
    return;
  }

  if (!IsListEmpty (&mRedfishTaskServicePrivate->RegisteredList)) {
    Node = GetFirstNode (&mRedfishTaskServicePrivate->RegisteredList);
    while (!IsNull (&mRedfishTaskServicePrivate->RegisteredList, Node)) {
      NextNode        = GetNextNode (&mRedfishTaskServicePrivate->RegisteredList, Node);
      RegisteredEntry = REDFISH_TASK_REGISTERED_ENTRY_FROM_LIST (Node);
      RedfishTaskRemoveRegisteredEntry (mRedfishTaskServicePrivate, RegisteredEntry);
      Node = NextNode;
    }
  }

  if (!IsListEmpty (&mRedfishTaskServicePrivate->TaskList)) {
    Node = GetFirstNode (&mRedfishTaskServicePrivate->TaskList);
    while (!IsNull (&mRedfishTaskServicePrivate->TaskList, Node)) {
      NextNode  = GetNextNode (&mRedfishTaskServicePrivate->TaskList, Node);
      TaskEntry = REDFISH_TASK_ENTRY_FROM_LIST (Node);
      RedfishTaskRemoveEntry (mRedfishTaskServicePrivate, TaskEntry);
      Node = NextNode;
    }
  }

  FreePool (mRedfishTaskServicePrivate);
  mRedfishTaskServicePrivate = NULL;
}

EDKII_REDFISH_TASK_PROTOCOL  mRedfishTaskProtocol = {
  EDKII_REDFISH_TASK_PROTOCOL_REVISION,
  RedfishTaskServiceRegister,
  RedfishTaskServiceUnregister,
  RedfishTaskServiceReportMessage,
  RedfishTaskServiceGetPayload,
  RedfishTaskServiceFreePayload
};

EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  mRedfishConfigHandler = {
  RedfishTaskServiceInit,
  RedfishTaskServiceStop
};

/**
  Main entry for this driver.

  @param[in] ImageHandle     Image handle this driver.
  @param[in] SystemTable     Pointer to SystemTable.

  @retval EFI_SUCCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
RedfishTaskServiceEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;

  if (mRedfishTaskServicePrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mRedfishTaskServicePrivate = AllocateZeroPool (sizeof (REDFISH_TASK_SERVICE_PRIVATE));
  if (mRedfishTaskServicePrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mRedfishTaskServicePrivate->ImageHandle = ImageHandle;
  CopyMem (&mRedfishTaskServicePrivate->TaskProtocol, &mRedfishTaskProtocol, sizeof (EDKII_REDFISH_TASK_PROTOCOL));
  CopyMem (&mRedfishTaskServicePrivate->ConfigHandler, &mRedfishConfigHandler, sizeof (EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL));
  InitializeListHead (&mRedfishTaskServicePrivate->RegisteredList);
  InitializeListHead (&mRedfishTaskServicePrivate->TaskList);

  //
  // Install the gEdkIIRedfishTaskProtocolGuid onto Handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mRedfishTaskServicePrivate->ImageHandle,
                  &gEdkIIRedfishTaskProtocolGuid,
                  &mRedfishTaskServicePrivate->TaskProtocol,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  &mRedfishTaskServicePrivate->ConfigHandler,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot install Redfish task protocol: %r\n", __func__, Status));
    return Status;
  }

  EfiCreateProtocolNotifyEvent (
    &gEdkIIRedfishFeatureProtocolGuid,
    TPL_CALLBACK,
    EdkIIRedfishFeatureProtocolIsReady,
    NULL,
    &Registration
    );

  Status = CreateAfterProvisioningEvent (
             RedfishTaskServiceOnRedfishAfterProvisioning,
             (VOID *)mRedfishTaskServicePrivate,
             &mRedfishTaskServicePrivate->RedfishEvent
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot create Redfish after provisioning event: %r\n", __func__, Status));
    return Status;
  }

  return EFI_SUCCESS;
}
