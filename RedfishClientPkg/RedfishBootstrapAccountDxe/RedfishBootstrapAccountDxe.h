/** @file
  Common header file for RedfishBootstrapAccountDxe driver.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_BOOTSTRAP_ACCOUNT_DXE_H_
#define REDFISH_BOOTSTRAP_ACCOUNT_DXE_H_

#include <Uefi.h>
#include <RedfishBase.h>

//
// Libraries
//
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/RedfishEventLib.h>
#include <Library/RedfishLib.h>
#include <Library/RedfishFeatureUtilityLib.h>
#include <Library/RedfishDebugLib.h>
#include <Library/RedfishVersionLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/EdkIIRedfishConfigHandler.h>
#include <Protocol/EdkIIRedfishCredential.h>
#include <Protocol/RestEx.h>

#define REDFISH_BOOTSTRAP_ACCOUNT_DEBUG         DEBUG_VERBOSE
#define REDFISH_MANAGER_ACCOUNT_COLLECTION_URI  L"AccountService/Accounts"
#define REDFISH_URI_LENGTH                      128

//
// Definitions of REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE
//
typedef struct {
  EFI_HANDLE                               ImageHandle;
  EFI_HANDLE                               RestExHandle;
  REDFISH_SERVICE                          RedfishService;
  EFI_EVENT                                RedfishEvent;
  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL    Protocol;
} REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE;

#define REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE_FROM_PROTOCOL(This) \
          BASE_CR ((This), REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE, Protocol)

#endif
