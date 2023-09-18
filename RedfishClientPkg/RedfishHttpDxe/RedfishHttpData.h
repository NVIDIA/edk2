/** @file
  Definitions of RedfishHttpData

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_HTTP_DATA_H_
#define EDKII_REDFISH_HTTP_DATA_H_

#include "RedfishHttpDxe.h"

///
/// Definition of REDFISH_HTTP_CACHE_DATA
///
typedef struct {
  LIST_ENTRY          List;
  EFI_STRING          Uri;
  UINTN               HitCount;
  REDFISH_RESPONSE    *Response;
} REDFISH_HTTP_CACHE_DATA;

#define REDFISH_HTTP_CACHE_FROM_LIST(a)  BASE_CR (a, REDFISH_HTTP_CACHE_DATA, List)

///
/// Definition of REDFISH_HTTP_CACHE_LIST
///
typedef struct {
  LIST_ENTRY    Head;
  UINTN         Count;
  UINTN         Capacity;
} REDFISH_HTTP_CACHE_LIST;

///
/// Definition of REDFISH_HTTP_RETRY_SETTING
///
typedef struct {
  UINT16    MaximumRetryGet;
  UINT16    MaximumRetryPut;
  UINT16    MaximumRetryPost;
  UINT16    MaximumRetryPatch;
  UINT16    MaximumRetryDelete;
  UINTN     RetryWait;
} REDFISH_HTTP_RETRY_SETTING;

///
/// Definition of REDFISH_HTTP_CACHE_PRIVATE
///
typedef struct {
  EFI_HANDLE                     ImageHandle;
  BOOLEAN                        CacheDisabled;
  REDFISH_HTTP_CACHE_LIST        CacheList;
  EDKII_REDFISH_HTTP_PROTOCOL    Protocol;
  REDFISH_HTTP_RETRY_SETTING     RetrySetting;
} REDFISH_HTTP_CACHE_PRIVATE;

#define REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS(a)  BASE_CR (a, REDFISH_HTTP_CACHE_PRIVATE, Protocol)

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
  );

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
  );

/**
  Release all cache from list.

  @param[in]    CacheList    The list to be released.

  @retval EFI_SUCCESS             All cache data are released.
  @retval EFI_INVALID_PARAMETER   CacheList is NULL.

**/
EFI_STATUS
ReleaseCacheList (
  IN  REDFISH_HTTP_CACHE_LIST  *CacheList
  );

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
  );

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
  );

#endif
