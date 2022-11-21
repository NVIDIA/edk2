/** @file

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishETagDxe.h"

REDFISH_ETAG_PRIVATE_DATA  *mRedfishETagPrivate = NULL;

/**
  Release REDFISH_ETAG_RECORD resource

  @param[in]    Record    Pointer to REDFISH_ETAG_RECORD instance

  @retval EFI_SUCCESS             REDFISH_ETAG_RECORD is released successfully.
  @retval EFI_INVALID_PARAMETER   Record is NULL

**/
EFI_STATUS
ReleaseETagRecord (
  IN REDFISH_ETAG_RECORD  *Record
  )
{
  if (Record == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Record->Uri != NULL) {
    FreePool (Record->Uri);
  }

  if (Record->ETag != NULL) {
    FreePool (Record->ETag);
  }

  FreePool (Record);

  return EFI_SUCCESS;
}

/**
  Create new Etag resource.

  @param[in]    Uri   The URI string matching to this ETAG.
  @param[in]    ETag  ETAG string.

  @retval REDFISH_ETAG_RECORD *   Pointer to newly created ETAG.
  @retval NULL                    No memory available.

**/
REDFISH_ETAG_RECORD *
NewETagRecord (
  IN  CHAR8  *Uri,
  IN  CHAR8  *ETag
  )
{
  REDFISH_ETAG_RECORD  *NewRecord;
  UINTN                Size;

  if (IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (ETag)) {
    return NULL;
  }

  NewRecord = AllocateZeroPool (sizeof (REDFISH_ETAG_RECORD));
  if (NewRecord == NULL) {
    return NULL;
  }

  Size           = AsciiStrSize (Uri);
  NewRecord->Uri = AllocateCopyPool (Size, Uri);
  if (NewRecord->Uri == NULL) {
    goto ON_ERROR;
  }

  NewRecord->Size = Size;
  Size            = AsciiStrSize (ETag);
  NewRecord->ETag = AllocateCopyPool (Size, ETag);
  if (NewRecord->ETag == NULL) {
    goto ON_ERROR;
  }

  NewRecord->Size += Size;
  return NewRecord;

ON_ERROR:

  if (NewRecord != NULL) {
    ReleaseETagRecord (NewRecord);
  }

  return NULL;
}

/**
  Add new ETAG by given URI and ETAG string to specify List.

  @param[in]    List  Target ETAG list to add.
  @param[in]    Uri   The URI string matching to this ETAG.
  @param[in]    ETag  ETAG string.

  @retval EFI_SUCCESS   ETAG recourd is added.
  @retval Others        Fail to add ETAG.

**/
EFI_STATUS
AddETagRecord (
  IN  REDFISH_ETAG_LIST  *List,
  IN  CHAR8              *Uri,
  IN  CHAR8              *ETag
  )
{
  REDFISH_ETAG_RECORD  *NewRecord;

  if ((List == NULL) || IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (ETag)) {
    return EFI_INVALID_PARAMETER;
  }

  NewRecord = NewETagRecord (Uri, ETag);
  if (NewETagRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&List->Listheader, &NewRecord->List);
  ++List->Count;
  List->TotalSize += NewRecord->Size;

  return EFI_SUCCESS;
}

