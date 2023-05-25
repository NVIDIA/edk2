/** @file
  Redfish HTTP cache library helps Redfish application to get Redfish resource
  from BMC with cache mechanism enabled.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishHttpCacheLibInternal.h"

REDFISH_HTTP_CACHE_PRIVATE  *mRedfishHttpCachePrivate = NULL;

/**
  This function copy the data in SrcResponse to DstResponse.

  @param[in]  SrcResponse      Source Response to copy.
  @param[out] DstResponse      Destination Response.

  @retval     EFI_SUCCESS      Response is copied successfully.
  @retval     Others           Error occurs.

**/
EFI_STATUS
CopyRedfishResponse (
  IN  REDFISH_RESPONSE  *SrcResponse,
  OUT REDFISH_RESPONSE  *DstResponse
  )
{
  EDKII_JSON_VALUE  JsonValue;
  REDFISH_SERVICE   Service;
  UINTN             Index;

  if ((SrcResponse == NULL) || (DstResponse == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (SrcResponse == DstResponse) {
    return EFI_SUCCESS;
  }

  //
  // Status code
  //
  if (SrcResponse->StatusCode != NULL) {
    DstResponse->StatusCode = AllocateCopyPool (sizeof (EFI_HTTP_STATUS_CODE), SrcResponse->StatusCode);
    if (DstResponse->StatusCode == NULL) {
      goto ON_ERROR;
    }
  }

  //
  // Header
  //
  if ((SrcResponse->HeaderCount > 0) && (SrcResponse->Headers != NULL)) {
    DstResponse->HeaderCount = 0;
    DstResponse->Headers     = AllocateZeroPool (sizeof (EFI_HTTP_HEADER) * SrcResponse->HeaderCount);
    if (DstResponse->Headers == NULL) {
      goto ON_ERROR;
    }

    for (Index = 0; Index < SrcResponse->HeaderCount; Index++) {
      DstResponse->Headers[Index].FieldName = AllocateCopyPool (AsciiStrSize (SrcResponse->Headers[Index].FieldName), SrcResponse->Headers[Index].FieldName);
      if (DstResponse->Headers[Index].FieldName == NULL) {
        goto ON_ERROR;
      }

      DstResponse->Headers[Index].FieldValue = AllocateCopyPool (AsciiStrSize (SrcResponse->Headers[Index].FieldValue), SrcResponse->Headers[Index].FieldValue);
      if (DstResponse->Headers[Index].FieldValue == NULL) {
        goto ON_ERROR;
      }

      DstResponse->HeaderCount += 1;
    }
  }

  //
  // Payload
  //
  if (SrcResponse->Payload != NULL) {
    Service              = RedfishServiceInPayload (SrcResponse->Payload);
    JsonValue            = RedfishJsonInPayload (SrcResponse->Payload);
    DstResponse->Payload = RedfishCreatePayload (JsonValue, Service);
    if (DstResponse->Payload  == NULL) {
      goto ON_ERROR;
    }
  }

  return EFI_SUCCESS;

ON_ERROR:

  RedfishFreeResponse (
    DstResponse->StatusCode,
    DstResponse->HeaderCount,
    DstResponse->Headers,
    DstResponse->Payload
    );

  return EFI_OUT_OF_RESOURCES;
}

/**
  This function clone input response and return to caller

  @param[in]  Response      Response to clone.

  @retval     REDFISH_RESPONSE *  Response is cloned.
  @retval     NULL                Errors occur.

**/
REDFISH_RESPONSE *
CloneRedfishResponse (
  IN REDFISH_RESPONSE  *Response
  )
{
  EFI_STATUS        Status;
  REDFISH_RESPONSE  *NewResponse;

  if (Response == NULL) {
    return NULL;
  }

  NewResponse = AllocateZeroPool (sizeof (REDFISH_RESPONSE));
  if (NewResponse == NULL) {
    return NULL;
  }

  Status = CopyRedfishResponse (Response, NewResponse);
  if (EFI_ERROR (Status)) {
    FreePool (NewResponse);
    return NULL;
  }

  return NewResponse;
}

/**

  Convert Unicode string to ASCII string. It's call responsibility to release returned buffer.

  @param[in]  UnicodeStr      Unicode string to convert.

  @retval     CHAR8 *         ASCII string returned.
  @retval     NULL            Errors occur.

**/
CHAR8 *
StringUnicodeToAscii (
  IN EFI_STRING  UnicodeStr
  )
{
  CHAR8       *AsciiStr;
  UINTN       AsciiStrSize;
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (UnicodeStr)) {
    return NULL;
  }

  AsciiStrSize = StrLen (UnicodeStr) + 1;
  AsciiStr     = AllocatePool (AsciiStrSize);
  if (AsciiStr == NULL) {
    return NULL;
  }

  Status = UnicodeStrToAsciiStrS (UnicodeStr, AsciiStr, AsciiStrSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UnicodeStrToAsciiStrS failed: %r\n", Status));
    FreePool (AsciiStr);
    return NULL;
  }

  return AsciiStr;
}

/**
  Release REDFISH_HTTP_CACHE_DATA resource

  @param[in]    Data    Pointer to REDFISH_HTTP_CACHE_DATA instance

  @retval EFI_SUCCESS             REDFISH_HTTP_CACHE_DATA is released successfully.
  @retval EFI_INVALID_PARAMETER   Data is NULL

**/
EFI_STATUS
ReleaseHttpCacheData (
  IN REDFISH_HTTP_CACHE_DATA  *Data
  )
{
  if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Data->Uri != NULL) {
    FreePool (Data->Uri);
  }

  if (Data->Response != NULL) {
    if (Data->Response->Payload != NULL) {
      RedfishFreeResponse (
        Data->Response->StatusCode,
        Data->Response->HeaderCount,
        Data->Response->Headers,
        Data->Response->Payload
        );
      FreePool (Data->Response);
    }
  }

  FreePool (Data);

  return EFI_SUCCESS;
}

