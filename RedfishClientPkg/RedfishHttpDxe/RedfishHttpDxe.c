/** @file
  RedfishHttpDxe produces EdkIIRedfishHttpProtocol
  for EDK2 Redfish Feature driver to do HTTP operations.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishHttpDxe.h"
#include "RedfishHttpData.h"

REDFISH_HTTP_CACHE_PRIVATE  *mRedfishHttpCachePrivate = NULL;

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

  Check HTTP status code to see if we like to retry HTTP request or not.

  @param[in]  StatusCode      HTTP status code.

  @retval     BOOLEAN         Return true when we like to retry request.
                              Return false when we don't want to retry request.

**/
BOOLEAN
RedfishRetryRequired (
  IN EFI_HTTP_STATUS_CODE  *StatusCode
  )
{
  if (StatusCode == NULL) {
    return TRUE;
  }

  if ((*StatusCode == HTTP_STATUS_500_INTERNAL_SERVER_ERROR) ||
      (*StatusCode == HTTP_STATUS_UNSUPPORTED_STATUS))
  {
    return TRUE;
  }

  return FALSE;
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
  Return HTTP method in ASCII string. Caller does not need
  to free returned string buffer.

  @param[in]  Method         HTTP method.

  @retval CHAR8 *   Method in string.
**/
CHAR8 *
HttpMethodToString (
  IN  EFI_HTTP_METHOD  Method
  )
{
  switch (Method) {
    case HttpMethodGet:
      return HTTP_METHOD_GET;
      break;
    case HttpMethodPost:
      return HTTP_METHOD_POST;
      break;
    case HttpMethodPatch:
      return HTTP_METHOD_PATCH;
      break;
    case HttpMethodPut:
      return HTTP_METHOD_PUT;
      break;
    case HttpMethodDelete:
      return HTTP_METHOD_DELETE;
      break;
    default:
      break;
  }

  return "Unknown";
}

/**
  Report HTTP communication error via report status code.

  @param[in]  Method         HTTP method.
  @param[in]  Uri            The URI which has failure.
  @param[in]  HttpStatusCode HTTP status code.

**/
VOID
ReportHttpError (
  IN  EFI_HTTP_METHOD       Method,
  IN  CHAR8                 *Uri,
  IN  EFI_HTTP_STATUS_CODE  *HttpStatusCode  OPTIONAL
  )
{
  CHAR8  ErrorMsg[REDFISH_ERROR_MSG_MAX];

  if (IS_EMPTY_STRING (Uri)) {
    DEBUG ((DEBUG_ERROR, "%a: no URI to report error status\n", __func__));
    return;
  }

  //
  // Report failure of URI and HTTP status code.
  //
  AsciiSPrint (ErrorMsg, sizeof (ErrorMsg), REDFISH_HTTP_ERROR_REPORT, HttpMethodToString (Method), (HttpStatusCode == NULL ? HTTP_STATUS_UNSUPPORTED_STATUS : *HttpStatusCode), Uri);

  REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
    EFI_ERROR_CODE | EFI_ERROR_MAJOR,
    EFI_COMPUTING_UNIT_MANAGEABILITY | EFI_MANAGEABILITY_EC_REDFISH_COMMUNICATION_ERROR,
    ErrorMsg,
    AsciiStrSize (ErrorMsg)
    );
}

