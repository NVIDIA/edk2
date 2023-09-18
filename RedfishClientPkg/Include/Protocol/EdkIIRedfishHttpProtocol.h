/** @file
  This file defines the EDKII_REDFISH_HTTP_PROTOCOL interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_HTTP_PROTOCOL_H_
#define EDKII_REDFISH_HTTP_PROTOCOL_H_

#include <Uefi.h>
#include <Library/RedfishLib.h>

typedef struct _EDKII_REDFISH_HTTP_PROTOCOL EDKII_REDFISH_HTTP_PROTOCOL;

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
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_GET_RESOURCE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_SERVICE             Service,
  IN  EFI_STRING                  Uri,
  OUT REDFISH_RESPONSE            *Response,
  IN  BOOLEAN                     UseCache
  );

/**
  This function free resources in Response. Response is no longer available
  after this function returns successfully.

  @param[in]  This         Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Response     HTTP response to be released.

  @retval     EFI_SUCCESS     Resrouce is released successfully.
  @retval     Others          Errors occur.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_FREE_RESPONSE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_RESPONSE            *Response
  );

/**
  This function expire the cached response of given URI.

  @param[in]  This         Pointer to EDKII_REDFISH_HTTP_PROTOCOL instance.
  @param[in]  Uri          Target response of URI.

  @retval     EFI_SUCCESS     Target response is expired successfully.
  @retval     Others          Errors occur.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_EXPIRE_RESPONSE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  EFI_STRING                  Uri
  );

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
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_PATCH_RESOURCE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_SERVICE             Service,
  IN  EFI_STRING                  Uri,
  IN  CHAR8                       *Content,
  OUT REDFISH_RESPONSE            *Response
  );

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
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_PUT_RESOURCE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_SERVICE             Service,
  IN  EFI_STRING                  Uri,
  IN  CHAR8                       *Content,
  OUT REDFISH_RESPONSE            *Response
  );

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
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_POST_RESOURCE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_SERVICE             Service,
  IN  EFI_STRING                  Uri,
  IN  CHAR8                       *Content,
  OUT REDFISH_RESPONSE            *Response
  );

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
typedef
EFI_STATUS
(EFIAPI *REDFISH_HTTP_DELETE_RESOURCE)(
  IN  EDKII_REDFISH_HTTP_PROTOCOL *This,
  IN  REDFISH_SERVICE             Service,
  IN  EFI_STRING                  Uri,
  OUT REDFISH_RESPONSE            *Response
  );

///
/// Definition of _EDKII_REDFISH_HTTP_PROTOCOL.
///
struct _EDKII_REDFISH_HTTP_PROTOCOL {
  UINT32                          Version;
  REDFISH_HTTP_GET_RESOURCE       GetResource;
  REDFISH_HTTP_PATCH_RESOURCE     PatchResource;
  REDFISH_HTTP_PUT_RESOURCE       PutResource;
  REDFISH_HTTP_POST_RESOURCE      PostResource;
  REDFISH_HTTP_DELETE_RESOURCE    DeleteResource;
  REDFISH_HTTP_FREE_RESPONSE      FreeResponse;
  REDFISH_HTTP_EXPIRE_RESPONSE    ExpireResponse;
};

#define EDKII_REDFISH_HTTP_PROTOCOL_REVISION  0x00001000

extern EFI_GUID  gEdkIIRedfishHttpProtocolGuid;

#endif
