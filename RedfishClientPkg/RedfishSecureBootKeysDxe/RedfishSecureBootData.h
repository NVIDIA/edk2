/** @file
  Redfish secure boot keys internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_SECURE_BOOT_DATA_H_
#define REDFISH_SECURE_BOOT_DATA_H_

#include <Uefi.h>
#include <Library/RedfishJsonBlobLib.h>
#include <Protocol/EdkIIRedfishSecureBootKeysProtocol.h>

#define REDFISH_SECURE_BOOT_KEY_SIGNATURE   SIGNATURE_32 ('R', 'F', 'S', 'K')
#define REDFISH_SECURE_BOOT_HASH_ALGORITHM  &gEfiHashAlgorithmSha256Guid
#define REDFISH_SECURE_BOOT_GET_KEY_LIST(a, b)  (b ? &((a)->Certificates) : &((a)->Signatures));

//
// Signature Type supported in UEFI specification 2.10
//
#define REDFISH_SECURE_BOOT_KEY_TYPE_SHA256          "EFI_CERT_SHA256_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048         "EFI_CERT_RSA2048_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048_SHA256  "EFI_CERT_RSA2048_SHA256_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_SHA1            "EFI_CERT_SHA1_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048_SHA1    "EFI_CERT_RSA2048_SHA1_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_X509            "PEM"
#define REDFISH_SECURE_BOOT_KEY_TYPE_SHA224          "EFI_CERT_SHA224_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_SHA384          "EFI_CERT_SHA384_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_SHA512          "EFI_CERT_SHA512_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA256     "EFI_CERT_X509_SHA256_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA384     "EFI_CERT_X509_SHA384_GUID"
#define REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA512     "EFI_CERT_X509_SHA512_GUID"
#define REDFISH_SECURE_BOOT_KEY_STR_LENGTH           256
#define REDFISH_SECURE_BOOT_KEY_STR_LENGTH_PER_LINE  16

///
/// Definition of REDFISH_SECURE_BOOT_KEY_STATE
///
typedef enum {
  RedfishSecureBootKeyStatePresented = 0,
  RedfishSecureBootKeyStateAddition,
  RedfishSecureBootKeyStateSubtraction,
  RedfishSecureBootKeyStateMax
} REDFISH_SECURE_BOOT_KEY_STATE;

///
/// Definition of REDFISH_SECURE_BOOT_KEY_TYPE
///
typedef enum {
  RedfishSecureBootKeyPkDefault = 0x00,
  RedfishSecureBootKeyKekDefault,
  RedfishSecureBootKeyDbDefault,
  RedfishSecureBootKeyDbtDefault,
  RedfishSecureBootKeyDbxDefault,
  RedfishSecureBootKeyDbrDefault,
  RedfishSecureBootKeyPk,
  RedfishSecureBootKeyKek,
  RedfishSecureBootKeyDb,
  RedfishSecureBootKeyDbt,
  RedfishSecureBootKeyDbx,
  RedfishSecureBootKeyDbr,
} REDFISH_SECURE_BOOT_KEY_TYPE;

///
/// Definition of SECURE_BOOT_KEY_VARIABLE_INFO
///
typedef struct {
  EFI_STRING                      VariableName;
  EFI_GUID                        *VariableGuid;
  UINTN                           MaxCount;
  BOOLEAN                         IsDefaultKey;
  REDFISH_SECURE_BOOT_KEY_TYPE    Type;
  CHAR8                           *Uri;
} SECURE_BOOT_KEY_VARIABLE_INFO;

///
/// Definition of SECURE_BOOT_KEY_SUPPORTED_TYPES
///
typedef struct {
  EFI_GUID    *SignatureType;
  CHAR8       *TypeString;
} SECURE_BOOT_KEY_SUPPORTED_TYPES;

///
/// Definition of REDFISH_SECURE_BOOT_KEY_DATA
///
typedef struct {
  LIST_ENTRY                       List;
  UINT16                           Id;
  CHAR8                            *Hash;
  CHAR8                            *Uri;
  EFI_GUID                         Type;
  EFI_SIGNATURE_DATA               *Data;
  UINTN                            DataSize;
  REDFISH_SECURE_BOOT_KEY_STATE    State;
} REDFISH_SECURE_BOOT_KEY_DATA;

#define REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST(a)  BASE_CR (a, REDFISH_SECURE_BOOT_KEY_DATA, List)

///
/// Definition of REDFISH_SECURE_BOOT_KEY_LIST_INFO
///
typedef struct {
  LIST_ENTRY    Head;
  UINTN         TotalCount;
  UINTN         AdditionCount;
  UINTN         SubtractionCount;
} REDFISH_SECURE_BOOT_KEY_LIST_INFO;

///
/// Definition of REDFISH_SECURE_BOOT_KEY_INFO
///
typedef struct {
  UINT32                               Signature;
  UINTN                                TotalCount;
  SECURE_BOOT_KEY_VARIABLE_INFO        *KeyVarInfo;
  REDFISH_JSON_BLOB                    *JsonBlob;
  EFI_STRING                           BlobName;
  EFI_GUID                             *BlobGuid;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO    Certificates;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO    Signatures;
} REDFISH_SECURE_BOOT_KEY_INFO;

/**
  This function create new key data and add into key list.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  Hash            Hash of key data
  @param[in]  Uri             URI of key key.
  @param[in]  IsCertificate   TRUE if this key is certificate.
                              FALSE if this key is Signature.
  @param[in]  State           State of key data.

  @retval EFI_SUCCESS         Key data is created successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfSecureBootKeyAddData (
  IN REDFISH_SECURE_BOOT_KEY_INFO    *KeyInfo,
  IN  CHAR8                          *Hash,
  IN  CHAR8                          *Uri,
  IN  BOOLEAN                        IsCertificate,
  IN  REDFISH_SECURE_BOOT_KEY_STATE  State
  );

/**
  This function create new key info instance with given VarInfo.

  @param[in]  VarInfo         Pointer to variable information.

  @retval REDFISH_SECURE_BOOT_KEY_INFO * Pointer to newly created key info instance.
  @retval NULL                           Error occurs.
**/
REDFISH_SECURE_BOOT_KEY_INFO *
RfSecureBootKeyNewInfo (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo
  );

