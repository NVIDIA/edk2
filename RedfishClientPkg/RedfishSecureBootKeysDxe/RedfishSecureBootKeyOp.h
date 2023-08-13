/** @file
  Redfish secure boot keys internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_SECURE_KEY_OP_H_
#define REDFISH_SECURE_KEY_OP_H_

#include <UefiSecureBoot.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Library/SecureBootVariableLib.h>
#include <Library/PlatformPKProtectionLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "RedfishSecureBootData.h"

#define REDFISH_SECURE_BOOT_PEM_PREFIX     "-----BEGIN CERTIFICATE-----"
#define REDFISH_SECURE_BOOT_PEM_POSTFIX    "-----END CERTIFICATE-----"
#define REDFISH_SECURE_BOOT_GUID_STR_SIZE  64

/**

  This function read secure boot key from given variable name
  and GUID. And add key information to key list.

  @param[in]  KeyInfo    Key variable information.

  @retval     EFI_SUCCESS         Key is loaded successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootLoadVariable (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo
  );

/**

  This function converts certificate from DER format to PEM format.
  It's call responsibility to release PemStr by calling FreePool().

  @param[in]  CertData      Certificate binary in DER format.
  @param[in]  CertDataSize  The size of CertData.
  @param[out] PemStr        Certificate in PEM format.

  @retval     EFI_SUCCESS   Certificate is converted to PEM format.
  @retval     Others        Errors occur.

**/
EFI_STATUS
RfSecureBootDerToPem (
  IN UINT8   *CertData,
  IN UINTN   CertDataSize,
  OUT CHAR8  **PemStr
  );

/**
  Convert GUID to ASCII string.

  @param[in]     Guid          Pointer to GUID to print.

  @retval    CHAR8 *   GUID in ASCII string returned.
  @retval    NULL      Error occurs.

**/
CHAR8 *
RfSecureBootGuidToString (
  IN  EFI_GUID  *Guid
  );

/**
  This function remove secure boot key variable specified by KeyName

  @param[in]  This              Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  KeyName           The name of secure boot key variable.

  @retval EFI_SUCCESS           Keys are deleted successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootDeleteAllKeys (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN EFI_STRING                               KeyName
  );

/**
  This function remove secure boot key variable specified by KeyName and load keys from
  secure boot default key variable.

  @param[in]  This              Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  KeyName           The name of secure boot key variable.

  @retval EFI_SUCCESS           Keys are reset successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootResetAllKeys (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN EFI_STRING                               KeyName
  );

/**
  This function enroll secure boot key specified by KeyName, KeyType and KeyString.

  @param[in]  This              Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  KeyName           The name of secure boot key variable.
  @param[in]  KeyType           The type of this key.
  @param[in]  KeyString         The key in ASCII string format.

  @retval EFI_SUCCESS           Key is enrolled successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootEnrollKey (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN EFI_STRING                               KeyName,
  IN CHAR8                                    *KeyType,
  IN CHAR8                                    *KeyString
  );

/**
  This function delete secure boot key specified by KeyUri in database. The corresponding
  secure boot key in variable will be deleted.

  @param[in]  This              Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  KeyName           The name of secure boot key variable.
  @param[in]  KeyUri            The URI of this key to delete.

  @retval EFI_SUCCESS           Key is deleted successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootDeleteKey (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN EFI_STRING                               KeyName,
  IN CHAR8                                    *KeyUri
  );

#endif
