/** @file
  RedfishTaskServiceDxe produces EdkIIRedfishTaskProtocol
  for EDK2 Redfish Feature driver registration.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishTaskServiceData.h"

/**
  This function release task entry.

  @param[in]  Entry      Instance to be released.

**/
VOID
RedfishTaskFreeEntry (
  IN REDFISH_TASK_ENTRY  *Entry
  )
{
  if (Entry == NULL) {
    return;
  }

  if (Entry->JsonText != NULL) {
    FreePool (Entry->JsonText);
  }

  if (Entry->TaskUri != NULL) {
    FreePool (Entry->TaskUri);
  }

  FreePool (Entry);
}

/**
  This function create new task entry with given values.

  @param[in]  TaskId     ID of this task.
  @param[in]  TaskUri    URI string of this task.
  @param[in]  JsonText   JSON text in this task.

  @retval REDFISH_TASK_ENTRY *  The pointer to newly created task entry.
  @retval NULL                  Error occurs.
**/
REDFISH_TASK_ENTRY *
RedfishTaskNewEntry (
  IN UINT16      TaskId,
  IN EFI_STRING  TaskUri,
  IN CHAR8       *JsonText
  )
{
  REDFISH_TASK_ENTRY  *NewEntry;

  if (IS_EMPTY_STRING (TaskUri) || IS_EMPTY_STRING (JsonText)) {
    return NULL;
  }

  NewEntry = AllocateZeroPool (sizeof (REDFISH_TASK_ENTRY));
  if (NewEntry == NULL) {
    return NULL;
  }

  NewEntry->TaskUri = AllocateCopyPool (StrSize (TaskUri), TaskUri);
  if (NewEntry->TaskUri == NULL) {
    goto ON_ERROR;
  }

  NewEntry->JsonText = AllocateCopyPool (AsciiStrSize (JsonText), JsonText);
  if (NewEntry->JsonText == NULL) {
    goto ON_ERROR;
  }

  NewEntry->TaskId            = TaskId;
  NewEntry->Result.TaskState  = RedfishTaskStateMax;
  NewEntry->Result.TaskStatus = RedfishTaskStatusMax;

  return NewEntry;

ON_ERROR:

  RedfishTaskFreeEntry (NewEntry);

  return NULL;
}

