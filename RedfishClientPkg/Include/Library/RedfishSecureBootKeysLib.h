/** @file
  This file defines the Redfish secure boot keys library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_SECURE_BOOT_KEYS_LIB_H_
#define REDFISH_SECURE_BOOT_KEYS_LIB_H_

#include <Uefi.h>
#include <Protocol/EdkIIRedfishSecureBootKeysProtocol.h>

/**
  Open Redfish secure boot key with given key name. RedfishSecureBootKey is returned
  if Redfish secure boot key is opened successfully. If any error is returned,
  RedfishSecureBootKey is set to NULL.

  @param[in]   SecureBootKeyname    Secure boot key name.
  @param[in]   SecureBootUri        Redfish URI to given secure boot key.
  @param[out]  RedfishSecureBootKey Pointer to opened Redfish secure boot key.

  @retval EFI_SUCCESS           Redfish secure boot key is opened successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyOpen (
  IN  EFI_STRING               SecureBootKeyname,
  IN  CHAR8                    *SecureBootUri,
  OUT REDFISH_SECURE_BOOT_KEY  **RedfishSecureBootKey
  );

/**
  Close Redfish secure boot key with given key handler. After this call,
  RedfishSecureBootKey is no longer usable.

  @param[in]   SecureBootKeyname    Secure boot key name.

  @retval EFI_SUCCESS           Redfish secure boot key is closed successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyClose (
  IN REDFISH_SECURE_BOOT_KEY  *RedfishSecureBootKey
  );

/**
  Release the resource of KeyList. After this function is returned, KeyList
  is no longer available.

  @param[in] KeyList              KeyList to release.

  @retval EFI_SUCCESS           Redfish secure boot key list is released successfully.
  @retval Others                Error occurs.

**/
VOID
RedfishSecureBootKeyFreeKeyList (
  IN REDFISH_SECURE_BOOT_KEY_LIST  *KeyList
  );

/**
  Get secure boot key list with given type. It's call responsibility to release KeyList
  by calling RedfishSecureBootKeyFreeKeys().

  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  GetCertificate       KeyList is in certificate type of key when it is TRUE
                                   KeyList is in signature type of key when it is FALSE
  @param[out] KeyList              Returned KeyList

  @retval EFI_SUCCESS           Redfish secure boot key is returned successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyGetKeyList (
  IN REDFISH_SECURE_BOOT_KEY        *RedfishSecureBootKey,
  IN BOOLEAN                        GetCertificate,
  OUT REDFISH_SECURE_BOOT_KEY_LIST  **KeyList
  );

/**
  Update key URI to given key ID.

  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  KeyId                Key ID number.
  @param[in]  UpdateCertificate    Updated key is in certificate type.
  @param[in]  KeyUri               URI of key to update.

  @retval EFI_SUCCESS           Update key URI successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyUpdateKey (
  IN REDFISH_SECURE_BOOT_KEY  *RedfishSecureBootKey,
  IN UINT16                   KeyId,
  IN BOOLEAN                  UpdateCertificate,
  IN EFI_STRING               KeyUri
  );

/**
  Reset secure boot database key state. All keys are addition keys to
  secure boot database. When secure boot database collection is cleared
  at BMC, call this function to have fresh start.

  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  ResetCertificate     TRUE to reset certificate keys.
                                   FALSE to reset signature keys.

  @retval EFI_SUCCESS           Keys are reset successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyReset (
  IN REDFISH_SECURE_BOOT_KEY  *RedfishSecureBootKey,
  IN BOOLEAN                  ResetCertificate
  );

/**
  This function remove secure boot key variable specified by KeyName

  @param[in]  KeyName           The name of secure boot key variable.

  @retval EFI_SUCCESS           Keys are deleted successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootDeleteAllKeys (
  IN EFI_STRING  KeyName
  );

/**
  This function remove secure boot key variable specified by KeyName and load keys from
  secure boot default key variable.

  @param[in]  KeyName           The name of secure boot key variable.

  @retval EFI_SUCCESS           Keys are reset successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootResetAllKeys (
  IN EFI_STRING  KeyName
  );

/**
  This function enroll secure boot key specified by KeyName, KeyType and KeyString.

  @param[in]  KeyName           The name of secure boot key variable.
  @param[in]  KeyType           The type of this key.
  @param[in]  KeyString         The key in ASCII string format.

  @retval EFI_SUCCESS           Key is enrolled successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootEnrollKey (
  IN EFI_STRING  KeyName,
  IN CHAR8       *KeyType,
  IN CHAR8       *KeyString
  );

/**
  This function delete secure boot key specified by KeyUri in database. The corresponding
  secure boot key in variable will be deleted.

  @param[in]  KeyName           The name of secure boot key variable.
  @param[in]  KeyUri            The URI of this key to delete.

  @retval EFI_SUCCESS           Key is deleted successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootDeleteKey (
  IN EFI_STRING  KeyName,
  IN CHAR8       *KeyUri
  );

#endif