/**
  Delete an ETAG by given ETAG instance.

  @param[in]    List    Target ETAG list to be removed.
  @param[in]    Record  Pointer to the instance to be deleted.

  @retval EFI_SUCCESS   ETAG recourd is removed.
  @retval Others        Fail to add ETAG.

**/
EFI_STATUS
DeleteETagRecord (
  IN  REDFISH_ETAG_LIST    *List,
  IN  REDFISH_ETAG_RECORD  *Record
  )
{
  if ((List == NULL) || (Record == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&Record->List);
  --List->Count;
  List->TotalSize -= Record->Size;

  return ReleaseETagRecord (Record);
}

/**
  Search on given ListHeader for given URI string.

  @param[in]    ListHeader  Target list to search.
  @param[in]    Uri         Target URI to search.

  @retval REDFISH_ETAG_RECORD   Target ETAG is found.
  @retval NULL                  No ETAG with given URI is found.

**/
REDFISH_ETAG_RECORD *
FindETagRecord (
  IN  LIST_ENTRY  *ListHeader,
  IN  CHAR8       *Uri
  )
{
  LIST_ENTRY           *List;
  REDFISH_ETAG_RECORD  *Record;

  if (IsListEmpty (ListHeader)) {
    return NULL;
  }

  Record = NULL;
  List   = GetFirstNode (ListHeader);
  while (!IsNull (ListHeader, List)) {
    Record = REDFISH_ETAG_RECORD_FROM_LIST (List);

    if (AsciiStrCmp (Record->Uri, Uri) == 0) {
      return Record;
    }

    List = GetNextNode (ListHeader, List);
  }

  return NULL;
}

#if ETAG_DEBUG_ENABLED

/**
  Debug output the ETAG list.

  @param[in]    ETagList  Target list to dump
  @param[in]    Msg       Debug message string.

  @retval EFI_SUCCESS             Debug dump finished.
  @retval EFI_INVALID_PARAMETER   ETagList is NULL.

**/
EFI_STATUS
DumpETagList (
  IN  REDFISH_ETAG_LIST  *ETagList,
  IN  EFI_STRING         Msg
  )
{
  LIST_ENTRY           *List;
  REDFISH_ETAG_RECORD  *Record;

  if (ETagList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IS_EMPTY_STRING (Msg)) {
    DEBUG ((DEBUG_ERROR, "%s\n", Msg));
  }

  if (IsListEmpty (&ETagList->Listheader)) {
    DEBUG ((DEBUG_INFO, "ETag list is empty\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Count: %d Total Size: %d\n", ETagList->Count, ETagList->TotalSize));
  Record = NULL;
  List   = GetFirstNode (&ETagList->Listheader);
  while (!IsNull (&ETagList->Listheader, List)) {
    Record = REDFISH_ETAG_RECORD_FROM_LIST (List);

    DEBUG ((DEBUG_INFO, "ETag: %a Uri: %a Size: %d\n", Record->ETag, Record->Uri, Record->Size));

    List = GetNextNode (&ETagList->Listheader, List);
  }

  return EFI_SUCCESS;
}

/**
  Debug output raw data buffer.

  @param[in]    Buffer      Debug output data buffer.
  @param[in]    BufferSize  The size of Buffer in byte.

  @retval EFI_SUCCESS             Debug dump finished.
  @retval EFI_INVALID_PARAMETER   Buffer is NULL.

**/
EFI_STATUS
DumpRawBuffer (
  IN  CHAR8  *Buffer,
  IN  UINTN  BufferSize
  )
{
  UINTN  Index;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Index = 0;

  DEBUG ((DEBUG_ERROR, "Buffer size: %d\n", BufferSize));
  while (Buffer[Index] != '\0') {
    DEBUG ((DEBUG_ERROR, "(%d) %c ", (Index + 1), Buffer[Index]));

    ++Index;
  }

  DEBUG ((DEBUG_ERROR, "\n"));

  return EFI_SUCCESS;
}

#endif

/**
  Release all ETag from list.

  @param[in]    ETagList    The list to be released.

  @retval EFI_SUCCESS             All etag is released.
  @retval EFI_INVALID_PARAMETER   ETagList is NULL.

**/
EFI_STATUS
ReleaseETagList (
  IN  REDFISH_ETAG_LIST  *ETagList
  )
{
  LIST_ENTRY           *List;
  LIST_ENTRY           *Next;
  REDFISH_ETAG_RECORD  *Record;

  if (ETagList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (&ETagList->Listheader)) {
    return EFI_SUCCESS;
  }

  Record = NULL;
  Next   = NULL;
  List   = GetFirstNode (&ETagList->Listheader);
  while (!IsNull (&ETagList->Listheader, List)) {
    Record = REDFISH_ETAG_RECORD_FROM_LIST (List);
    Next   = GetNextNode (&ETagList->Listheader, List);

    DeleteETagRecord (ETagList, Record);

    List = Next;
  }

  return EFI_SUCCESS;
}

/**
  Save etag in list to UEFI variable.

  @param[in]    ETagList      The list to be saved.
  @param[in]    VariableName  The UEFI variable name.

  @retval EFI_SUCCESS             All etag is saved.
  @retval EFI_INVALID_PARAMETER   VariableName or ETagList is NULL.

**/
EFI_STATUS
SaveETagList (
  IN  REDFISH_ETAG_LIST  *ETagList,
  IN  EFI_STRING         VariableName
  )
{
  LIST_ENTRY           *List;
  REDFISH_ETAG_RECORD  *Record;
  CHAR8                *VarData;
  VOID                 *Data;
  CHAR8                *Seeker;
  UINTN                VarSize;
  UINTN                StrSize;
  EFI_STATUS           Status;

  if ((ETagList == NULL) || IS_EMPTY_STRING (VariableName)) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (&ETagList->Listheader)) {
    return EFI_SUCCESS;
  }

  //
  // Caculate the total size we need to keep ETag list.
  //
  VarSize = ETagList->TotalSize + 1; // terminator character
  VarData = AllocateZeroPool (VarSize);
  if (VarData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Seeker = VarData;
  Record = NULL;
  List   = GetFirstNode (&ETagList->Listheader);
  while (!IsNull (&ETagList->Listheader, List)) {
    Record = REDFISH_ETAG_RECORD_FROM_LIST (List);

    StrSize = AsciiStrSize (Record->Uri);
    CopyMem (Seeker, Record->Uri, StrSize);

    Seeker += (StrSize - 1);
    *Seeker = '|';
    ++Seeker;

    StrSize = AsciiStrSize (Record->ETag);
    CopyMem (Seeker, Record->ETag, StrSize);

    Seeker += (StrSize - 1);
    *Seeker = '\n';

    ++Seeker;

    List = GetNextNode (&ETagList->Listheader, List);
  }

  *Seeker = '\0';

 #if ETAG_DEBUG_ENABLED
  DumpRawBuffer (VarData, VarSize);
 #endif

  ASSERT (((UINTN)Seeker - (UINTN)VarData + 1) == VarSize);

  //
  // Check if variable exists already. If yes, remove it first.
  //
  Status = GetVariable2 (
             VariableName,
             &gEfiRedfishClientVariableGuid,
             (VOID *)&Data,
             NULL
             );
  if (!EFI_ERROR (Status)) {
    FreePool (Data);
    gRT->SetVariable (VariableName, &gEfiRedfishClientVariableGuid, VARIABLE_ATTRIBUTE_NV_BS, 0, NULL);
  }

  return gRT->SetVariable (VariableName, &gEfiRedfishClientVariableGuid, VARIABLE_ATTRIBUTE_NV_BS, VarSize, (VOID *)VarData);
}

/**
  Read etag from UEFI variable if it exists.

  @param[in]    ETagList      The list to be loaded.
  @param[in]    VariableName  The UEFI variable name.

  @retval EFI_SUCCESS             All etag is read successfully.
  @retval EFI_INVALID_PARAMETER   VariableName or ETagList is NULL.
  @retval EFI_NOT_FOUND           No etag is found on UEFI variable.

**/
EFI_STATUS
InitialETagList (
  IN  REDFISH_ETAG_LIST  *ETagList,
  IN  EFI_STRING         VariableName
  )
{
  CHAR8       *VarData;
  CHAR8       *UriPointer;
  CHAR8       *ETagPointer;
  CHAR8       *Seeker;
  UINTN       VariableSize;
  EFI_STATUS  Status;

  if ((ETagList == NULL) || IS_EMPTY_STRING (VariableName)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if variable exists already.
  //
  Status = GetVariable2 (
             VariableName,
             &gEfiRedfishClientVariableGuid,
             (VOID *)&VarData,
             &VariableSize
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Seeker      = VarData;
  UriPointer  = VarData;
  ETagPointer = VarData;
  while (*Seeker != '\0') {
    //
    // Find URI
    //
    Seeker = AsciiStrStr (UriPointer, "|");
    if (Seeker == NULL) {
      DEBUG ((DEBUG_ERROR, "%a, data corrupted\n", __FUNCTION__));
      Status = EFI_DEVICE_ERROR;
      goto ON_ERROR;
    }

    *Seeker     = '\0';
    ETagPointer = ++Seeker;

    //
    // Find ETAG
    //
    Seeker = AsciiStrStr (ETagPointer, "\n");
    if (Seeker == NULL) {
      DEBUG ((DEBUG_ERROR, "%a, data corrupted\n", __FUNCTION__));
      Status = EFI_DEVICE_ERROR;
      goto ON_ERROR;
    }

    *Seeker = '\0';

    AddETagRecord (ETagList, UriPointer, ETagPointer);

    UriPointer = ++Seeker;
  }

 #if ETAG_DEBUG_ENABLED
  DumpETagList (ETagList, L"Initial ETag List from Variable");
 #endif

  Status = EFI_SUCCESS;

ON_ERROR:

  FreePool (VarData);

  return Status;
}

/**
  Get ETag string by given URI

  @param[in]   This                    Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.
  @param[in]   Uri                     Target URI to search.
  @param[out]  ETag                    The ETag string to corresponding URI.

  @retval EFI_SUCCESS                  The ETag is found successfully.
  @retval EFI_INVALID_PARAMETER        Invalid parameter is given.

**/
EFI_STATUS
RedfishETagGet (
  IN  EDKII_REDFISH_ETAG_PROTOCOL  *This,
  IN  CHAR8                        *Uri,
  OUT CHAR8                        **ETag
  )
{
  REDFISH_ETAG_RECORD        *Target;
  REDFISH_ETAG_PRIVATE_DATA  *Private;

  if ((This == NULL) || IS_EMPTY_STRING (Uri) || (ETag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_ETAG_PRIVATE_FROM_THIS (This);

  *ETag = NULL;

  Target = FindETagRecord (&Private->ETagList.Listheader, Uri);
  if (Target == NULL) {
    return EFI_NOT_FOUND;
  }

  *ETag = AllocateCopyPool (AsciiStrSize (Target->ETag), Target->ETag);

  return EFI_SUCCESS;
}

/**
  Keep ETag string which maps to given Uri.

  @param[in]   This                Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.
  @param[in]   Uri                 The target Uri which related to ETag.
  @param[in]   ETag                The ETag to add. If ETag is NULL, the record of correspoonding URI will be removed.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishETagSet (
  IN  EDKII_REDFISH_ETAG_PROTOCOL  *This,
  IN  CHAR8                        *Uri,
  IN  CHAR8                        *ETag  OPTIONAL
  )
{
  REDFISH_ETAG_RECORD        *Target;
  REDFISH_ETAG_PRIVATE_DATA  *Private;
  EFI_STATUS                 Status;

  if ((This == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_ETAG_PRIVATE_FROM_THIS (This);

  Status = EFI_NOT_FOUND;
  Target = FindETagRecord (&Private->ETagList.Listheader, Uri);
  if (Target != NULL) {
    //
    // Remove old one and create new one.
    //
    Status = DeleteETagRecord (&Private->ETagList, Target);
  }

  //
  // When ETag is NULL, it means that we want to remov this record.
  //
  if (ETag == NULL) {
    return Status;
  }

  return AddETagRecord (&Private->ETagList, Uri, ETag);
}

/**
  Refresh the ETag database and save database to variable.

  @param[in]   This                Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishETagFlush (
  IN  EDKII_REDFISH_ETAG_PROTOCOL  *This
  )
{
  REDFISH_ETAG_PRIVATE_DATA  *Private;
  EFI_STATUS                 Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_ETAG_PRIVATE_FROM_THIS (This);

  Status = SaveETagList (&Private->ETagList, Private->VariableName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, save ETag list to variable: %s failed: %r\n", __FUNCTION__, Private->VariableName, Status));
  }

  return Status;
}

/**
  Callback function executed when the ExitBootService event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[out]  Context  Pointer to the Context buffer

**/
VOID
EFIAPI
RedfishETagOnExitBootService (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  //
  // Memory is about to be released. Keep list into variable.
  //
  RedfishETagFlush (&mRedfishETagPrivate->Protocol);
}

/**
  Unloads an image.

  @param[in]  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishETagDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;

  if (mRedfishETagPrivate != NULL) {
    Status = gBS->UninstallProtocolInterface (
                    mRedfishETagPrivate->ImageHandle,
                    &gEdkIIRedfishETagProtocolGuid,
                    (VOID *)&mRedfishETagPrivate->Protocol
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a, can not uninstall gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
      ASSERT (FALSE);
    }

    ReleaseETagList (&mRedfishETagPrivate->ETagList);

    if (mRedfishETagPrivate->VariableName != NULL) {
      FreePool (mRedfishETagPrivate->VariableName);
    }

    if (mRedfishETagPrivate->Event != NULL) {
      gBS->CloseEvent (mRedfishETagPrivate->Event);
    }

    FreePool (mRedfishETagPrivate);
    mRedfishETagPrivate = NULL;
  }

  return EFI_SUCCESS;
}

//
// EDKII_REDFISH_ETAG_PROTOCOL.
//
EDKII_REDFISH_ETAG_PROTOCOL  mRedfishETagProtocol = {
  RedfishETagGet,
  RedfishETagSet,
  RedfishETagFlush
};

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
RedfishETagDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  mRedfishETagPrivate = AllocateZeroPool (sizeof (REDFISH_ETAG_PRIVATE_DATA));
  if (mRedfishETagPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InitializeListHead (&mRedfishETagPrivate->ETagList.Listheader);
  mRedfishETagPrivate->VariableName = AllocateCopyPool (StrSize (ETAG_VARIABLE_NAME), ETAG_VARIABLE_NAME);
  if (mRedfishETagPrivate->VariableName == NULL) {
    goto ON_ERROR;
  }

  mRedfishETagPrivate->ImageHandle = ImageHandle;
  CopyMem (&mRedfishETagPrivate->Protocol, &mRedfishETagProtocol, sizeof (EDKII_REDFISH_ETAG_PROTOCOL));

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkIIRedfishETagProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *)&mRedfishETagPrivate->Protocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, can not install gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
    ASSERT (FALSE);
    goto ON_ERROR;
  }

  //
  // Create Exit Boot Service event.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RedfishETagOnExitBootService,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mRedfishETagPrivate->Event
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to register Exit Boot Service event.", __FUNCTION__));
    goto ON_ERROR;
  }

  //
  // Read existing record from variable.
  //
  Status = InitialETagList (&mRedfishETagPrivate->ETagList, mRedfishETagPrivate->VariableName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%a, Initial ETag List: %r\n", __FUNCTION__, Status));
  }

  return EFI_SUCCESS;

ON_ERROR:

  RedfishETagDriverUnload (ImageHandle);

  return Status;
}
