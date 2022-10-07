/** @file
  This file defines the EDKII_REDFISH_FEATURE_PROTOCOL interface.

  (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_ETAG_PROTOCOL_H_
#define EDKII_REDFISH_ETAG_PROTOCOL_H_

typedef struct _EDKII_REDFISH_ETAG_PROTOCOL EDKII_REDFISH_ETAG_PROTOCOL;

/**
  Get ETag string by given URI

  @param[in]   This                    Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.
  @param[in]   Uri                     Target URI to search.
  @param[out]  ETag                    The ETag string to corresponding URI.

  @retval EFI_SUCCESS                  The ETag is found successfully.
  @retval EFI_INVALID_PARAMETER        Invalid parameter is given.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_ETAG_PROTOCOL_GET) (
  IN  EDKII_REDFISH_ETAG_PROTOCOL *This,
  IN  CHAR8                       *Uri,
  OUT CHAR8                       **ETag
  );

/**
  Keep ETag string which maps to given Uri.

  @param[in]   This                Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.
  @param[in]   Uri                 The target Uri which related to ETag.
  @param[in]   ETag                The ETag to add. If ETag is NULL, the record of correspoonding URI will be removed.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_ETAG_PROTOCOL_SET) (
  IN  EDKII_REDFISH_ETAG_PROTOCOL   *This,
  IN  CHAR8                         *Uri,
  IN  CHAR8                         *ETag
  );

/**
  Refresh the ETag database and save database to variable.

  @param[in]   This                Pointer to EDKII_REDFISH_ETAG_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_ETAG_PROTOCOL_FLUSH) (
  IN  EDKII_REDFISH_ETAG_PROTOCOL    *This
  );

struct _EDKII_REDFISH_ETAG_PROTOCOL {
  EDKII_REDFISH_ETAG_PROTOCOL_GET   Get;
  EDKII_REDFISH_ETAG_PROTOCOL_SET   Set;
  EDKII_REDFISH_ETAG_PROTOCOL_FLUSH Flush;
};

extern EFI_GUID gEdkIIRedfishETagProtocolGuid;

#endif
