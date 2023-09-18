/** @file
  Redfish HTTP cache library helps Redfish application to get Redfish resource
  from BMC with cache mechanism enabled.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/RedfishHttpLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EDKII_REDFISH_HTTP_PROTOCOL  *mRedfishHttpProtocol = NULL;

/**
  This function free resources in Response. Response is no longer available
  after this function returns successfully.

  @param[in]  Response     HTTP response to be released.

  @retval     EFI_SUCCESS     Resrouce is released successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpFreeResource (
  IN  REDFISH_RESPONSE  *Response
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->FreeResponse (
                                 mRedfishHttpProtocol,
                                 Response
                                 );
}

/**
  Get redfish resource from given resource URI with cache mechanism
  supported. It's caller's responsibility to Response by calling
  RedfishHttpFreeResource ().

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
  IN  REDFISH_SERVICE   Service,
  IN  EFI_STRING        Uri,
  OUT REDFISH_RESPONSE  *Response,
  IN  BOOLEAN           UseCache
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->GetResource (
                                 mRedfishHttpProtocol,
                                 Service,
                                 Uri,
                                 Response,
                                 UseCache
                                 );
}

/**
  This function expire the cached response of given URI.

  @param[in]  Uri          Target response of URI.

  @retval     EFI_SUCCESS     Target response is expired successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpExpireResponse (
  IN  EFI_STRING  Uri
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->ExpireResponse (
                                 mRedfishHttpProtocol,
                                 Uri
                                 );
}

/**
  Perform HTTP PATCH to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to patch.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpPatchResource (
  IN  REDFISH_SERVICE   Service,
  IN  EFI_STRING        Uri,
  IN  CHAR8             *Content,
  OUT REDFISH_RESPONSE  *Response
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->PatchResource (
                                 mRedfishHttpProtocol,
                                 Service,
                                 Uri,
                                 Content,
                                 Response
                                 );
}

/**
  Perform HTTP PUT to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to put.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpPutResource (
  IN  REDFISH_SERVICE   Service,
  IN  EFI_STRING        Uri,
  IN  CHAR8             *Content,
  OUT REDFISH_RESPONSE  *Response
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->PutResource (
                                 mRedfishHttpProtocol,
                                 Service,
                                 Uri,
                                 Content,
                                 Response
                                 );
}

/**
  Perform HTTP POST to send redfish resource to given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[in]  Content       Data to post.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpPostResource (
  IN  REDFISH_SERVICE   Service,
  IN  EFI_STRING        Uri,
  IN  CHAR8             *Content,
  OUT REDFISH_RESPONSE  *Response
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->PostResource (
                                 mRedfishHttpProtocol,
                                 Service,
                                 Uri,
                                 Content,
                                 Response
                                 );
}

/**
  Perform HTTP DELETE to delete redfish resource on given resource URI.
  It's caller's responsibility to free Response by calling FreeResponse ().

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  Uri           Target resource URI.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishHttpDeleteResource (
  IN  REDFISH_SERVICE   Service,
  IN  EFI_STRING        Uri,
  OUT REDFISH_RESPONSE  *Response
  )
{
  if (mRedfishHttpProtocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishHttpProtocol->DeleteResource (
                                 mRedfishHttpProtocol,
                                 Service,
                                 Uri,
                                 Response
                                 );
}

/**
  Callback function when gEdkIIRedfishHttpProtocolGuid is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
RedfishHttpProtocolIsReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS  Status;

  if (mRedfishHttpProtocol != NULL) {
    gBS->CloseEvent (Event);
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEdkIIRedfishHttpProtocolGuid,
                  NULL,
                  (VOID **)&mRedfishHttpProtocol
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent (Event);
}

/**

  Initial HTTP library instance.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCCESS  Initial library successfully.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishHttpConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID  *Registration;

  EfiCreateProtocolNotifyEvent (
    &gEdkIIRedfishHttpProtocolGuid,
    TPL_CALLBACK,
    RedfishHttpProtocolIsReady,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}
