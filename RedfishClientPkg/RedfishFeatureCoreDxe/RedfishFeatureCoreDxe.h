/** @file
  Definitions of RedfishFeatureCoreDxe

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_FEATURE_CORE_DXE_H_
#define EDKII_REDFISH_FEATURE_CORE_DXE_H_

#include <Protocol/EdkIIRedfishFeature.h>
#include <Protocol/EdkIIRedfishOverrideProtocol.h>

#include <Base.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/RedfishEventLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/ReportStatusCodeLib.h>

#define MaxNodeNameLength             64
#define MaxParentUriLength            512
#define NodeSeperator                 L'/'
#define NodeSeparatorStr              L"/"
#define UriSeperator                  L';'
#define NodeIsCollectionLeftBracket   L'{'
#define NodeIsCollectionRightBracket  L'}'
#define NodeIsCollectionSymbol        L"/{}"
#define REDFISH_FEATURE_CORE_TPL      TPL_CALLBACK
#define REDFISH_TASK_SERVICE_URI      L"TaskService"
#define REDFISH_INTERNAL_ERROR        "Redfish service failure. Configuration at BMC may not be update-to-date."
#define REDFISH_COMMUNICATION_ERROR   "Redfish communication failure. Configuration at BMC may not be update-to-date."
#define REDFISH_CONFIG_CHANGED        "System configuration is changed from RESTful interface. System reboot."

typedef struct _REDFISH_FEATURE_INTERNAL_DATA REDFISH_FEATURE_INTERNAL_DATA;
struct _REDFISH_FEATURE_INTERNAL_DATA {
  REDFISH_FEATURE_INTERNAL_DATA    *SiblingList;         ///< Next same level in hierarchy of resource URI.
  REDFISH_FEATURE_INTERNAL_DATA    *ChildList;           ///< Next level in hierarchy of resource URI.
  EFI_STRING                       NodeName;             ///< Name of the node in hierarchy of resource URI.
  REDFISH_FEATURE_CALLBACK         Callback;             ///< Callback function of Redfish feature driver.
  VOID                             *Context;             ///< Context of feature driver.
  RESOURCE_INFORMATION_EXCHANGE    *InformationExchange; ///< Information returned from Redfish feature driver.
  UINT32                           Flags;
};

#define REDFISH_FEATURE_INTERNAL_DATA_IS_COLLECTION  0x00000001

typedef struct {
  EDKII_REDFISH_FEATURE_PROTOCOL    *This;
  FEATURE_CALLBACK_ACTION           Action;
} REDFISH_FEATURE_STARTUP_CONTEXT;
#endif
