/** @file
  This file defines the EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL interface.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_RESOURCE_CONFIG_H_
#define EDKII_REDFISH_RESOURCE_CONFIG_H_

typedef struct _EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL;

/**
  Provising redfish resource by given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[in]   Uri                 Target URI to create resource.
  @param[in]   HttpPostMode        TRUE if resource does not exist, HTTP POST method is used.
                                   FALSE if the resource exist but some of properties are missing,
                                   HTTP PUT method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_PROVISIONING) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  IN     EFI_STRING                              Uri,
  IN     BOOLEAN                                 HttpPostMode
  );

/**
  Consume resource from given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_CONSUME) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  IN     EFI_STRING                              Uri
  );


/**
  Update resource to given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_UPDATE) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  IN     EFI_STRING                              Uri
  );


/**
  Check resource on given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_CHECK) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  IN     EFI_STRING                              Uri
  );

/**
  Identify resource on given URI.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              This is target resource which we want to handle.
  @retval EFI_UNSUPPORTED          This is not the target resource.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_IDENTIFY) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  IN     EFI_STRING                              Uri
  );

//
// definition of REDFISH_SCHEMA_INFO
//
#define REDFISH_SCHEMA_STRING_SIZE  (FixedPcdGet32 (PcdMaxRedfishSchemaStringSize))
#define REDFISH_SCHEMA_VERSION_SIZE (FixedPcdGet32 (PcdMaxRedfishSchemaVersionSize))

typedef struct _SCHEMA_INFO {
  CHAR8        Schema[REDFISH_SCHEMA_STRING_SIZE];
  CHAR8        Major[REDFISH_SCHEMA_VERSION_SIZE];
  CHAR8        Minor[REDFISH_SCHEMA_VERSION_SIZE];
  CHAR8        Errata[REDFISH_SCHEMA_VERSION_SIZE];
} REDFISH_SCHEMA_INFO;

/**
  Get information about this protocol.

  @param[in]   This                Pointer to EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL instance.
  @param[out]  Info                The schema information.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_GET_INFO) (
  IN     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *This,
  OUT    REDFISH_SCHEMA_INFO                     *Info
  );

struct _EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL {
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_PROVISIONING Provisioning;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_CONSUME      Consume;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_UPDATE       Update;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_CHECK        Check;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_IDENTIFY     Identify;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL_GET_INFO     GetInfo;
};

extern EFI_GUID gEdkIIRedfishResourceConfigProtocolGuid;

#endif
