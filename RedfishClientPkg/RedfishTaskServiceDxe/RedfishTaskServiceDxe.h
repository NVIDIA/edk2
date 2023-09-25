/** @file
  Definitions of RedfishTaskServiceDxe

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_TASK_SERVICE_DXE_H_
#define EDKII_REDFISH_TASK_SERVICE_DXE_H_

#include <Base.h>
#include <RedfishBase.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/RedfishMessageLib.h>
#include <Library/RedfishEventLib.h>
#include <Library/RedfishVersionLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishTaskLib.h>
#include <Library/RedfishProtocolFeaturesLib.h>
#include <Library/JsonLib.h>

#include <Protocol/EdkIIRedfishTaskProtocol.h>
#include <Protocol/EdkIIRedfishConfigHandler.h>
#include <Protocol/EdkIIRedfishFeature.h>

#define REDFISH_TASK_DEBUG           DEBUG_VERBOSE
#define REDFISH_TASK_SERVICE_URI     L"TaskService"
#define REDFISH_TASK_COLLECTION_URI  L"Tasks"
#define MAX_URI_LENGTH               256
#define REDFISH_ATTRIBUTE_MEMBERS    "Members"

///
/// Definition of REDFISH_TASK_SERVICE_PRIVATE
//
typedef struct {
  EFI_HANDLE                               ImageHandle;
  EDKII_REDFISH_TASK_PROTOCOL              TaskProtocol;
  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL    ConfigHandler;
  EDKII_REDFISH_FEATURE_PROTOCOL           *FeatureProtocol;
  REDFISH_SERVICE                          RedfishService;
  UINTN                                    RegisteredCount;
  LIST_ENTRY                               RegisteredList;
  UINTN                                    TaskCount;
  LIST_ENTRY                               TaskList;
  EFI_EVENT                                RedfishEvent;
} REDFISH_TASK_SERVICE_PRIVATE;

#define REDFISH_TASK_SERVICE_PRIVATE_FROM_THIS(a)             BASE_CR (a, REDFISH_TASK_SERVICE_PRIVATE, TaskProtocol)
#define REDFISH_TASK_SERVICE_PRIVATE_FROM_CONFIG_PROTOCOL(a)  BASE_CR (a, REDFISH_TASK_SERVICE_PRIVATE, ConfigHandler)

#endif
