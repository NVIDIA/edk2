/** @file
  Redfish debug library to debug Redfish application.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishMessageInternal.h"

REDFISH_MESSAGE_PRIVATE  *mRedfishMessagePrivate = NULL;

/**
  This function release message record.

  @param[in]    Record   Message record to release.

**/
VOID
RfMessageFreeRecord (
  IN REDFISH_MESSAGE_RECORD  *Record
  )
{
  if (Record == NULL) {
    return;
  }

  if (Record->Message != NULL) {
    FreePool (Record->Message);
  }

  if (Record->Resolution != NULL) {
    FreePool (Record->Resolution);
  }

  FreePool (Record);
}

/**
  This function create new message record instance.

  @param[in]  Message          Message string of this record.
  @param[in]  Resolution       Resolution string of this record.
  @param[in]  MessageSeverity  Message severity of this record.

  @retval REDFISH_MESSAGE_RECORD *  Pointer to newly created instance.
  @retval NULL                      Error occurs.
**/
REDFISH_MESSAGE_RECORD *
RfMessageNewRecord (
  IN CHAR8                     *Message,
  IN CHAR8                     *Resolution,
  IN REDFISH_MESSAGE_SEVERITY  MessageSeverity
  )
{
  REDFISH_MESSAGE_RECORD  *NewRecord;

  if (MessageSeverity >= RedfishMessageSeverityMax) {
    return NULL;
  }

  NewRecord = AllocateZeroPool (sizeof (REDFISH_MESSAGE_RECORD));
  if (NewRecord == NULL) {
    return NULL;
  }

  NewRecord->MessageSeverity = MessageSeverity;
  if (!IS_EMPTY_STRING (Message)) {
    NewRecord->Message = AllocateCopyPool (AsciiStrSize (Message), Message);
    if ( NewRecord->Message == NULL) {
      goto ON_ERROR;
    }
  }

  if (!IS_EMPTY_STRING (Resolution)) {
    NewRecord->Resolution = AllocateCopyPool (AsciiStrSize (Resolution), Resolution);
    if ( NewRecord->Resolution == NULL) {
      goto ON_ERROR;
    }
  }

  return NewRecord;

ON_ERROR:

  RfMessageFreeRecord (NewRecord);

  return NULL;
}

/**
  This function remove target record from list.

  @param[in]  Entry           Pointer to message entry instance.
  @param[in]  RecordToRemove  Target record to remove.

  @retval EFI_SUCCESS         Target record is removed from list successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfMessageRemoveRecord (
  IN REDFISH_MESSAGE_ENTRY   *Entry,
  IN REDFISH_MESSAGE_RECORD  *RecordToRemove
  )
{
  if ((Entry == NULL) || (RecordToRemove == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&RecordToRemove->List);
  RfMessageFreeRecord (RecordToRemove);
  Entry->Count -= 1;

  return EFI_SUCCESS;
}

/**
  This function create new record and add it to list.

  @param[in]  Entry            Pointer to message entry instance.
  @param[in]  Message          Message string of this record.
  @param[in]  Resolution       Resolution string of this record.
  @param[in]  MessageSeverity  Message severity of this record.

  @retval EFI_SUCCESS         New record is created and add to list successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfMessageAddRecord (
  IN REDFISH_MESSAGE_ENTRY     *Entry,
  IN CHAR8                     *Message,
  IN CHAR8                     *Resolution OPTIONAL,
  IN REDFISH_MESSAGE_SEVERITY  MessageSeverity
  )
{
  REDFISH_MESSAGE_RECORD  *NewRecord;

  if (Entry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Entry->Count > REDFISH_MESSAGE_MAX) {
    DEBUG ((DEBUG_ERROR, "%a: limitation reached: %d/%d\n", __func__, Entry->Count, REDFISH_MESSAGE_MAX));
    return EFI_OUT_OF_RESOURCES;
  }

  NewRecord = RfMessageNewRecord (Message, Resolution, MessageSeverity);
  if (NewRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&Entry->MessageHead, &NewRecord->List);
  Entry->Count += 1;

  return EFI_SUCCESS;
}

/**
  This function release message entry.

  @param[in]    Record   Message entry to release.

**/
VOID
RfMessageFreeEntry (
  IN REDFISH_MESSAGE_ENTRY  *Entry
  )
{
  LIST_ENTRY              *Node;
  LIST_ENTRY              *NextNode;
  REDFISH_MESSAGE_RECORD  *Record;

  if (Entry == NULL) {
    return;
  }

  if (!IsListEmpty (&Entry->MessageHead)) {
    Node = GetFirstNode (&Entry->MessageHead);
    while (!IsNull (&Entry->MessageHead, Node)) {
      NextNode = GetNextNode (&Entry->MessageHead, Node);
      Record   = REDFISH_MESSAGE_RECORD_FROM_LIST (Node);
      RfMessageRemoveRecord (Entry, Record);
      Node = NextNode;
    }
  }

  if (Entry->Uri != NULL) {
    FreePool (Entry->Uri);
  }

  FreePool (Entry);
}

