/** @file

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishConfigLangMapDxe.h"

REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA  *mRedfishConfigLangMapPrivate = NULL;

/**
  Release REDFISH_CONFIG_LANG_MAP_RECORD resource

  @param[in]    Record    Pointer to REDFISH_CONFIG_LANG_MAP_RECORD instance

  @retval EFI_SUCCESS             REDFISH_CONFIG_LANG_MAP_RECORD is released successfully.
  @retval EFI_INVALID_PARAMETER   Record is NULL

**/
EFI_STATUS
ReleaseConfigLangMapRecord (
  IN REDFISH_CONFIG_LANG_MAP_RECORD  *Record
  )
{
  if (Record == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Record->Uri != NULL) {
    FreePool (Record->Uri);
  }

  if (Record->ConfigLang != NULL) {
    FreePool (Record->ConfigLang);
  }

  FreePool (Record);

  return EFI_SUCCESS;
}

/**
  Create new resource map resource.

  @param[in]    Uri         The URI string matching to this ConfigLang.
  @param[in]    ConfigLang  ConfigLang string.

  @retval REDFISH_CONFIG_LANG_MAP_RECORD *  Pointer to newly created config language map.
  @retval NULL                              No memory available.

**/
REDFISH_CONFIG_LANG_MAP_RECORD *
NewConfigLangMapRecord (
  IN  EFI_STRING  Uri,
  IN  EFI_STRING  ConfigLang
  )
{
  REDFISH_CONFIG_LANG_MAP_RECORD  *NewRecord;
  UINTN                           Size;

  if (IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (ConfigLang)) {
    return NULL;
  }

  NewRecord = AllocateZeroPool (sizeof (REDFISH_CONFIG_LANG_MAP_RECORD));
  if (NewRecord == NULL) {
    return NULL;
  }

  Size           = StrSize (Uri);
  NewRecord->Uri = AllocateCopyPool (Size, Uri);
  if (NewRecord->Uri == NULL) {
    goto ON_ERROR;
  }

  NewRecord->Size       = Size;
  Size                  = StrSize (ConfigLang);
  NewRecord->ConfigLang = AllocateCopyPool (Size, ConfigLang);
  if (NewRecord->ConfigLang == NULL) {
    goto ON_ERROR;
  }

  NewRecord->Size += Size;
  return NewRecord;

ON_ERROR:

  if (NewRecord != NULL) {
    ReleaseConfigLangMapRecord (NewRecord);
  }

  return NULL;
}

/**
  Add new config language map by given URI and ConfigLang string to specify List.

  @param[in]    List        Target config language map list to add.
  @param[in]    Uri         The URI string matching to this ConfigLang.
  @param[in]    ConfigLang  ConfigLang string.

  @retval EFI_SUCCESS   config language map record is added.
  @retval Others        Fail to add config language map.

**/
EFI_STATUS
AddConfigLangMapRecord (
  IN  REDFISH_CONFIG_LANG_MAP_LIST  *List,
  IN  EFI_STRING                    Uri,
  IN  EFI_STRING                    ConfigLang
  )
{
  REDFISH_CONFIG_LANG_MAP_RECORD  *NewRecord;

  if ((List == NULL) || IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (ConfigLang)) {
    return EFI_INVALID_PARAMETER;
  }

  NewRecord = NewConfigLangMapRecord (Uri, ConfigLang);
  if (NewConfigLangMapRecord == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&List->Listheader, &NewRecord->List);
  ++List->Count;
  List->TotalSize += NewRecord->Size;

  return EFI_SUCCESS;
}

