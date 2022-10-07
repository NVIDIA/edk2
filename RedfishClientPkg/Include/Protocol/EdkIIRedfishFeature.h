/** @file
  This file defines the EDKII_REDFISH_FEATURE_PROTOCOL interface.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_FEATURE_H_
#define EDKII_REDFISH_FEATURE_H_

#include <Protocol/EdkIIRedfishInterchangeData.h>

typedef struct _EDKII_REDFISH_FEATURE_PROTOCOL EDKII_REDFISH_FEATURE_PROTOCOL;

#define EDKII_REDFISH_FEATURE_PROTOCOL_GUID \
    {  \
      0x785CC694, 0x4930, 0xEFBF, { 0x2A, 0xCB, 0xA4, 0xB6, 0xA1, 0xCC, 0xAA, 0x34 }  \
    }

typedef enum {
  CallbackActionNone = 0,       ///< Invalid action
  CallbackActionStartOperation, ///< Start the operations on Redfish resource
  CallbackActionMax
} FEATURE_CALLBACK_ACTION;

/**
  The callback function provided by Redfish Feature driver.

  @param[in]     This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]     FeatureAction       The action Redfish feature driver should take.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] ExchangeInformation The pointer to RESOURCE_INFORMATION_EXCHANGE.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_FEATURE_CALLBACK) (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL *This,
  IN     FEATURE_CALLBACK_ACTION        FeatureAction,
  IN     VOID                           *Context,
  IN OUT RESOURCE_INFORMATION_EXCHANGE  *ExchangeInformation
);
/**
  The registration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]   FeatureManagedUri   The URI represents the hierarchy path of the Redfish
                                   resource in the entire Redfish data model that managed
                                   by Redfish feature driver . Each node in the hierarchy
                                   path is the property name defined in the schema of the
                                   resource.
  @param[in]   Callback            Callback routine associated with this registration that
                                   provided by Redfish feature driver to execute the action
                                   on Redfish resource which is managed by this Redfish
                                   feature driver.
  @param[in]   Context             The context of the registering feature driver. The pointer
                                   to the conext is delivered through callback function.
  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_FEATURE_REGISTER) (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN     EFI_STRING                      FeatureManagedUri,
  IN     REDFISH_FEATURE_CALLBACK        Callback,
  IN     VOID                            *Context
);

/**
  The unregistration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]   FeatureManagedUri   The URI represents the hierarchy path of the Redfish
                                   resource in the entire Redfish data model that managed
                                   by Redfish feature driver . Each node in the hierarchy
                                   path is the property name defined in the schema of the
                                   resource.
  @param[in]   Context             The context used for the previous feature driver
                                   registration.
  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_FEATURE_UNREGISTER) (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN     EFI_STRING                      FeatureManagedUri,
  IN     VOID                            *Context
);

struct _EDKII_REDFISH_FEATURE_PROTOCOL {
  REDFISH_FEATURE_REGISTER      Register;
  REDFISH_FEATURE_UNREGISTER    Unregister;
};

extern EFI_GUID gEdkIIRedfishFeatureProtocolGuid;

#endif
