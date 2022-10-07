/** @file
  Redfish feature driver collection common header file.

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_COLLECTION_COMMON_H_
#define EFI_REDFISH_COLLECTION_COMMON_H_

#include <RedfishBase.h>

//
// Libraries
//
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/EdkIIRedfishResourceConfigLib.h>
#include <Library/RedfishVersionLib.h>

//
// Protocols
//
#include <Protocol/EdkIIRedfishConfigHandler.h>
#include <Protocol/EdkIIRedfishFeature.h>
#include <Protocol/RestJsonStructure.h>
#include <Protocol/RestEx.h>

#define REDFISH_MAX_COLLECTION_INDEX_LEN  8

typedef struct _REDFISH_COLLECTION_PRIVATE {
  EFI_REST_JSON_STRUCTURE_PROTOCOL      *JsonStructProtocol;
  EDKII_REDFISH_FEATURE_PROTOCOL        *FeatureProtocol;
  REDFISH_SERVICE                       RedfishService;
  RESOURCE_INFORMATION_EXCHANGE         *InformationExchange;
  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL ConfigHandler;
  EFI_STRING                            CollectionUri;
  CHAR8                                 *CollectionJson;
  REDFISH_PAYLOAD                       CollectionPayload;
  REDFISH_RESPONSE                      RedResponse;
  EFI_STRING                            RedfishVersion;
} REDFISH_COLLECTION_PRIVATE;

#define REDFISH_COLLECTION_PRIVATE_DATA_FROM_PROTOCOL(This) \
          BASE_CR ((This), REDFISH_COLLECTION_PRIVATE, ConfigHandler)

#endif