/**
  This function release key info instance.

  @param[in]  KeyInfo         Pointer to key info.

**/
VOID
RfSecureBootKeyFreeInfo (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo
  );

/**
  This function create new key data and the data is came from
  specific secure boot key variable.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  Uri             URI of key key.
  @param[in]  Type            Pointer to GUID represented as key type.
  @param[in]  Data            Pointer to key blob.
  @param[in]  DataSize        The size of data.

  @retval EFI_SUCCESS         Key data is created successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfSecureBootKeyAddDataFromVariable (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo,
  IN CHAR8                         *Uri,
  IN EFI_GUID                      *Type,
  IN EFI_SIGNATURE_DATA            *Data,
  IN UINTN                         DataSize
  );

/**
  This function find target KeyData with given key ID.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  Id              ID of target key.
  @param[in]  FindCertificate TRUE if this is search on certificate.
                              FALSE if this is search on signature.

  @retval REDFISH_SECURE_BOOT_KEY_DATA *  The pointer to target data.
  @retval NULL                            target data cannot be found.
**/
REDFISH_SECURE_BOOT_KEY_DATA  *
RfSecureBootKeyFindId (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo,
  IN UINT16                        Id,
  IN BOOLEAN                       FindCertificate
  );

/**
  This function find target KeyData with given URI.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  Uri             URI of target key.
  @param[in]  FindCertificate TRUE if this is search on certificate.
                              FALSE if this is search on signature.

  @retval REDFISH_SECURE_BOOT_KEY_DATA *  The pointer to target data.
  @retval NULL                            target data cannot be found.
**/
REDFISH_SECURE_BOOT_KEY_DATA  *
RfSecureBootKeyFindUri (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo,
  IN CHAR8                         *Uri,
  IN BOOLEAN                       FindCertificate
  );

/**
  This function converts the signature type from GUID defined in UEFI
  specification to string format. It's call responsibility to release
  SignatureType by calling FreePool().

  @param[in]  SignatureType     The GUID of signature type.
  @param[out] TypeString        The signature type in ASCII string.

  @retval EFI_SUCCESS           Signature type in string is returned.
  @retval Others                Error occurs.
**/
EFI_STATUS
RfGetSignatureTypeString (
  IN EFI_GUID  *SignatureType,
  OUT CHAR8    **TypeString
  );

/**
  This function converts the signature type in string format to its
  GUID defined in UEFI specification.

  @param[in]  TypeString        The signature type in ASCII string.
  @param[out] SignatureType     The GUID of signature type.

  @retval EFI_SUCCESS           Signature type in GUID is returned.
  @retval Others                Error occurs.
**/
EFI_STATUS
RfGetSignatureTypeGuid (
  IN  CHAR8     *TypeString,
  OUT EFI_GUID  *SignatureType
  );

/**
  This function converts binary blob to string in Hexadecimal ASCII format.
  It's call responsibility to release returned string by calling FreePool().

  @param[in]  Blob         The data to be hashed.
  @param[in]  BlobSize     The size of data.

  @retval CHAR8 *          The ASCII string representation of data.
  @retval NULL             Error occurs.
**/
CHAR8 *
RfBlobToHexString (
  IN UINT8  *Blob,
  IN UINTN  BlobSize
  );

/**
  This function read NULL-terminated ASCIi string in Hexadecimal format and
  convert them into binary blob. It's call responsibility to release
  Blob by calling FreePool().

  @param[in]  HexString    ASCII string in Hexadecimal format.
  @param[out] BlobSize     The size of blob.
  @param[out] Blob         The buffer to carry data.

  @retval EFI_SUCCESS      data is returned successfully.
  @retval Others           Error occurs.
**/
EFI_STATUS
RfHexStringToBlob (
  IN CHAR8   *HexString,
  OUT UINTN  *BlobSize,
  OUT UINT8  **Blob
  );

