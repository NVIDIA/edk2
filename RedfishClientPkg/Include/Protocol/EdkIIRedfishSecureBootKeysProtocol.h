/** @file
  This file defines the EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL_H_
#define EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL_H_

#include <Uefi.h>
#include <Guid/ImageAuthentication.h>

typedef struct _EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL;
typedef VOID                                             *REDFISH_SECURE_BOOT_KEY;

///
/// Definition of REDFISH_SECURE_BOOT_KEY_RECORD
///
typedef struct {
  UINT16    Id;             ///< The ID of this secure boot key.
  CHAR8     *Uri;           ///< The URI of secure boot key in secure boot database.
  CHAR8     *KeyString;     ///< Key string posted to secure boot database.
  CHAR8     *TypeString;    ///< Key type string posted to secure boot database.
  CHAR8     *Owner;         ///< GUID string posted to secure boot databases.
} REDFISH_SECURE_BOOT_KEY_RECORD;

///
/// Definition of REDFISH_SECURE_BOOT_KEY_LIST
///
typedef struct {
  BOOLEAN                           IsCertificate;    ///< This is certificate type of key if it is TRUE. FALSE when it is signature.
  UINTN                             TotalKeyCount;    ///< The total number of secure boot key.
  UINTN                             AdditionCount;    ///< The number of key in Addition list.
  UINTN                             SubtractionCount; ///< The number of key in subtraction list.
  REDFISH_SECURE_BOOT_KEY_RECORD    *AdditionList;    ///< Array of key which is addition to secure boot key database.
  REDFISH_SECURE_BOOT_KEY_RECORD    *SubtractionList; ///< Array of key which is subtraction to secure boot key database.
} REDFISH_SECURE_BOOT_KEY_LIST;

/**
  Open Redfish secure boot key with given key name. RedfishSecureBootKey is returned
  if Redfish secure boot key is opened successfully. If any error is returned,
  RedfishSecureBootKey is set to NULL.

  @param[in]   This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]   SecureBootKeyname    Secure boot key name.
  @param[in]   SecureBootUri        Redfish URI to given secure boot key.
  @param[out]  RedfishSecureBootKey Pointer to opened Redfish secure boot key.

  @retval EFI_SUCCESS           Redfish secure boot key is opened successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_OPEN)(
  IN  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN  EFI_STRING                               SecureBootKeyName,
  IN  CHAR8                                    *SecureBootUri,
  OUT REDFISH_SECURE_BOOT_KEY                  **RedfishSecureBootKey
  );

/**
  Close Redfish secure boot key with given key handler. After this call,
  RedfishSecureBootKey is no longer usable.

  @param[in]   This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]   RedfishSecureBootKey Secure boot key handle.

  @retval EFI_SUCCESS           Redfish secure boot key is closed successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_CLOSE)(
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey
  );

/**
  Release the resource of KeyList. After this function is returned, KeyList
  is no longer available.

  @param[in] This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in] KeyList              KeyList to release.

  @retval EFI_SUCCESS           Redfish secure boot key list is released successfully.
  @retval Others                Error occurs.

**/
typedef
VOID
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_FREE_KEY_LIST)(
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY_LIST             *KeyList
  );

/**
  Get secure boot key list with given type. It's call responsibility to release KeyList
  by calling FreeKeyList().

  @param[in]  This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  GetCertificate       KeyList is in certificate type of key when it is TRUE
                                   KeyList is in signature type of key when it is FALSE
  @param[out] KeyList              Returned KeyList

  @retval EFI_SUCCESS           Redfish secure boot key is returned successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_GET_KEY_LIST)(
  IN  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN  REDFISH_SECURE_BOOT_KEY        *RedfishSecureBootKey,
  IN  BOOLEAN                        GetCertificate,
  OUT REDFISH_SECURE_BOOT_KEY_LIST  **KeyList
  );

/**
  Update key URI to given key ID.

  @param[in]  This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  KeyId                Key ID number.
  @param[in]  UpdateCertificate    Updated key is in certificate type.
  @param[in]  KeyUri               URI of key to update.

  @retval EFI_SUCCESS           Update key URI successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_UPDATE)(
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey,
  IN UINT16                                   KeyId,
  IN BOOLEAN                                  UpdateCertificate,
  IN EFI_STRING                               KeyUri
  );

/**
  Reset secure boot database key state. All keys are addition keys to
  secure boot database. When secure boot database collection is cleared
  at BMC, call this function to have fresh start.

  @param[in]  This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  RedfishSecureBootKey Pointer to secure boot key instance.
  @param[in]  ResetCertificate     TRUE to reset certificate keys.
                                   FALSE to reset signature keys.

  @retval EFI_SUCCESS           Keys are reset successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_KEYS_RESET)(
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey,
  IN BOOLEAN                                  ResetCertificate
  );

/**
  This function remove secure boot key variable specified by KeyName

  @param[in]  This              Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]  KeyName           The name of secure boot key variable.

  @retval EFI_SUCCESS           Keys are deleted successfully.
  @retval Others                Error occurs.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_DELETE_ALL_KEYS)(
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
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_RESET_ALL_KEYS)(
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
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_ENROLL_KEY)(
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
typedef
EFI_STATUS
(EFIAPI *REDFISH_SECURE_BOOT_DELETE_KEY)(
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN EFI_STRING                               KeyName,
  IN CHAR8                                    *KeyUri
  );

///
/// Definition of _EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL.
///
struct _EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL {
  UINT32                                    Version;
  REDFISH_SECURE_BOOT_KEYS_OPEN             Open;
  REDFISH_SECURE_BOOT_KEYS_CLOSE            Close;
  REDFISH_SECURE_BOOT_KEYS_UPDATE           Update;
  REDFISH_SECURE_BOOT_KEYS_RESET            Reset;
  REDFISH_SECURE_BOOT_KEYS_FREE_KEY_LIST    FreeKeyList;
  REDFISH_SECURE_BOOT_KEYS_GET_KEY_LIST     GetKeyList;
  REDFISH_SECURE_BOOT_DELETE_ALL_KEYS       DeleteAllKeys;
  REDFISH_SECURE_BOOT_RESET_ALL_KEYS        ResetAllKeys;
  REDFISH_SECURE_BOOT_ENROLL_KEY            EnrollKey;
  REDFISH_SECURE_BOOT_DELETE_KEY            DeleteKey;
};

#define EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL_REVISION  0x00001000

extern EFI_GUID  gEdkIIRedfishSecureBootKeysProtocolGuid;

#endif