/**
  Perform HTTP GET to Get redfish resource from given resource URI with
  cache mechanism supported. It's caller's responsibility to free Response
  by calling FreeResponse ().

  @param[in]  This          Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
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
EFIAPI
RedfishHttpGetResource (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_SERVICE              Service,
  IN  EFI_STRING                   Uri,
  OUT REDFISH_RESPONSE             *Response,
  IN  BOOLEAN                      UseCache
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *AsciiUri;
  REDFISH_HTTP_CACHE_DATA     *CacheData;
  UINTN                       RetryCount;
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Get URI: %s cache: %a\n", __func__, Uri, (UseCache ? "true" : "false")));

  Private    = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  AsciiUri   = NULL;
  CacheData  = NULL;
  RetryCount = 0;
  ZeroMem (Response, sizeof (REDFISH_RESPONSE));

  if (Private->CacheDisabled) {
    UseCache = FALSE;
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: cache is disabled by PCD!\n", __func__));
  }

  //
  // Search for cache list.
  //
  if (UseCache) {
    CacheData = FindHttpCacheData (&Private->CacheList.Head, Uri);
    if (CacheData != NULL) {
      DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: cache hit! %s\n", __func__, Uri));

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
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: HTTP request: %a :%r\n", __func__, AsciiUri, Status));
    if (!EFI_ERROR (Status) || (RetryCount >= Private->RetrySetting.MaximumRetryGet)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (!RedfishRetryRequired (Response->StatusCode)) {
        break;
      }

      //
      // Release response for next round of request.
      //
      This->FreeResponse (This, Response);
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishGetByUri failed, retry (%d/%d)\n", __func__, RetryCount, Private->RetrySetting.MaximumRetryGet));
    if (Private->RetrySetting.RetryWait > 0) {
      gBS->Stall (Private->RetrySetting.RetryWait);
    }
  } while (TRUE);

  if (EFI_ERROR (Status)) {
    DEBUG_CODE (
      DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
      );
    //
    // Report status code for Redfish failure
    //
    ReportHttpError (HttpMethodGet, AsciiUri, Response->StatusCode);
    DEBUG ((DEBUG_ERROR, "%a: get %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, Private->RetrySetting.MaximumRetryGet, Status));
    goto ON_RELEASE;
  }

  if (!Private->CacheDisabled) {
    //
    // Keep response in cache list
    //
    Status = AddHttpCacheData (&Private->CacheList, Uri, Response);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to cache %s: %r\n", __func__, Uri, Status));
      goto ON_RELEASE;
    }

    DEBUG_CODE (
      DumpHttpCacheList (__func__, REDFISH_HTTP_CACHE_DEBUG_DUMP, &Private->CacheList);
      );
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

/**
  This function free resources in given Response.

  @param[in]  This         Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Response     HTTP response to be released.

  @retval     EFI_SUCCESS     Resrouce is released successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
EFIAPI
RedfishHttpFreeResponse (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_RESPONSE             *Response
  )
{
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Response == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: entry\n", __func__));

  Private = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  //
  // Call function in RedfishLib to release resource.
  //
  RedfishFreeResponse (
    Response->StatusCode,
    Response->HeaderCount,
    Response->Headers,
    Response->Payload
    );

  return EFI_SUCCESS;
}

/**
  This function expire the cached response of given URI.

  @param[in]  This         Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Uri          Target response of URI.

  @retval     EFI_SUCCESS     Target response is expired successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpExpireResponse (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  EFI_STRING                   Uri
  )
{
  REDFISH_HTTP_CACHE_PRIVATE  *Private;
  REDFISH_HTTP_CACHE_DATA     *CacheData;

  if ((This == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: expire URI: %s\n", __func__, Uri));

  Private = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);

  CacheData = FindHttpCacheData (&Private->CacheList.Head, Uri);
  if (CacheData == NULL) {
    return EFI_NOT_FOUND;
  }

  return DeleteHttpCacheData (&Private->CacheList, CacheData);
}

/**
  Perform HTTP PATCH to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  This          Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to patch.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
EFIAPI
RedfishHttpPatchResource (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_SERVICE              Service,
  IN  EFI_STRING                   Uri,
  IN  CHAR8                        *Content,
  OUT REDFISH_RESPONSE             *Response
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *AsciiUri;
  UINTN                       RetryCount;
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Content)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Patch URI: %s\n", __func__, Uri));

  Private    = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  AsciiUri   = NULL;
  RetryCount = 0;
  ZeroMem (Response, sizeof (REDFISH_RESPONSE));

  AsciiUri = StringUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Patch resource to redfish service.
  //
  do {
    RetryCount += 1;
    Status      = RedfishPatchToUri (
                    Service,
                    AsciiUri,
                    Content,
                    Response
                    );
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: HTTP request: %a :%r\n", __func__, AsciiUri, Status));
    if (!EFI_ERROR (Status) || (RetryCount >= Private->RetrySetting.MaximumRetryPatch)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (!RedfishRetryRequired (Response->StatusCode)) {
        break;
      }

      //
      // Release response for next round of request.
      //
      This->FreeResponse (This, Response);
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishPatchToUri failed, retry (%d/%d)\n", __func__, RetryCount, Private->RetrySetting.MaximumRetryPatch));
    if (Private->RetrySetting.RetryWait > 0) {
      gBS->Stall (Private->RetrySetting.RetryWait);
    }
  } while (TRUE);

  //
  // Redfish resource is updated. Automatically expire the cached response
  // so application can directly get resource from Redfish service again.
  //
  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Resource is updated, expire URI: %s\n", __func__, Uri));
  RedfishHttpExpireResponse (This, Uri);

  if (EFI_ERROR (Status)) {
    DEBUG_CODE (
      DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
      );
    //
    // Report status code for Redfish failure
    //
    ReportHttpError (HttpMethodPatch, AsciiUri, Response->StatusCode);
    DEBUG ((DEBUG_ERROR, "%a: patch %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, Private->RetrySetting.MaximumRetryGet, Status));
    goto ON_RELEASE;
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

/**
  Perform HTTP PUT to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  This          Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to put.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
EFIAPI
RedfishHttpPutResource (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_SERVICE              Service,
  IN  EFI_STRING                   Uri,
  IN  CHAR8                        *Content,
  OUT REDFISH_RESPONSE             *Response
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *AsciiUri;
  UINTN                       RetryCount;
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Content)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Put URI: %s\n", __func__, Uri));

  Private    = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  AsciiUri   = NULL;
  RetryCount = 0;
  ZeroMem (Response, sizeof (REDFISH_RESPONSE));

  AsciiUri = StringUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Patch resource to redfish service.
  //
  do {
    RetryCount += 1;
    Status      = RedfishPutToUri (
                    Service,
                    AsciiUri,
                    Content,
                    0,
                    NULL,
                    Response
                    );
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: HTTP request: %a :%r\n", __func__, AsciiUri, Status));
    if (!EFI_ERROR (Status) || (RetryCount >= Private->RetrySetting.MaximumRetryPut)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (!RedfishRetryRequired (Response->StatusCode)) {
        break;
      }

      //
      // Release response for next round of request.
      //
      This->FreeResponse (This, Response);
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishPutToUri failed, retry (%d/%d)\n", __func__, RetryCount, Private->RetrySetting.MaximumRetryPut));
    if (Private->RetrySetting.RetryWait > 0) {
      gBS->Stall (Private->RetrySetting.RetryWait);
    }
  } while (TRUE);

  //
  // Redfish resource is updated. Automatically expire the cached response
  // so application can directly get resource from Redfish service again.
  //
  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Resource is updated, expire URI: %s\n", __func__, Uri));
  RedfishHttpExpireResponse (This, Uri);

  if (EFI_ERROR (Status)) {
    DEBUG_CODE (
      DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
      );
    //
    // Report status code for Redfish failure
    //
    ReportHttpError (HttpMethodPut, AsciiUri, Response->StatusCode);
    DEBUG ((DEBUG_ERROR, "%a: put %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, Private->RetrySetting.MaximumRetryGet, Status));
    goto ON_RELEASE;
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

/**
  Perform HTTP POST to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  This          Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to post.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
EFIAPI
RedfishHttpPostResource (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_SERVICE              Service,
  IN  EFI_STRING                   Uri,
  IN  CHAR8                        *Content,
  OUT REDFISH_RESPONSE             *Response
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *AsciiUri;
  UINTN                       RetryCount;
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Content)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Post URI: %s\n", __func__, Uri));

  Private    = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  AsciiUri   = NULL;
  RetryCount = 0;
  ZeroMem (Response, sizeof (REDFISH_RESPONSE));

  AsciiUri = StringUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Patch resource to redfish service.
  //
  do {
    RetryCount += 1;
    Status      = RedfishPostToUri (
                    Service,
                    AsciiUri,
                    Content,
                    0,
                    NULL,
                    Response
                    );
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: HTTP request: %a :%r\n", __func__, AsciiUri, Status));
    if (!EFI_ERROR (Status) || (RetryCount >= Private->RetrySetting.MaximumRetryPut)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (!RedfishRetryRequired (Response->StatusCode)) {
        break;
      }

      //
      // Release response for next round of request.
      //
      This->FreeResponse (This, Response);
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishPostToUri failed, retry (%d/%d)\n", __func__, RetryCount, Private->RetrySetting.MaximumRetryPut));
    if (Private->RetrySetting.RetryWait > 0) {
      gBS->Stall (Private->RetrySetting.RetryWait);
    }
  } while (TRUE);

  //
  // Redfish resource is updated. Automatically expire the cached response
  // so application can directly get resource from Redfish service again.
  //
  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Resource is updated, expire URI: %s\n", __func__, Uri));
  RedfishHttpExpireResponse (This, Uri);

  if (EFI_ERROR (Status)) {
    DEBUG_CODE (
      DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
      );
    //
    // Report status code for Redfish failure
    //
    ReportHttpError (HttpMethodPost, AsciiUri, Response->StatusCode);
    DEBUG ((DEBUG_ERROR, "%a: post %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, Private->RetrySetting.MaximumRetryGet, Status));
    goto ON_RELEASE;
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

/**
  Perform HTTP DELETE to delete redfish resource on given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  This          Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
EFIAPI
RedfishHttpDeleteResource (
  IN  EDKII_REDFISH_HTTP_PROTOCOL  *This,
  IN  REDFISH_SERVICE              Service,
  IN  EFI_STRING                   Uri,
  OUT REDFISH_RESPONSE             *Response
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *AsciiUri;
  UINTN                       RetryCount;
  REDFISH_HTTP_CACHE_PRIVATE  *Private;

  if ((This == NULL) || (Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Delete URI: %s\n", __func__, Uri));

  Private    = REDFISH_HTTP_CACHE_PRIVATE_FROM_THIS (This);
  AsciiUri   = NULL;
  RetryCount = 0;
  ZeroMem (Response, sizeof (REDFISH_RESPONSE));

  AsciiUri = StringUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Patch resource to redfish service.
  //
  do {
    RetryCount += 1;
    Status      = RedfishDeleteByUri (
                    Service,
                    AsciiUri,
                    Response
                    );
    DEBUG ((REDFISH_HTTP_CACHE_DEBUG_REQUEST, "%a: HTTP request: %a :%r\n", __func__, AsciiUri, Status));
    if (!EFI_ERROR (Status) || (RetryCount >= Private->RetrySetting.MaximumRetryDelete)) {
      break;
    }

    //
    // Retry when BMC is not ready.
    //
    if ((Response->StatusCode != NULL)) {
      DEBUG_CODE (
        DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
        );

      if (!RedfishRetryRequired (Response->StatusCode)) {
        break;
      }

      //
      // Release response for next round of request.
      //
      This->FreeResponse (This, Response);
    }

    DEBUG ((DEBUG_WARN, "%a: RedfishDeleteByUri failed, retry (%d/%d)\n", __func__, RetryCount, Private->RetrySetting.MaximumRetryDelete));
    if (Private->RetrySetting.RetryWait > 0) {
      gBS->Stall (Private->RetrySetting.RetryWait);
    }
  } while (TRUE);

  //
  // Redfish resource is updated. Automatically expire the cached response
  // so application can directly get resource from Redfish service again.
  //
  DEBUG ((REDFISH_HTTP_CACHE_DEBUG, "%a: Resource is updated, expire URI: %s\n", __func__, Uri));
  RedfishHttpExpireResponse (This, Uri);

  if (EFI_ERROR (Status)) {
    DEBUG_CODE (
      DumpRedfishResponse (NULL, DEBUG_ERROR, Response);
      );
    //
    // Report status code for Redfish failure
    //
    ReportHttpError (HttpMethodDelete, AsciiUri, Response->StatusCode);
    DEBUG ((DEBUG_ERROR, "%a: delete %a failed (%d/%d): %r\n", __func__, AsciiUri, RetryCount, Private->RetrySetting.MaximumRetryGet, Status));
    goto ON_RELEASE;
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  return Status;
}

EDKII_REDFISH_HTTP_PROTOCOL  mRedfishHttpProtocol = {
  EDKII_REDFISH_HTTP_PROTOCOL_REVISION,
  RedfishHttpGetResource,
  RedfishHttpPatchResource,
  RedfishHttpPutResource,
  RedfishHttpPostResource,
  RedfishHttpDeleteResource,
  RedfishHttpFreeResponse,
  RedfishHttpExpireResponse
};

/**
  Unloads an image.

  @param[in]  ImageHandle         Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS             The image has been unloaded.
  @retval EFI_INVALID_PARAMETER   ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishHttpDriverUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  if (mRedfishHttpCachePrivate == NULL) {
    return EFI_SUCCESS;
  }

  if (!IsListEmpty (&mRedfishHttpCachePrivate->CacheList.Head)) {
    ReleaseCacheList (&mRedfishHttpCachePrivate->CacheList);
  }

  gBS->UninstallMultipleProtocolInterfaces (
         ImageHandle,
         &gEdkIIRedfishHttpProtocolGuid,
         &mRedfishHttpCachePrivate->Protocol,
         NULL
         );

  FreePool (mRedfishHttpCachePrivate);
  mRedfishHttpCachePrivate = NULL;

  return EFI_SUCCESS;
}

/**
  Main entry for this driver.

  @param[in] ImageHandle     Image handle this driver.
  @param[in] SystemTable     Pointer to SystemTable.

  @retval EFI_SUCCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
RedfishHttpEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  if (mRedfishHttpCachePrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mRedfishHttpCachePrivate = AllocateZeroPool (sizeof (REDFISH_HTTP_CACHE_PRIVATE));
  if (mRedfishHttpCachePrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initial cache list and protocol instance.
  //
  mRedfishHttpCachePrivate->ImageHandle = ImageHandle;
  CopyMem (&mRedfishHttpCachePrivate->Protocol, &mRedfishHttpProtocol, sizeof (EDKII_REDFISH_HTTP_PROTOCOL));
  mRedfishHttpCachePrivate->CacheList.Capacity = REDFISH_HTTP_CACHE_LIST_SIZE;
  mRedfishHttpCachePrivate->CacheList.Count    = 0x00;
  mRedfishHttpCachePrivate->CacheDisabled      = PcdGetBool (PcdHttpCacheDisabled);
  InitializeListHead (&mRedfishHttpCachePrivate->CacheList.Head);

  //
  // Get retry settings
  //
  mRedfishHttpCachePrivate->RetrySetting.MaximumRetryGet    = PcdGet16 (PcdHttpGetRetry);
  mRedfishHttpCachePrivate->RetrySetting.MaximumRetryPut    = PcdGet16 (PcdHttpPutRetry);
  mRedfishHttpCachePrivate->RetrySetting.MaximumRetryPatch  = PcdGet16 (PcdHttpPatchRetry);
  mRedfishHttpCachePrivate->RetrySetting.MaximumRetryPost   = PcdGet16 (PcdHttpPostRetry);
  mRedfishHttpCachePrivate->RetrySetting.MaximumRetryDelete = PcdGet16 (PcdHttpDeleteRetry);
  mRedfishHttpCachePrivate->RetrySetting.RetryWait          = PcdGet16 (PcdHttpRetryWaitInSecond) * 1000000U;

  //
  // Install the gEdkIIRedfishHttpProtocolGuid onto Handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mRedfishHttpCachePrivate->ImageHandle,
                  &gEdkIIRedfishHttpProtocolGuid,
                  &mRedfishHttpCachePrivate->Protocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot install Redfish http protocol: %r\n", __func__, Status));
    RedfishHttpDriverUnload (ImageHandle);
    return Status;
  }

  return EFI_SUCCESS;
}
