/** @file
  Definitions of RedfishTaskServiceData

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_TASK_DATA_H_
#define EDKII_REDFISH_TASK_DATA_H_

#include <Base.h>
#include <RedfishBase.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Protocol/EdkIIRedfishTaskProtocol.h>

#include "RedfishTaskServiceDxe.h"

#define REDFISH_TASK_MAX_ENTRY  0xFF

///
/// Definition of REDFISH_TASK_REGISTERED_ENTRY
//
typedef struct {
  LIST_ENTRY             List;
  UINT16                 TaskId;
  EFI_STRING             TaskUri;
  CHAR8                  *JsonText;
  REDFISH_TASK_RESULT    Result;
} REDFISH_TASK_ENTRY;

#define REDFISH_TASK_ENTRY_FROM_LIST(a)  BASE_CR (a, REDFISH_TASK_ENTRY, List)

///
/// Definition of REDFISH_TASK_REGISTERED_ENTRY
//
typedef struct {
  LIST_ENTRY               List;
  EFI_STRING               ListenUri;
  BOOLEAN                  PartialMatch;
  REDFISH_TASK_CALLBACK    Callback;
  VOID                     *Context;
} REDFISH_TASK_REGISTERED_ENTRY;

#define REDFISH_TASK_REGISTERED_ENTRY_FROM_LIST(a)  BASE_CR (a, REDFISH_TASK_REGISTERED_ENTRY, List)

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  This function converts task state from string format to REDFISH_TASK_STATE.

  @param[in]  TaskState   Task state in ASCII string.

  @retval REDFISH_TASK_STATE   Task state on return.
**/
REDFISH_TASK_STATE
RedfishTaskStateConverter (
  IN CHAR8  *TaskState
  );

#endif