/**
  Create new cache data.

  @param[in]    Uri       The URI string matching to this cache data.
  @param[in]    Response  HTTP response.

  @retval REDFISH_HTTP_CACHE_DATA *   Pointer to newly created cache data.
  @retval NULL                        No memory available.

**/
REDFISH_HTTP_CACHE_DATA *
NewHttpCacheData (
  IN  EFI_STRING        Uri,
  IN  REDFISH_RESPONSE  *Response
  )
{
  REDFISH_HTTP_CACHE_DATA  *NewData;
  UINTN                    Size;

  if (IS_EMPTY_STRING (Uri) || (Response == NULL)) {
    return NULL;
  }

  NewData = AllocateZeroPool (sizeof (REDFISH_HTTP_CACHE_DATA));
  if (NewData == NULL) {
    return NULL;
  }

  Size         = StrSize (Uri);
  NewData->Uri = AllocateCopyPool (Size, Uri);
  if (NewData->Uri == NULL) {
    goto ON_ERROR;
  }

  NewData->Response = Response;
  NewData->HitCount = 1;

  return NewData;

ON_ERROR:

  if (NewData != NULL) {
    ReleaseHttpCacheData (NewData);
  }

  return NULL;
}

/**
  Search on given ListHeader for given URI string.

  @param[in]    ListHeader  Target list to search.
  @param[in]    Uri         Target URI to search.

  @retval REDFISH_HTTP_CACHE_DATA   Target cache data is found.
  @retval NULL                      No cache data with given URI is found.

**/
REDFISH_HTTP_CACHE_DATA *
FindHttpCacheData (
  IN  LIST_ENTRY  *ListHeader,
  IN  EFI_STRING  Uri
  )
{
  LIST_ENTRY               *List;
  REDFISH_HTTP_CACHE_DATA  *Data;

  if (IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  if (IsListEmpty (ListHeader)) {
    return NULL;
  }

  Data = NULL;
  List = GetFirstNode (ListHeader);
  while (!IsNull (ListHeader, List)) {
    Data = REDFISH_HTTP_CACHE_FROM_LIST (List);

    if (StrCmp (Data->Uri, Uri) == 0) {
      return Data;
    }

    List = GetNextNode (ListHeader, List);
  }

  return NULL;
}

/**
  Search on given ListHeader and return cache data with minimum hit count.

  @param[in]    ListHeader  Target list to search.

  @retval REDFISH_HTTP_CACHE_DATA   Target cache data is returned.
  @retval NULL                      No cache data is found.

**/
REDFISH_HTTP_CACHE_DATA *
FindUnusedHttpCacheData (
  IN  LIST_ENTRY  *ListHeader
  )
{
  LIST_ENTRY               *List;
  REDFISH_HTTP_CACHE_DATA  *Data;
  REDFISH_HTTP_CACHE_DATA  *UnusedData;
  UINTN                    HitCount;

  if (IsListEmpty (ListHeader)) {
    return NULL;
  }

  Data       = NULL;
  UnusedData = NULL;
  HitCount   = 0;

  List       = GetFirstNode (ListHeader);
  Data       = REDFISH_HTTP_CACHE_FROM_LIST (List);
  UnusedData = Data;
  HitCount   = Data->HitCount;
  List       = GetNextNode (ListHeader, List);

  while (!IsNull (ListHeader, List)) {
    Data = REDFISH_HTTP_CACHE_FROM_LIST (List);

    if (Data->HitCount < HitCount) {
      HitCount   = Data->HitCount;
      UnusedData = Data;
    }

    List = GetNextNode (ListHeader, List);
  }

  return UnusedData;
}

/**
  Delete a cache data by given cache instance.

  @param[in]    List    Target cache list to be removed.
  @param[in]    Data    Pointer to the instance to be deleted.

  @retval EFI_SUCCESS   Cache data is removed.
  @retval Others        Fail to remove cache data.

**/
EFI_STATUS
DeleteHttpCacheData (
  IN  REDFISH_HTTP_CACHE_LIST  *List,
  IN  REDFISH_HTTP_CACHE_DATA  *Data
  )
{
  if ((List == NULL) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: delete: %s\n", __func__, Data->Uri));

  RemoveEntryList (&Data->List);
  --List->Count;

  return ReleaseHttpCacheData (Data);
}

/**
  Add new cache by given URI and HTTP response to specify List.

  @param[in]    List      Target cache list to add.
  @param[in]    Uri       The URI string matching to this cache data.
  @param[in]    Response  HTTP response.

  @retval EFI_SUCCESS   Cache data is added.
  @retval Others        Fail to add cache data.

**/
EFI_STATUS
AddHttpCacheData (
  IN  REDFISH_HTTP_CACHE_LIST  *List,
  IN  EFI_STRING               Uri,
  IN  REDFISH_RESPONSE         *Response
  )
{
  REDFISH_HTTP_CACHE_DATA  *NewData;
  REDFISH_HTTP_CACHE_DATA  *OldData;
  REDFISH_HTTP_CACHE_DATA  *UnusedData;
  REDFISH_RESPONSE         *NewResponse;

  if ((List == NULL) || IS_EMPTY_STRING (Uri) || (Response == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If same cache data exist, replace it with latest one.
  //
  OldData = FindHttpCacheData (&List->Head, Uri);
  if (OldData != NULL) {
    DeleteHttpCacheData (List, OldData);
  }

  //
  // Check capacity
  //
  if (List->Count >= List->Capacity) {
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: list is full and retire unused cache\n", __func__));
    UnusedData = FindUnusedHttpCacheData (&List->Head);
    if (UnusedData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    DeleteHttpCacheData (List, UnusedData);
  }

  //
  // Clone a local copy
  //
  NewResponse = CloneRedfishResponse (Response);
  if (NewResponse == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewData = NewHttpCacheData (Uri, NewResponse);
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&List->Head, &NewData->List);
  ++List->Count;

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: cache(%d/%d) %s\n", __func__, List->Count, List->Capacity, NewData->Uri));

  return EFI_SUCCESS;
}

/**
  Release all cache from list.

  @param[in]    CacheList    The list to be released.

  @retval EFI_SUCCESS             All cache data are released.
  @retval EFI_INVALID_PARAMETER   CacheList is NULL.

**/
EFI_STATUS
ReleaseCacheList (
  IN  REDFISH_HTTP_CACHE_LIST  *CacheList
  )
{
  LIST_ENTRY               *List;
  LIST_ENTRY               *Next;
  REDFISH_HTTP_CACHE_DATA  *Data;

  if (CacheList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (&CacheList->Head)) {
    return EFI_SUCCESS;
  }

  Data = NULL;
  Next = NULL;
  List = GetFirstNode (&CacheList->Head);
  while (!IsNull (&CacheList->Head, List)) {
    Data = REDFISH_HTTP_CACHE_FROM_LIST (List);
    Next = GetNextNode (&CacheList->Head, List);

    DeleteHttpCacheData (CacheList, Data);

    List = Next;
  }

  return EFI_SUCCESS;
}

/**
  Debug output the cache list.

  @param[in]    Msg            Debug message string.
  @param[in]    ErrorLevel     Output error level.
  @param[in]    CacheList      Target list to dump.

  @retval EFI_SUCCESS             Debug dump finished.
  @retval EFI_INVALID_PARAMETER   HttpCacheList is NULL.

**/
EFI_STATUS
DumpHttpCacheList (
  IN  CONST CHAR8              *Msg,
  IN  UINTN                    ErrorLevel,
  IN  REDFISH_HTTP_CACHE_LIST  *CacheList
  )
{
  LIST_ENTRY               *List;
  REDFISH_HTTP_CACHE_DATA  *Data;
  UINTN                    Index;

  if (CacheList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IS_EMPTY_STRING (Msg)) {
    DEBUG ((ErrorLevel, "%a\n", Msg));
  }

  if (IsListEmpty (&CacheList->Head)) {
    DEBUG ((ErrorLevel, "list is empty\n"));
    return EFI_NOT_FOUND;
  }

  DEBUG ((ErrorLevel, "list count: %d capacity: %d\n", CacheList->Count, CacheList->Capacity));
  Data  = NULL;
  Index = 0;
  List  = GetFirstNode (&CacheList->Head);
  while (!IsNull (&CacheList->Head, List)) {
    Data = REDFISH_HTTP_CACHE_FROM_LIST (List);

    DEBUG ((ErrorLevel, "%d) Uri: %s Hit: %d\n", ++Index, Data->Uri, Data->HitCount));

    List = GetNextNode (&CacheList->Head, List);
  }

  return EFI_SUCCESS;
}

/**
  Get redfish resource from given resource URI with cache mechanism
  supported. It's caller's responsibility to Response by calling
  RedfishFreeResponse ().

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[out] Response      HTTP response from redfish service.
  @param[in]  UseCache      If it is TRUE, this function will search for
                            cache first. If it is FALSE, this function
                            will query Redfish URI directly.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpGetResource (
  IN  REDFISH_SERVICE   *Service,
  IN  EFI_STRING        Uri,
  OUT REDFISH_RESPONSE  *Response,
  IN  BOOLEAN           UseCache
  )
{
  EFI_STATUS               Status;
  CHAR8                    *AsciiUri;
  REDFISH_HTTP_CACHE_DATA  *CacheData;
  UINTN                    RetryCount;

  if ((Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mRedfishHttpCachePrivate == NULL) {
    return EFI_NOT_READY;
  }

  AsciiUri   = NULL;
  CacheData  = NULL;
  RetryCount = 0;

  //
  // Search for cache list.
  //
  if (UseCache) {
    CacheData = FindHttpCacheData (&mRedfishHttpCachePrivate->CacheList.Head, Uri);
    if (CacheData != NULL) {
      DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: cache hit! %s\n", __func__, Uri));

      //
      // Copy cached response to caller's buffer.
      //
      Status               = CopyRedfishResponse (CacheData->Response, Response);
      CacheData->HitCount += 1;
      return Status;
    }
  }

  AsciiUri = StringUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get resource from redfish service.
  //
  do {
    RetryCount += 1;
    Status      = RedfishGetByUri (
                    Service,
                    AsciiUri,
                    Response
                    );
    if (!EFI_ERROR (Status) || (RetryCount >= REDFISH_HTTP_GET_RETRY_MAX)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (*Response->StatusCode != HTTP_STATUS_500_INTERNAL_SERVER_ERROR) {
        break;
      }

      FreePool (Response->StatusCode);
      Response->StatusCode = NULL;
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishGetByUri failed, retry (%d/%d)\n", __func__, RetryCount, REDFISH_HTTP_GET_RETRY_MAX));
    gBS->Stall (REDFISH_HTTP_RETRY_WAIT);
  } while (TRUE);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: get %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, REDFISH_HTTP_GET_RETRY_MAX, Status));
    if (Response->Payload != NULL) {
      RedfishFreeResponse (
        NULL,
        0,
        NULL,
        Response->Payload
        );
      Response->Payload = NULL;
    }

    goto ON_RELEASE;
  }

  //
  // Keep response in cache list
  //
  Status = AddHttpCacheData (&mRedfishHttpCachePrivate->CacheList, Uri, Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to cache %s: %r\n", __func__, Uri, Status));
    goto ON_RELEASE;
  }

  DEBUG_CODE (
    DumpHttpCacheList (__func__, REDFISH_HTTP_CACHE_DEBUG_DUMP, &mRedfishHttpCachePrivate->CacheList);
    );

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

/**
  Reset the cached data specified by given URI. When response data
  returned by RedfishHttpResetResource() is modified, the response
  data can not be used by other caller. Application calls this
  function to make this data to be stale data and
  RedfishHttpResetResource() will get latest data from remote server
  again.

  @param[in]  Uri           Target resource URI.

  @retval     EFI_SUCCESS     Resrouce is reset successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpResetResource (
  IN  EFI_STRING  Uri
  )
{
  REDFISH_HTTP_CACHE_DATA  *CacheData;

  if (IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mRedfishHttpCachePrivate == NULL) {
    return EFI_NOT_READY;
  }

  CacheData = FindHttpCacheData (&mRedfishHttpCachePrivate->CacheList.Head, Uri);
  if (CacheData == NULL) {
    return EFI_NOT_FOUND;
  }

  DeleteHttpCacheData (&mRedfishHttpCachePrivate->CacheList, CacheData);

  return EFI_SUCCESS;
}

/**

  Initial HTTP cache library instance.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCCESS  Initial library successfully.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishHttpCacheConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  mRedfishHttpCachePrivate = AllocateZeroPool (sizeof (REDFISH_HTTP_CACHE_PRIVATE));
  if (mRedfishHttpCachePrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initial cache list
  //
  mRedfishHttpCachePrivate->CacheList.Capacity = REDFISH_HTTP_CACHE_LIST_SIZE;
  mRedfishHttpCachePrivate->CacheList.Count    = 0x00;
  InitializeListHead (&mRedfishHttpCachePrivate->CacheList.Head);

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
RedfishHttpCacheDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mRedfishHttpCachePrivate != NULL) {
    if (!IsListEmpty (&mRedfishHttpCachePrivate->CacheList.Head)) {
      ReleaseCacheList (&mRedfishHttpCachePrivate->CacheList);
    }

    FreePool (mRedfishHttpCachePrivate);
    mRedfishHttpCachePrivate = NULL;
  }

  return EFI_SUCCESS;
}
