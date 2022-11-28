/** @file
  This file defines the EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL interface.

  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL_H_
#define EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL_H_

#include <Uefi.h>

typedef struct _EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL;

//
// The definition of REDFISH_SCHEMA_INFO
//
typedef struct {
  CHAR8    *Uri;
  CHAR8    *Schema;
  CHAR8    *Version;
} REDFISH_RESOURCE_SCHEMA_INFO;

/**
  Provide redfish resource with addendum data in given schema.

  @param[in]      This             Pointer to EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL instance.
  @param[in]      SchemaInfo       Redfish schema information.
  @param[in,out]  Json             On input, this is the Redfish data in given Uri in JSON object.
                                   On output, This is the Redfish data with addendum information
                                   which platform would like to add to Redfish service.

  @retval EFI_SUCCESS              Addendum data is attached.
  @retval EFI_UNSUPPORTED          No addendum data is required in given schema.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_ADDENDUM_PROVISIONING)(
  IN     EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL  *This,
  IN     REDFISH_RESOURCE_SCHEMA_INFO              *SchemaInfo,
  IN OUT EDKII_JSON_VALUE                          Json
  );

/**
  Provide redfish OEM resource with given schema information.

  @param[in]   This             Pointer to EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL instance.
  @param[in]   SchemaInfo       Redfish schema information.
  @param[out]  Json             This is the Redfish data which is attached to OEM object in given
                                schema.

  @retval EFI_SUCCESS              OEM data is attached.
  @retval EFI_UNSUPPORTED          No OEM data is required in given schema.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_RESOURCE_ADDENDUM_OEM)(
  IN   EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL  *This,
  IN   REDFISH_RESOURCE_SCHEMA_INFO              *SchemaInfo,
  OUT  EDKII_JSON_VALUE                          Json
  );

//
// The definition of _EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL
//
struct _EDKII_REDFISH_RESOURCE_ADDENDUM_PROTOCOL {
  UINT64                                          Revision;
  EDKII_REDFISH_RESOURCE_ADDENDUM_OEM             OemCallback;
  EDKII_REDFISH_RESOURCE_ADDENDUM_PROVISIONING    ProvisioningCallback;
};

extern EFI_GUID  gEdkIIRedfishResourceAddendumProtocolGuid;

#endif