/**
  This function create new task entry and add it into task list.
  It's call responsibility to release returned entry by calling
  RedfishTaskRemoveEntry().

  @param[in]  TaskId     ID of this task.
  @param[in]  TaskUri    URI string of this task.
  @param[in]  JsonText   JSON text in this task.
  @param[in]  TaskState  Task state.
  @param[in]  TaskStatus Task status.
  @param[in]  Entry      Newly created task entry on return.

  @retval EFI_SUCCESS         task is created successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RedfishTaskAddEntry (
  IN  REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN  UINT16                        TaskId,
  IN  EFI_STRING                    TaskUri,
  IN  CHAR8                         *JsonText,
  IN  REDFISH_TASK_STATE            TaskState,
  IN  REDFISH_TASK_STATUS           TaskStatus,
  OUT REDFISH_TASK_ENTRY            **Entry OPTIONAL
  )
{
  REDFISH_TASK_ENTRY  *NewEntry;

  if ((Private == NULL) || (TaskState >= RedfishTaskStateMax) || (TaskStatus >= RedfishTaskStatusMax)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Private->TaskCount >= REDFISH_TASK_MAX_ENTRY) {
    DEBUG ((DEBUG_ERROR, "%a: maximum of task entry reached %d/%d\n", __func__, Private->TaskCount, REDFISH_TASK_MAX_ENTRY));
    return EFI_OUT_OF_RESOURCES;
  }

  NewEntry = RedfishTaskNewEntry (TaskId, TaskUri, JsonText);
  if (NewEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewEntry->Result.TaskState  = TaskState;
  NewEntry->Result.TaskStatus = TaskStatus;

  InsertTailList (&Private->TaskList, &NewEntry->List);
  Private->TaskCount += 1;

  if (Entry != NULL) {
    *Entry = NewEntry;
  }

  return EFI_SUCCESS;
}

/**
  This function remove task entry from task list.

  @param[in]  Private        Pointer to private instance.
  @param[in]  Entry          Entry to be removed.

  @retval EFI_SUCCESS        Task entry is removed successfully.
  @retval Others             Error occurs.
**/
EFI_STATUS
RedfishTaskRemoveEntry (
  IN REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN REDFISH_TASK_ENTRY            *Entry
  )
{
  if ((Private == NULL) || (Entry == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&Entry->List);
  RedfishTaskFreeEntry (Entry);
  Private->TaskCount -= 1;

  return EFI_SUCCESS;
}

/**
  This function find targe task entry by given task ID.

  @param[in]  Private        Pointer to private instance.
  @param[in]  TaskId         The target task ID.

  @retval REDFISH_TASK_ENTRY *  Task entry is found and returned.
  @retval NULL                  Task entry cannot be found.
**/
REDFISH_TASK_ENTRY *
RedfishTaskFindEntry (
  IN REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN UINTN                         TaskId
  )
{
  LIST_ENTRY          *Node;
  REDFISH_TASK_ENTRY  *Entry;

  if (Private == NULL) {
    return NULL;
  }

  if (IsListEmpty (&Private->TaskList)) {
    return NULL;
  }

  Node = GetFirstNode (&Private->TaskList);
  while (!IsNull (&Private->TaskList, Node)) {
    Entry =  REDFISH_TASK_ENTRY_FROM_LIST (Node);

    if (Entry->TaskId == TaskId) {
      return Entry;
    }

    Node = GetNextNode (&Private->TaskList, Node);
  }

  return NULL;
}

/**
  This function release registered entry.

  @param[in]  Entry      Instance to be released.

**/
VOID
RedfishTaskFreeRegisteredEntry (
  IN REDFISH_TASK_REGISTERED_ENTRY  *Entry
  )
{
  if (Entry == NULL) {
    return;
  }

  if (Entry->ListenUri != NULL) {
    FreePool (Entry->ListenUri);
  }

  FreePool (Entry);
}

/**
  This function create new registry entry with given values.

  @param[in]  Uri           URI of this registry.
  @param[in]  PartialMatch  TRUE if caller like to do partial match to given URI.
  @param[in]  Callback      Caller's callback handler.
  @param[in]  Context       Caller's context.

  @retval REDFISH_TASK_REGISTERED_ENTRY *  The pointer to newly created registered entry.
  @retval NULL                             Error occurs.
**/
REDFISH_TASK_REGISTERED_ENTRY *
RedfishTaskNewRegisteredEntry (
  IN EFI_STRING             Uri,
  IN BOOLEAN                PartialMatch,
  IN REDFISH_TASK_CALLBACK  Callback,
  IN VOID                   *Context  OPTIONAL
  )
{
  REDFISH_TASK_REGISTERED_ENTRY  *NewEntry;

  NewEntry = AllocateZeroPool (sizeof (REDFISH_TASK_REGISTERED_ENTRY));
  if (NewEntry == NULL) {
    return NULL;
  }

  if (!IS_EMPTY_STRING (Uri)) {
    NewEntry->ListenUri = AllocateCopyPool (StrSize (Uri), Uri);
    if (NewEntry->ListenUri == NULL) {
      goto ON_ERROR;
    }
  }

  NewEntry->Callback     = Callback;
  NewEntry->Context      = Context;
  NewEntry->PartialMatch = PartialMatch;

  return NewEntry;

ON_ERROR:

  RedfishTaskFreeRegisteredEntry (NewEntry);

  return NULL;
}

/**
  This function create new registry entry and add it into list.
  It's call responsibility to release returned entry by calling
  RedfishTaskRemoveRegisteredEntry().

  @param[in]  Private       Pointer to private instance.
  @param[in]  Uri           URI of this registry.
  @param[in]  PartialMatch  TRUE if caller like to do partial match to given URI.
  @param[in]  Callback      Caller's callback handler.
  @param[in]  Context       Caller's context.
  @param[in]  Entry         Newly created registry entry on return.

  @retval EFI_SUCCESS         registry entry is created successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RedfishTaskAddRegisteredEntry (
  IN  REDFISH_TASK_SERVICE_PRIVATE   *Private,
  IN  EFI_STRING                     Uri,
  IN  BOOLEAN                        PartialMatch,
  IN  REDFISH_TASK_CALLBACK          Callback,
  IN  VOID                           *Context,
  OUT REDFISH_TASK_REGISTERED_ENTRY  **Entry OPTIONAL
  )
{
  REDFISH_TASK_REGISTERED_ENTRY  *NewEntry;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Private->RegisteredCount >= REDFISH_TASK_MAX_ENTRY) {
    DEBUG ((DEBUG_ERROR, "%a: maximum of task entry reached %d/%d\n", __func__, Private->RegisteredCount, REDFISH_TASK_MAX_ENTRY));
    return EFI_OUT_OF_RESOURCES;
  }

  NewEntry = RedfishTaskNewRegisteredEntry (Uri, PartialMatch, Callback, Context);
  if (NewEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&Private->RegisteredList, &NewEntry->List);
  Private->RegisteredCount += 1;

  if (Entry != NULL) {
    *Entry = NewEntry;
  }

  return EFI_SUCCESS;
}

/**
  This function remove registry entry from registry list.

  @param[in]  Private        Pointer to private instance.
  @param[in]  Entry          Entry to be removed.

  @retval EFI_SUCCESS        Registry entry is removed successfully.
  @retval Others             Error occurs.
**/
EFI_STATUS
RedfishTaskRemoveRegisteredEntry (
  IN  REDFISH_TASK_SERVICE_PRIVATE   *Private,
  IN  REDFISH_TASK_REGISTERED_ENTRY  *Entry
  )
{
  if ((Private == NULL) || (Entry == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&Entry->List);
  RedfishTaskFreeRegisteredEntry (Entry);
  Private->RegisteredCount -= 1;

  return EFI_SUCCESS;
}

/**
  This function find targe registry entry by given URI.

  @param[in]  Private        Pointer to private instance.
  @param[in]  Uri            The URI to search.
  @param[in]  Context        The context of target registry.

  @retval REDFISH_TASK_REGISTERED_ENTRY *  Registry entry is found and returned.
  @retval NULL                             Registry entry cannot be found.
**/
REDFISH_TASK_REGISTERED_ENTRY *
RedfishTaskFindRegisteredEntry (
  IN REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN EFI_STRING                    Uri,
  IN VOID                          *Context
  )
{
  LIST_ENTRY                     *Node;
  REDFISH_TASK_REGISTERED_ENTRY  *Entry;

  if ((Private == NULL) || IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  if (IsListEmpty (&Private->RegisteredList)) {
    return NULL;
  }

  Node = GetFirstNode (&Private->RegisteredList);
  while (!IsNull (&Private->RegisteredList, Node)) {
    Entry =  REDFISH_TASK_REGISTERED_ENTRY_FROM_LIST (Node);

    if ((StrCmp (Entry->ListenUri, Uri) == 0) && (Entry->Context == Context)) {
      return Entry;
    }

    Node = GetNextNode (&Private->RegisteredList, Node);
  }

  return NULL;
}

/**
  This function find targe registry entry by given URI and return
  callback handler and callback context.

  @param[in]  Private         Pointer to private instance.
  @param[in]  Uri             The URI to search.
  @param[out] CallbackFunc    Pointer to callback handler.
  @param[out] CallbackContext Pointer to callback context.

  @retval EFI_SUCCESS         Callback and context are returned successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RedfishTaskFindCallback (
  IN  REDFISH_TASK_SERVICE_PRIVATE  *Private,
  IN  EFI_STRING                    Uri,
  OUT REDFISH_TASK_CALLBACK         *CallbackFunc,
  OUT VOID                          **CallbackContext
  )
{
  LIST_ENTRY                     *Node;
  REDFISH_TASK_REGISTERED_ENTRY  *Entry;

  if ((Private == NULL) || IS_EMPTY_STRING (Uri) || (CallbackFunc == NULL) || (CallbackContext == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *CallbackFunc    = NULL;
  *CallbackContext = NULL;

  if (IsListEmpty (&Private->RegisteredList)) {
    return EFI_NOT_FOUND;
  }

  Node = GetFirstNode (&Private->RegisteredList);
  while (!IsNull (&Private->RegisteredList, Node)) {
    Entry =  REDFISH_TASK_REGISTERED_ENTRY_FROM_LIST (Node);

    if (Entry->PartialMatch) {
      if (StrStr (Uri, Entry->ListenUri) != NULL) {
        *CallbackFunc    = Entry->Callback;
        *CallbackContext = Entry->Context;
        return EFI_SUCCESS;
      }
    } else {
      if (StrCmp (Entry->ListenUri, Uri) == 0) {
        *CallbackFunc    = Entry->Callback;
        *CallbackContext = Entry->Context;
        return EFI_SUCCESS;
      }
    }

    Node = GetNextNode (&Private->RegisteredList, Node);
  }

  return EFI_NOT_FOUND;
}

/**
  This function converts task state from string format to REDFISH_TASK_STATE.

  @param[in]  TaskState   Task state in ASCII string.

  @retval REDFISH_TASK_STATE   Task state on return.
**/
REDFISH_TASK_STATE
RedfishTaskStateConverter (
  IN CHAR8  *TaskState
  )
{
  if (IS_EMPTY_STRING (TaskState)) {
    return RedfishTaskStateMax;
  }

  if (AsciiStrCmp ("New", TaskState) == 0) {
    return RedfishTaskStateNew;
  } else if (AsciiStrCmp ("Starting", TaskState) == 0) {
    return RedfishTaskStateStarting;
  } else if (AsciiStrCmp ("Running", TaskState) == 0) {
    return RedfishTaskStateRunning;
  } else if (AsciiStrCmp ("Suspended", TaskState) == 0) {
    return RedfishTaskStateSuspended;
  } else if (AsciiStrCmp ("Interrupted", TaskState) == 0) {
    return RedfishTaskStateInterrupted;
  } else if (AsciiStrCmp ("Pending", TaskState) == 0) {
    return RedfishTaskStatePending;
  } else if (AsciiStrCmp ("Stopping", TaskState) == 0) {
    return RedfishTaskStateStopping;
  } else if (AsciiStrCmp ("Completed", TaskState) == 0) {
    return RedfishTaskStateCompleted;
  } else if (AsciiStrCmp ("Killed", TaskState) == 0) {
    return RedfishTaskStateKilled;
  } else if (AsciiStrCmp ("Exception", TaskState) == 0) {
    return RedfishTaskStateException;
  } else if (AsciiStrCmp ("Service", TaskState) == 0) {
    return RedfishTaskStateService;
  } else if (AsciiStrCmp ("Cancelling", TaskState) == 0) {
    return RedfishTaskStateCancelling;
  } else if (AsciiStrCmp ("Cancelled", TaskState) == 0) {
    return RedfishTaskStateCancelled;
  }

  return RedfishTaskStateMax;
}
