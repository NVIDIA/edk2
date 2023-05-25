/** @file
  Redfish feature driver common header file.

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_RESOURCE_COMMON_H_
#define EFI_REDFISH_RESOURCE_COMMON_H_

#define MAX_RED_PATH_LEN  128

#include <RedfishBase.h>

//
// Libraries
//
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishVersionLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/RedfishPlatformConfigLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RedfishResourceIdentifyLib.h>
#include <Library/EdkIIRedfishResourceConfigLib.h>
#include <Library/RedfishAddendumLib.h>
#include <Library/RedfishHttpCacheLib.h>

//
// Protocols
//
#include <Protocol/EdkIIRedfishFeature.h>
#include <Protocol/EdkIIRedfishConfigHandler.h>
#include <Protocol/EdkIIRedfishResourceConfigProtocol.h>
#include <Protocol/RestJsonStructure.h>
#include <Protocol/RestEx.h>

typedef struct _REDFISH_RESOURCE_COMMON_PRIVATE {
  REDFISH_SERVICE                           RedfishService;
  EFI_REST_JSON_STRUCTURE_PROTOCOL          *JsonStructProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL    RedfishResourceConfig;
  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL     ConfigHandler;
  EFI_EVENT                                 Event;
  EFI_STRING                                Uri;
  CHAR8                                     *Json;
  REDFISH_PAYLOAD                           Payload;
  //
  //  Below are used for the external resource.
  //
  EDKII_REDFISH_FEATURE_PROTOCOL            *FeatureProtocol;
  RESOURCE_INFORMATION_EXCHANGE             *InformationExchange;
  EFI_STRING                                RedfishVersion;
} REDFISH_RESOURCE_COMMON_PRIVATE;

#define REDFISH_RESOURCE_COMMON_PRIVATE_DATA_FROM_CONFIG_PROTOCOL(This) \
          BASE_CR ((This), REDFISH_RESOURCE_COMMON_PRIVATE, ConfigHandler)

#define REDFISH_RESOURCE_COMMON_PRIVATE_DATA_FROM_RESOURCE_PROTOCOL(This) \
          BASE_CR ((This), REDFISH_RESOURCE_COMMON_PRIVATE, RedfishResourceConfig)

/**
  Consume resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.
  @param[in]   HeaderEtag          The Etag string returned in HTTP header.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishConsumeResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json,
  IN     CHAR8                            *HeaderEtag OPTIONAL
  );

/**
  Provisioning redfish resource by given URI.

  @param[in]   This                Pointer to EFI_HP_REDFISH_HII_PROTOCOL instance.
  @param[in]   ResourceExist       TRUE if resource exists, PUT method will be used.
                                   FALSE if resource does not exist POST method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishProvisioningResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     BOOLEAN                          ResourceExist
  );

/**
  Check resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.
  @param[in]   HeaderEtag          The Etag string returned in HTTP header.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishCheckResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json,
  IN     CHAR8                            *HeaderEtag OPTIONAL
  );

/**
  Update resource to given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishUpdateResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json
  );

/**
  Identify resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishIdentifyResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json
  );

#endif
