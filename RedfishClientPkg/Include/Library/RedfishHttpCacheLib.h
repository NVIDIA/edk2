/** @file
  This file defines the Redfish HTTP cache library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_HTTP_CACHE_LIB_H_
#define REDFISH_HTTP_CACHE_LIB_H_

#include <Uefi.h>
#include <Library/RedfishLib.h>

/**
  Read redfish resource by given resource URI. Returned "Response"
  will be released in library destructor so caller does not release
  it manually.

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
  OUT REDFISH_RESPONSE  **Response,
  IN  BOOLEAN           UseCache
  );

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
  );

#endif
