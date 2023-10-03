/** @file
  This file defines the EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_RESOURCE_CONFIG2_H_
#define EDKII_REDFISH_RESOURCE_CONFIG2_H_

#include <Protocol/EdkIIRedfishResourceConfigProtocol.h>

typedef struct _EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL;

/**
  Provision redfish resource by given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[in]   Uri                 Target URI to create resource.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.
  @param[in]   HttpPostMode        TRUE if resource does not exist, HTTP POST method is used.
                                   FALSE if the resource exist but some of properties are missing,
                                   HTTP PUT method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_PROVISIONING)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  IN     EFI_STRING                               Uri,
  IN     CHAR8                                    *JsonText OPTIONAL,
  IN     BOOLEAN                                  HttpPostMode
  );

/**
  Consume resource from given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_CONSUME)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  IN     EFI_STRING                               Uri,
  IN     CHAR8                                    *JsonText OPTIONAL
  );

/**
  Update resource to given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_UPDATE)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  IN     EFI_STRING                               Uri,
  IN     CHAR8                                    *JsonText OPTIONAL
  );

/**
  Check resource on given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_CHECK)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  IN     EFI_STRING                               Uri,
  IN     CHAR8                                    *JsonText OPTIONAL
  );

/**
  Identify resource on given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              This is target resource which we want to handle.
  @retval EFI_UNSUPPORTED          This is not the target resource.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_IDENTIFY)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  IN     EFI_STRING                               Uri,
  IN     CHAR8                                    *JsonText OPTIONAL
  );

/**
  Get information about this protocol.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL instance.
  @param[out]  Info                The schema information.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_GET_INFO)(
  IN     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *This,
  OUT    REDFISH_SCHEMA_INFO                     *Info
  );

struct _EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL {
  UINT32                                                  Version;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_PROVISIONING    Provisioning;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_CONSUME         Consume;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_UPDATE          Update;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_CHECK           Check;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_IDENTIFY        Identify;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_GET_INFO        GetInfo;
};

#define EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL_REVISION  0x00001000

extern EFI_GUID  gEdkIIRedfishResourceConfig2ProtocolGuid;

#endif