/**
  Delete an config language map by given config language map instance.

  @param[in]    List    Target config language map list to be removed.
  @param[in]    Record  Pointer to the instance to be deleted.

  @retval EFI_SUCCESS   config language map record is removed.
  @retval Others        Fail to add config language map.

**/
EFI_STATUS
DeleteConfigLangMapRecord (
  IN  REDFISH_CONFIG_LANG_MAP_LIST    *List,
  IN  REDFISH_CONFIG_LANG_MAP_RECORD  *Record
  )
{
  if ((List == NULL) || (Record == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&Record->List);
  --List->Count;
  List->TotalSize -= Record->Size;

  return ReleaseConfigLangMapRecord (Record);
}

/**
  Search on given ListHeader for given ConfigLang string.

  @param[in]    ListHeader  Target list to search.
  @param[in]    Query       Target string to search.
  @param[in]    QueryIsUri  Query string is URI string or not

  @retval REDFISH_CONFIG_LANG_MAP_RECORD  Target in map is found.
  @retval NULL                            No target in map with given query is found.

**/
REDFISH_CONFIG_LANG_MAP_RECORD *
FindConfigLangMapRecord (
  IN  LIST_ENTRY  *ListHeader,
  IN  EFI_STRING  Query,
  IN  BOOLEAN     QueryIsUri
  )
{
  LIST_ENTRY                      *List;
  REDFISH_CONFIG_LANG_MAP_RECORD  *Record;

  if (IsListEmpty (ListHeader)) {
    return NULL;
  }

  if (IS_EMPTY_STRING (Query)) {
    return NULL;
  }

  Record = NULL;
  List   = GetFirstNode (ListHeader);
  while (!IsNull (ListHeader, List)) {
    Record = REDFISH_CONFIG_LANG_MAP_RECORD_FROM_LIST (List);

    if (QueryIsUri) {
      if (StrCmp (Record->Uri, Query) == 0) {
        return Record;
      }
    } else {
      if (StrCmp (Record->ConfigLang, Query) == 0) {
        return Record;
      }
    }

    List = GetNextNode (ListHeader, List);
  }

  return NULL;
}

#if CONFIG_LANG_MAP_DEBUG_ENABLED

/**
  Debug output the config language map list.

  @param[in]    ConfigLangMapList  Target list to dump
  @param[in]    Msg       Debug message string.

  @retval EFI_SUCCESS             Debug dump finished.
  @retval EFI_INVALID_PARAMETER   ConfigLangMapList is NULL.

**/
EFI_STATUS
DumpConfigLangMapList (
  IN  REDFISH_CONFIG_LANG_MAP_LIST  *ConfigLangMapList,
  IN  EFI_STRING                    Msg
  )
{
  LIST_ENTRY                      *List;
  REDFISH_CONFIG_LANG_MAP_RECORD  *Record;

  if (ConfigLangMapList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IS_EMPTY_STRING (Msg)) {
    DEBUG ((CONFIG_LANG_MAP_TRACE, "%s\n", Msg));
  }

  if (IsListEmpty (&ConfigLangMapList->Listheader)) {
    DEBUG ((CONFIG_LANG_MAP_TRACE, "ConfigLangMap list is empty\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((CONFIG_LANG_MAP_TRACE, "Count: %d Total Size: %d\n", ConfigLangMapList->Count, ConfigLangMapList->TotalSize));
  Record = NULL;
  List   = GetFirstNode (&ConfigLangMapList->Listheader);
  while (!IsNull (&ConfigLangMapList->Listheader, List)) {
    Record = REDFISH_CONFIG_LANG_MAP_RECORD_FROM_LIST (List);

    DEBUG ((CONFIG_LANG_MAP_TRACE, "ConfigLang: %s Uri: %s Size: %d\n", Record->ConfigLang, Record->Uri, Record->Size));

    List = GetNextNode (&ConfigLangMapList->Listheader, List);
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
  IN  UINT8  *Buffer,
  IN  UINTN  BufferSize
  )
{
  UINTN   Index;
  CHAR16  *Seeker;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Index  = 0;
  Seeker = (CHAR16 *)Buffer;
  DEBUG ((DEBUG_ERROR, "Buffer size: %d\n", BufferSize));
  while (Seeker[Index] != '\0') {
    DEBUG ((DEBUG_ERROR, "(%d) %c ", (Index + 1), Seeker[Index]));

    ++Index;
  }

  DEBUG ((DEBUG_ERROR, "\n"));

  return EFI_SUCCESS;
}

#endif

/**
  Release all ConfigLangMap from list.

  @param[in]    ConfigLangMapList    The list to be released.

  @retval EFI_SUCCESS             All config lang is released.
  @retval EFI_INVALID_PARAMETER   ConfigLangMapList is NULL.

**/
EFI_STATUS
ReleaseConfigLangMapList (
  IN  REDFISH_CONFIG_LANG_MAP_LIST  *ConfigLangMapList
  )
{
  LIST_ENTRY                      *List;
  LIST_ENTRY                      *Next;
  REDFISH_CONFIG_LANG_MAP_RECORD  *Record;

  if (ConfigLangMapList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (&ConfigLangMapList->Listheader)) {
    return EFI_SUCCESS;
  }

  Record = NULL;
  Next   = NULL;
  List   = GetFirstNode (&ConfigLangMapList->Listheader);
  while (!IsNull (&ConfigLangMapList->Listheader, List)) {
    Record = REDFISH_CONFIG_LANG_MAP_RECORD_FROM_LIST (List);
    Next   = GetNextNode (&ConfigLangMapList->Listheader, List);

    DeleteConfigLangMapRecord (ConfigLangMapList, Record);

    List = Next;
  }

  return EFI_SUCCESS;
}

/**
  Save config lang in list to UEFI variable.

  @param[in]    ConfigLangMapList The list to be saved.
  @param[in]    VariableName      The UEFI variable name.

  @retval EFI_SUCCESS             All config lang is saved.
  @retval EFI_INVALID_PARAMETER   VariableName or ConfigLangMapList is NULL.

**/
EFI_STATUS
SaveConfigLangMapList (
  IN  REDFISH_CONFIG_LANG_MAP_LIST  *ConfigLangMapList,
  IN  EFI_STRING                    VariableName
  )
{
  LIST_ENTRY                      *List;
  REDFISH_CONFIG_LANG_MAP_RECORD  *Record;
  UINT8                           *VarData;
  VOID                            *Data;
  EFI_STRING                      Seeker;
  UINTN                           VarSize;
  UINTN                           StringSize;
  EFI_STATUS                      Status;

  if ((ConfigLangMapList == NULL) || IS_EMPTY_STRING (VariableName)) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (&ConfigLangMapList->Listheader)) {
    return EFI_SUCCESS;
  }

  //
  // Calculate the total size we need to keep ConfigLangMap list.
  //
  VarSize = ConfigLangMapList->TotalSize + sizeof (CHAR16); // terminator character
  VarData = AllocateZeroPool (VarSize);
  if (VarData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Seeker = (EFI_STRING)VarData;
  Record = NULL;
  List   = GetFirstNode (&ConfigLangMapList->Listheader);
  while (!IsNull (&ConfigLangMapList->Listheader, List)) {
    Record = REDFISH_CONFIG_LANG_MAP_RECORD_FROM_LIST (List);

    StringSize = StrSize (Record->Uri);
    CopyMem (Seeker, Record->Uri, StringSize);

    Seeker += (StringSize / sizeof (CHAR16) - 1);
    *Seeker = L'|';
    ++Seeker;

    StringSize = StrSize (Record->ConfigLang);
    CopyMem (Seeker, Record->ConfigLang, StringSize);

    Seeker += (StringSize / sizeof (CHAR16) - 1);
    *Seeker = L'\n';

    ++Seeker;

    List = GetNextNode (&ConfigLangMapList->Listheader, List);
  }

  *Seeker = L'\0';

 #if CONFIG_LANG_MAP_DEBUG_ENABLED
  DumpRawBuffer (VarData, VarSize);
 #endif

  ASSERT (((UINTN)Seeker - (UINTN)VarData + sizeof (CHAR16)) == VarSize);

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

  Status = gRT->SetVariable (VariableName, &gEfiRedfishClientVariableGuid, VARIABLE_ATTRIBUTE_NV_BS, VarSize, (VOID *)VarData);

  FreePool (VarData);

  return Status;
}

/**
  Read config lang map from UEFI variable if it exists.

  @param[in]    ConfigLangMapList The list to be loaded.
  @param[in]    VariableName      The UEFI variable name.

  @retval EFI_SUCCESS             All config lang is read successfully.
  @retval EFI_INVALID_PARAMETER   VariableName or ConfigLangMapList is NULL.
  @retval EFI_NOT_FOUND           No config lang is found on UEFI variable.

**/
EFI_STATUS
InitialConfigLangMapList (
  IN  REDFISH_CONFIG_LANG_MAP_LIST  *ConfigLangMapList,
  IN  EFI_STRING                    VariableName
  )
{
  UINT8       *VarData;
  EFI_STRING  UriPointer;
  EFI_STRING  ConfigLangPointer;
  EFI_STRING  Seeker;
  UINTN       VariableSize;
  EFI_STATUS  Status;

  if ((ConfigLangMapList == NULL) || IS_EMPTY_STRING (VariableName)) {
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

  Seeker            = (EFI_STRING)VarData;
  UriPointer        = (EFI_STRING)VarData;
  ConfigLangPointer = (EFI_STRING)VarData;
  while (*Seeker != '\0') {
    //
    // Find URI
    //
    Seeker = StrStr (UriPointer, L"|");
    if (Seeker == NULL) {
      DEBUG ((DEBUG_ERROR, "%a, data corrupted\n", __func__));
      Status = EFI_DEVICE_ERROR;
      goto ON_ERROR;
    }

    *Seeker           = '\0';
    ConfigLangPointer = ++Seeker;

    //
    // Find config language map
    //
    Seeker = StrStr (ConfigLangPointer, L"\n");
    if (Seeker == NULL) {
      DEBUG ((DEBUG_ERROR, "%a, data corrupted\n", __func__));
      Status = EFI_DEVICE_ERROR;
      goto ON_ERROR;
    }

    *Seeker = '\0';

    AddConfigLangMapRecord (ConfigLangMapList, UriPointer, ConfigLangPointer);

    UriPointer = ++Seeker;
  }

 #if CONFIG_LANG_MAP_DEBUG_ENABLED
  DumpConfigLangMapList (ConfigLangMapList, L"Initial ConfigLangMap List from Variable");
 #endif

  Status = EFI_SUCCESS;

ON_ERROR:

  FreePool (VarData);

  return Status;
}

/**
  Get string in database by given query string.

  @param[in]   This                    Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.
  @param[in]   QueryStringType         The type of given QueryString.
  @param[in]   QueryString             Query string.
  @param[out]  ResultString            Returned string mapping to give query string.

  @retval EFI_SUCCESS                  The result is found successfully.
  @retval EFI_INVALID_PARAMETER        Invalid parameter is given.

**/
EFI_STATUS
RedfishConfigLangMapGet (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *This,
  IN  REDFISH_CONFIG_LANG_MAP_GET_TYPE        QueryStringType,
  IN  EFI_STRING                              QueryString,
  OUT EFI_STRING                              *ResultString
  )
{
  REDFISH_CONFIG_LANG_MAP_RECORD        *Target;
  REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA  *Private;
  EFI_STRING                            Result;

  if ((This == NULL) || IS_EMPTY_STRING (QueryString) || (ResultString == NULL) || (QueryStringType >= RedfishGetTypeMax)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((CONFIG_LANG_MAP_TRACE, "%a: type: 0x%x %s\n", __func__, QueryStringType, QueryString));

  Private = REDFISH_CONFIG_LANG_MAP_PRIVATE_FROM_THIS (This);

  *ResultString = NULL;

  Target = FindConfigLangMapRecord (&Private->ConfigLangList.Listheader, QueryString, (QueryStringType == RedfishGetTypeUri));
  if (Target == NULL) {
 #if CONFIG_LANG_MAP_DEBUG_ENABLED
    DumpConfigLangMapList (&Private->ConfigLangList, L"EFI_NOT_FOUND");
 #endif
    return EFI_NOT_FOUND;
  }

  Result        = (QueryStringType == RedfishGetTypeUri ? Target->ConfigLang : Target->Uri);
  *ResultString = AllocateCopyPool (StrSize (Result), Result);
  if (*ResultString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Save URI string which maps to given ConfigLang.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.
  @param[in]   ConfigLang          Config language to set
  @param[in]   Uri                 Uri which is mapping to give ConfigLang. If Uri is NULL,
                                   the record will be removed.

  @retval EFI_SUCCESS              Uri is saved successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishConfigLangMapSet (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *This,
  IN  EFI_STRING                              ConfigLang,
  IN  EFI_STRING                              Uri        OPTIONAL
  )
{
  REDFISH_CONFIG_LANG_MAP_RECORD        *Target;
  REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA  *Private;
  EFI_STATUS                            Status;

  if ((This == NULL) || IS_EMPTY_STRING (ConfigLang)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((CONFIG_LANG_MAP_TRACE, "%a: %s -> %s\n", __func__, ConfigLang, (Uri == NULL ? L"NULL" : Uri)));

  Private = REDFISH_CONFIG_LANG_MAP_PRIVATE_FROM_THIS (This);

  Status = EFI_NOT_FOUND;
  Target = FindConfigLangMapRecord (&Private->ConfigLangList.Listheader, ConfigLang, FALSE);
  if (Target != NULL) {
    //
    // Remove old one and create new one.
    //
    Status = DeleteConfigLangMapRecord (&Private->ConfigLangList, Target);
  }

  //
  // When Uri is NULL, it means that we want to remove this record.
  //
  if (Uri == NULL) {
    return Status;
  }

  return AddConfigLangMapRecord (&Private->ConfigLangList, Uri, ConfigLang);
}

/**
  Refresh the resource map database and save database to variable.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishConfigLangMapFlush (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *This
  )
{
  REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA  *Private;
  EFI_STATUS                            Status;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = REDFISH_CONFIG_LANG_MAP_PRIVATE_FROM_THIS (This);

  Status = SaveConfigLangMapList (&Private->ConfigLangList, Private->VariableName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, save ConfigLangMap list to variable: %s failed: %r\n", __func__, Private->VariableName, Status));
  }

  DEBUG ((CONFIG_LANG_MAP_TRACE, "%a: save Config Language map to variable: %s\n", __func__, Private->VariableName));

  return Status;
}

/**
  Callback function executed when the AfterProvisioning event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[out]  Context  Pointer to the Context buffer

**/
VOID
EFIAPI
RedfishConfigLangMapOnAfterProvisioning (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  //
  // Redfish provisioning is finished. Keep list into variable.
  //
  RedfishConfigLangMapFlush (&mRedfishConfigLangMapPrivate->Protocol);
}

/**
  Unloads an image.

  @param[in]  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishConfigLangMapDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;

  if (mRedfishConfigLangMapPrivate != NULL) {
    Status = gBS->UninstallProtocolInterface (
                    mRedfishConfigLangMapPrivate->ImageHandle,
                    &gEdkIIRedfishConfigLangMapProtocolGuid,
                    (VOID *)&mRedfishConfigLangMapPrivate->Protocol
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a, can not uninstall gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __func__, Status));
      ASSERT (FALSE);
    }

    ReleaseConfigLangMapList (&mRedfishConfigLangMapPrivate->ConfigLangList);

    if (mRedfishConfigLangMapPrivate->VariableName != NULL) {
      FreePool (mRedfishConfigLangMapPrivate->VariableName);
    }

    if (mRedfishConfigLangMapPrivate->ExitBootEvent != NULL) {
      gBS->CloseEvent (mRedfishConfigLangMapPrivate->ExitBootEvent);
    }

    if (mRedfishConfigLangMapPrivate->ProvisionEvent != NULL) {
      gBS->CloseEvent (mRedfishConfigLangMapPrivate->ProvisionEvent);
    }

    FreePool (mRedfishConfigLangMapPrivate);
    mRedfishConfigLangMapPrivate = NULL;
  }

  return EFI_SUCCESS;
}

//
// EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL.
//
EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  mRedfishConfigLangMapProtocol = {
  RedfishConfigLangMapGet,
  RedfishConfigLangMapSet,
  RedfishConfigLangMapFlush
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
RedfishConfigLangMapDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  mRedfishConfigLangMapPrivate = AllocateZeroPool (sizeof (REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA));
  if (mRedfishConfigLangMapPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InitializeListHead (&mRedfishConfigLangMapPrivate->ConfigLangList.Listheader);
  mRedfishConfigLangMapPrivate->VariableName = AllocateCopyPool (StrSize (CONFIG_LANG_MAP_VARIABLE_NAME), CONFIG_LANG_MAP_VARIABLE_NAME);
  if (mRedfishConfigLangMapPrivate->VariableName == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  mRedfishConfigLangMapPrivate->ImageHandle = ImageHandle;
  CopyMem (&mRedfishConfigLangMapPrivate->Protocol, &mRedfishConfigLangMapProtocol, sizeof (EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL));

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkIIRedfishConfigLangMapProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *)&mRedfishConfigLangMapPrivate->Protocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, can not install gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __func__, Status));
    ASSERT (FALSE);
    goto ON_ERROR;
  }

  //
  // Read existing record from variable.
  //
  DEBUG ((CONFIG_LANG_MAP_TRACE, "%a, Initial ConfigLangMap List from variable: %s\n", __func__, mRedfishConfigLangMapPrivate->VariableName));
  Status = InitialConfigLangMapList (&mRedfishConfigLangMapPrivate->ConfigLangList, mRedfishConfigLangMapPrivate->VariableName);
  if (EFI_ERROR (Status)) {
    DEBUG ((CONFIG_LANG_MAP_TRACE, "%a, Initial ConfigLangMap List: %r\n", __func__, Status));
  }

  //
  // Register after provisioning event
  //
  Status = CreateAfterProvisioningEvent (
             RedfishConfigLangMapOnAfterProvisioning,
             NULL,
             &mRedfishConfigLangMapPrivate->ProvisionEvent
             );

  return EFI_SUCCESS;

ON_ERROR:

  RedfishConfigLangMapDriverUnload (ImageHandle);

  return Status;
}
