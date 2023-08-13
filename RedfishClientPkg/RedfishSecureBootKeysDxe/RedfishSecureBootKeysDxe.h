/** @file
  Redfish secure boot keys internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_SECURE_BOOT_KEYS_DXE_H_
#define REDFISH_SECURE_BOOT_KEYS_DXE_H_

#include <Uefi.h>
#include <RedfishBase.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/JsonLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Hash.h>
#include <Protocol/Hash2.h>
#include <Protocol/ServiceBinding.h>
#include <Protocol/EdkIIRedfishSecureBootKeysProtocol.h>

#define REDFISH_SECURE_BOOT_KEY_DEBUG       DEBUG_VERBOSE
#define REDFISH_SECURE_BOOT_VARIABLE_DEBUG  DEBUG_VERBOSE
#define REDFISH_SECURE_BOOT_KEY_DUMP        0x00
#define REDFISH_SECURE_BOOT_KEY_MAX_COUNT   0x200
#define REDFISH_SECURE_BOOT_PK_MAX_COUNT    0x01
#define REDFISH_SECURE_BOOT_KEY_TYPES       0x02
#define REDFISH_SECURE_BOOT_BLOB_PREFIX     L"Redfish"
#define REDFISH_SECURE_BOOT_JSON_HASH_NAME  "Hash"
#define REDFISH_SECURE_BOOT_JSON_URI_NAME   "Uri"
#define REDFISH_SECURE_BOOT_JSON_CERT_NAME  "Certificates"
#define REDFISH_SECURE_BOOT_JSON_SIG_NAME   "Signatures"

///
/// Definition of REDFISH_SECURE_BOOT_KEYS_PRIVATE
///
typedef struct {
  EFI_HANDLE                                 ImageHandle;
  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL    SecureBootKeysProtocol;
  EFI_EVENT                                  ProtocolNotify;
  VOID                                       *Registration;
  EFI_HASH2_PROTOCOL                         *Hash2Protocol;
  EFI_SERVICE_BINDING_PROTOCOL               *ServiceBinding;
  EFI_HANDLE                                 Hash2Handle;
} REDFISH_SECURE_BOOT_KEYS_PRIVATE;

#define REDFISH_SECURE_BOOT_KEYS_PRIVATE_FROM_THIS(a)  BASE_CR (a, REDFISH_SECURE_BOOT_KEYS_PRIVATE, SecureBootKeysProtocol)

#endif