/**
  This function create new message entry instance.

  @param[in]  Uri  URI of this message entry.

  @retval REDFISH_MESSAGE_ENTRY *  Pointer to newly created instance.
  @retval NULL                     Error occurs.
**/
REDFISH_MESSAGE_ENTRY *
RfMessageNewEntry (
  IN EFI_STRING  Uri
  )
{
  REDFISH_MESSAGE_ENTRY  *NewEntry;

  NewEntry = AllocateZeroPool (sizeof (REDFISH_MESSAGE_ENTRY));
  if (NewEntry == NULL) {
    return NULL;
  }

  if (!IS_EMPTY_STRING (Uri)) {
    NewEntry->Uri = AllocateCopyPool (StrSize (Uri), Uri);
    if ( NewEntry->Uri == NULL) {
      goto ON_ERROR;
    }
  }

  InitializeListHead (&NewEntry->MessageHead);

  return NewEntry;

ON_ERROR:

  RfMessageFreeEntry (NewEntry);

  return NULL;
}

/**
  This function remove target entry from list.

  @param[in]  Private         Pointer to private instance.
  @param[in]  EntryToRemove   Target entry to remove.

  @retval EFI_SUCCESS         Target entry is removed from list successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfMessageRemoveEntry (
  IN REDFISH_MESSAGE_PRIVATE  *Private,
  IN REDFISH_MESSAGE_ENTRY    *EntryToRemove
  )
{
  if ((Private == NULL) || (EntryToRemove == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&EntryToRemove->List);
  RfMessageFreeEntry (EntryToRemove);
  Private->Count -= 1;

  return EFI_SUCCESS;
}

/**
  This function create new entry and add it to list. It's call responsibility to
  release ReturnEntry by calling RfMessageRemoveEntry().

  @param[in]  Private      Pointer to private instance.
  @param[in]  Uri          URI of this message entry.
  @param[out] ReturnEntry  Pointer of newly created entry.

  @retval EFI_SUCCESS         New record is created and add to list successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfMessageAddEntry (
  IN REDFISH_MESSAGE_PRIVATE  *Private,
  IN EFI_STRING               Uri,
  OUT REDFISH_MESSAGE_ENTRY   **ReturnEntry  OPTIONAL
  )
{
  REDFISH_MESSAGE_ENTRY  *NewEntry;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Private->Count > REDFISH_MESSAGE_MAX) {
    DEBUG ((DEBUG_ERROR, "%a: limitation reached: %d/%d\n", __func__, Private->Count, REDFISH_MESSAGE_MAX));
    return EFI_OUT_OF_RESOURCES;
  }

  NewEntry = RfMessageNewEntry (Uri);
  if (NewEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&Private->ListHead, &NewEntry->List);
  Private->Count += 1;

  if (ReturnEntry != NULL) {
    *ReturnEntry = NewEntry;
  }

  return EFI_SUCCESS;
}

/**
  This function search target message entry by given Uri in list.

  @param[in]  Private      Pointer to private instance.
  @param[in]  Uri          URI of message entry to search.

  @retval REDFISH_MESSAGE_ENTRY *    Target message entry is found and returned.
  @retval NULL                       Target message entry cannot be found.
**/
REDFISH_MESSAGE_ENTRY *
RfMessageFindEntry (
  IN REDFISH_MESSAGE_PRIVATE  *Private,
  IN EFI_STRING               Uri
  )
{
  LIST_ENTRY             *Node;
  REDFISH_MESSAGE_ENTRY  *Entry;

  if ((Private == NULL) || IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  if (IsListEmpty (&Private->ListHead)) {
    return NULL;
  }

  Node = GetFirstNode (&Private->ListHead);
  while (!IsNull (&Private->ListHead, Node)) {
    Entry = REDFISH_MESSAGE_ENTRY_FROM_LIST (Node);
    if (StrCmp (Entry->Uri, Uri) == 0) {
      return Entry;
    }

    Node = GetNextNode (&Private->ListHead, Node);
  }

  return NULL;
}

/**
  This function release private instance.

  @param[in]  Private      Pointer to private instance.

**/
VOID
RfMessageFreePrivate (
  IN REDFISH_MESSAGE_PRIVATE  *Private
  )
{
  LIST_ENTRY             *Node;
  LIST_ENTRY             *NextNode;
  REDFISH_MESSAGE_ENTRY  *Entry;

  if (Private == NULL) {
    return;
  }

  if (!IsListEmpty (&Private->ListHead)) {
    Node = GetFirstNode (&Private->ListHead);
    while (!IsNull (&Private->ListHead, Node)) {
      NextNode = GetNextNode (&Private->ListHead, Node);
      Entry    = REDFISH_MESSAGE_ENTRY_FROM_LIST (Node);
      RfMessageRemoveEntry (Private, Entry);
      Node = NextNode;
    }
  }

  FreePool (Private);
}

/**
  This function create new private instance.

  @retval REDFISH_MESSAGE_PRIVATE *  Pointer to newly created instance.
  @retval NULL                       Error occurs.
**/
REDFISH_MESSAGE_PRIVATE *
RfMessageNewPrivate (
  VOID
  )
{
  REDFISH_MESSAGE_PRIVATE  *NewInstance;

  NewInstance = AllocateZeroPool (sizeof (REDFISH_MESSAGE_PRIVATE));
  if (NewInstance == NULL) {
    return NULL;
  }

  InitializeListHead (&NewInstance->ListHead);

  return NewInstance;
}

/**
  Add Redfish message to given resource URI.

  @param[in]    ResourceUri       Report Redfish message to this resource URI.
  @param[in]    Message           String message.
  @param[in]    Resolution        Resolution message. This is optional.
  @param[in]    MessageSeverity   Message severity.

  @retval EFI_SUCCESS             Redfish message is reported.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageReport (
  IN EFI_STRING                ResourceUri,
  IN CHAR8                     *Message,
  IN CHAR8                     *Resolution OPTIONAL,
  IN REDFISH_MESSAGE_SEVERITY  MessageSeverity
  )
{
  EFI_STATUS             Status;
  REDFISH_MESSAGE_ENTRY  *TargetEntry;

  if (mRedfishMessagePrivate == NULL) {
    return EFI_NOT_READY;
  }

  if (IS_EMPTY_STRING (ResourceUri) || IS_EMPTY_STRING (Message)) {
    return EFI_INVALID_PARAMETER;
  }

  TargetEntry = RfMessageFindEntry (mRedfishMessagePrivate, ResourceUri);
  if (TargetEntry == NULL) {
    Status = RfMessageAddEntry (mRedfishMessagePrivate, ResourceUri, &TargetEntry);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: can not add entry: %s: %r\n", __func__, ResourceUri, Status));
      return Status;
    }
  }

  Status = RfMessageAddRecord (TargetEntry, Message, Resolution, MessageSeverity);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot add record: %s:%r\n", __func__, ResourceUri, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Get all messages reported to given URI. It's caller's responsibility to release
  MessageArray by calling RedfishMessageFree().

  @param[in]    TargetUri       Report Redfish message to this resource URI.
  @param[out]   MessageArray    The array of REDFISH_MESSAGE_DATA.
  @param[out]   MessageCount    The number of REDFISH_MESSAGE_DATA in MessageArray.

  @retval EFI_SUCCESS             Redfish message is reported.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageGet (
  IN  EFI_STRING            TargetUri,
  OUT REDFISH_MESSAGE_DATA  **MessageArray,
  OUT UINTN                 *MessageCount
  )
{
  REDFISH_MESSAGE_ENTRY   *TargetEntry;
  REDFISH_MESSAGE_DATA    *Array;
  UINTN                   Index;
  LIST_ENTRY              *Node;
  REDFISH_MESSAGE_RECORD  *Record;

  if (mRedfishMessagePrivate == NULL) {
    return EFI_NOT_READY;
  }

  if (IS_EMPTY_STRING (TargetUri) || (MessageArray == NULL) || (MessageCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Index         = 0;
  *MessageCount = 0;
  *MessageArray = NULL;
  TargetEntry   = RfMessageFindEntry (mRedfishMessagePrivate, TargetUri);
  if (TargetEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  if (IsListEmpty (&TargetEntry->MessageHead)) {
    return EFI_NOT_FOUND;
  }

  Array = AllocateZeroPool (sizeof (REDFISH_MESSAGE_DATA) * TargetEntry->Count);
  if (Array == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Node = GetFirstNode (&TargetEntry->MessageHead);
  while (!IsNull (&TargetEntry->MessageHead, Node)) {
    Record = REDFISH_MESSAGE_RECORD_FROM_LIST (Node);

    if (!IS_EMPTY_STRING (Record->Message)) {
      Array[Index].Message = AllocateCopyPool (AsciiStrSize (Record->Message), Record->Message);
      ASSERT (Array[Index].Message != NULL);
    }

    if (!IS_EMPTY_STRING (Record->Resolution)) {
      Array[Index].Resolution = AllocateCopyPool (AsciiStrSize (Record->Resolution), Record->Resolution);
      ASSERT (Array[Index].Resolution != NULL);
    }

    Array[Index].MessageSeverity = Record->MessageSeverity;

    Index += 1;
    Node   = GetNextNode (&TargetEntry->MessageHead, Node);
  }

  *MessageArray = Array;
  *MessageCount = TargetEntry->Count;

  return EFI_SUCCESS;
}

/**
  Release MessageArray.

  @param[in]   MessageArray    The array of REDFISH_MESSAGE_DATA.
  @param[in]   MessageCount    The number of REDFISH_MESSAGE_DATA in MessageArray.

  @retval EFI_SUCCESS             Redfish message is released.
  @retval Others                  Error occurs.

**/
EFI_STATUS
RedfishMessageFree (
  IN REDFISH_MESSAGE_DATA  *MessageArray,
  IN UINTN                 MessageCount
  )
{
  UINTN  Index;

  if ((MessageArray == NULL) || (MessageCount == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < MessageCount; Index++) {
    if (MessageArray[Index].Message != NULL) {
      FreePool (MessageArray[Index].Message);
    }

    if (MessageArray[Index].Resolution != NULL) {
      FreePool (MessageArray[Index].Resolution);
    }
  }

  FreePool (MessageArray);

  return EFI_SUCCESS;
}

/**

  Initial Redfish message library instance.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCCESS  Initial library successfully.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishMessageConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mRedfishMessagePrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mRedfishMessagePrivate = RfMessageNewPrivate ();
  if (mRedfishMessagePrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

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
RedfishMessageDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mRedfishMessagePrivate == NULL) {
    return EFI_SUCCESS;
  }

  RfMessageFreePrivate (mRedfishMessagePrivate);
  mRedfishMessagePrivate = NULL;

  return EFI_SUCCESS;
}