/**
  This function update the state of key data and corresponding
  counter information.

  @param[in]  KeyInfo           Pointer to key info.
  @param[in]  UpdateCertificate TRUE if this is certificate key.
                                FALSE if this is signature key.
  @param[in]  DataToUpdate      Target to update.
  @param[in]  NewState          The new state.

  @retval EFI_SUCCESS         Key state is updated successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfSecureBootKeyUpdateState (
  IN REDFISH_SECURE_BOOT_KEY_INFO   *KeyInfo,
  IN BOOLEAN                        UpdateCertificate,
  IN REDFISH_SECURE_BOOT_KEY_DATA   *DataToUpdate,
  IN REDFISH_SECURE_BOOT_KEY_STATE  NewState
  );

/**
  This function delete key data from key list.

  @param[in]  KeyInfo           Pointer to key info.
  @param[in]  KeyList           Pointer to key list.
  @param[in]  DataToDelete      Target to delete.

  @retval EFI_SUCCESS         Key is deleted successfully.
  @retval Others              Error occurs.
**/
EFI_STATUS
RfSecureBootKeyDeleteData (
  IN REDFISH_SECURE_BOOT_KEY_INFO       *KeyInfo,
  IN REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList,
  IN REDFISH_SECURE_BOOT_KEY_DATA       *DataToDelete
  );

/**
  This function returns the hash value of input binary blob, which is
  certificate or signature data. It's call responsibility to release
  HashCtx by calling FreePool().

  @param[in]  Data         The data to be hashed.
  @param[in]  DataSize     The size of data.
  @param[out] HashCtx      The hash context.
  @param[out] HashCtxSize  The hash context size.

  @retval EFI_SUCCESS      Hash of data is returned successfully.
  @retval Others           Error occurs.
**/
EFI_STATUS
RfHashSecureBootKey (
  IN  UINT8  *Data,
  IN  UINTN  DataSize,
  OUT UINT8  **HashCtx,
  OUT UINTN  *HashCtxSize
  );

/**
  The function returns corresponding variable information by
  given secure boot variable name.

  @param[in]    SecureBootKeyName  secure boot variable name.

  @retval SECURE_BOOT_KEY_VARIABLE_INFO        Pointer to variable info.
  @retval NULL                                 Error occurs.

**/
SECURE_BOOT_KEY_VARIABLE_INFO *
RfGetSecureBootKeyInfo (
  IN  EFI_STRING  SecureBootKeyName
  );

/**
  The function returns corresponding default variable information by
  given secure boot variable name.

  @param[in]    KeyInfo  Pointer to key variable information.

  @retval SECURE_BOOT_KEY_VARIABLE_INFO        Pointer to default variable info.
  @retval NULL                                 Error occurs.

**/
SECURE_BOOT_KEY_VARIABLE_INFO *
RfGetSecureBootDefaultKeyInfo (
  IN  SECURE_BOOT_KEY_VARIABLE_INFO  *KeyInfo
  );

/**
  This function open history table from JSON blob by given secure boot variable name.
  It's call responsibility to SecureBootKeyInfo by calling RfCloseHistoryTable()

  @param[in]  KeyVariableInfo      Pointer to variable info instance.
  @param[out] SecureBootKeyInfo    Newly created key info instance.

  @retval EFI_SUCCESS           Key info instance is created successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RfOpenHistoryTable (
  IN  SECURE_BOOT_KEY_VARIABLE_INFO  *KeyVariableInfo,
  OUT REDFISH_SECURE_BOOT_KEY_INFO   **SecureBootKeyInfo
  );

/**
  This function close history table and release SecureBootKeyInfo resource.

  @param[in] SecureBootKeyInfo   Key info instance to release.
  @param[in] DoWrite             Write current table to JSON blob storage.

  @retval EFI_SUCCESS           Key info instance is released successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RfCloseHistoryTable (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *SecureBootKeyInfo,
  IN BOOLEAN                       DoWrite
  );

#if REDFISH_SECURE_BOOT_KEY_DUMP

/**
  Debug print blob binary in ASCII string format on screen.

  @param[in]  ErrorLevel        Debug error print level.
  @param[in]  Message           Additional message to show. This is optional.
  @param[in]  Blob              Binary blob to display.
  @param[in]  BlobSize          The size of blob.

**/
VOID
RfDebugDumpBlob (
  IN UINTN        ErrorLevel,
  IN CONST CHAR8  *Message,
  IN UINT8        *Blob,
  IN UINTN        BlobSize
  );

/**
  Debug print ASCII string on screen without the length limitation of
  DEBUG macro.

  @param[in]  ErrorLevel        Debug error print level.
  @param[in]  Message           Additional message to show. This is optional.
  @param[in]  StringToDump      Null-terminated string.

**/
VOID
RfDebugDumpString (
  IN UINTN        ErrorLevel,
  IN CONST CHAR8  *Message,
  IN CHAR8        *StringToDump
  );

/**

  This function dump secure boot key data in given error level.

  @param[in]  ErrorLevel  DEBUG macro error level
  @param[in]  KeyInfo     Pointer to key info data

  @retval     EFI_SUCCESS         secure boot key data is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootKeyDumpInfo (
  IN UINTN                         ErrorLevel,
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo
  );

#endif

#endif
