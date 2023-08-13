/** @file
  Redfish secure boot keys library to track secure boot keys between system boots.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "RedfishSecureBootKeysDxe.h"
#include "RedfishSecureBootData.h"

extern REDFISH_SECURE_BOOT_KEYS_PRIVATE  *mSecureBootKeysPrivate;
//
// All supported secure boot key types.
//
SECURE_BOOT_KEY_SUPPORTED_TYPES  mSupportedSignatureTypes[] = {
  {
    &gEfiCertSha256Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_SHA256
  },
  {
    &gEfiCertRsa2048Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048
  },
  {
    &gEfiCertRsa2048Sha256Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048_SHA256
  },
  {
    &gEfiCertSha1Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_SHA1
  },
  {
    &gEfiCertRsa2048Sha1Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_RSA2048_SHA1
  },
  {
    &gEfiCertX509Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_X509
  },
  {
    &gEfiCertSha224Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_SHA224
  },
  {
    &gEfiCertSha384Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_SHA384
  },
  {
    &gEfiCertSha512Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_SHA512
  },
  {
    &gEfiCertX509Sha256Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA256
  },
  {
    &gEfiCertX509Sha384Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA384
  },
  {
    &gEfiCertX509Sha512Guid,
    REDFISH_SECURE_BOOT_KEY_TYPE_X509_SHA512
  }
};

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
  IN CONST CHAR8  *Message OPTIONAL,
  IN UINT8        *Blob,
  IN UINTN        BlobSize
  )
{
  CHAR8  *Buffer;
  UINTN  BufferSize;
  UINTN  Index;
  UINTN  Offset;

  if ((Blob == NULL) || (BlobSize == 0)) {
    DEBUG ((ErrorLevel, "null blob\n"));
    return;
  }

  if (!IS_EMPTY_STRING (Message)) {
    DEBUG ((ErrorLevel, "%a\n", Message));
  }

  BufferSize = (REDFISH_SECURE_BOOT_KEY_STR_LENGTH_PER_LINE * 3) + 1;
  Buffer     = AllocatePool (BufferSize);
  if (Buffer == NULL) {
    DEBUG ((ErrorLevel, "out of resource\n"));
    return;
  }

  for (Index = 0, Offset = 0; Index < BlobSize; Index++, Offset += 3) {
    if (Offset == BufferSize - 1) {
      Offset = 0;
      DEBUG ((ErrorLevel, "%a\n", Buffer));
    }

    AsciiSPrint (Buffer + Offset, BufferSize - Offset, "%02X ", Blob[Index]);
  }

  DEBUG ((ErrorLevel, "%a\n", Buffer));
  FreePool (Buffer);
}

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
  )
{
  CHAR8        Buffer[REDFISH_SECURE_BOOT_KEY_STR_LENGTH + 1];
  UINTN        StrLen;
  UINTN        Count;
  UINTN        Index;
  CONST CHAR8  *Runner;

  if (IS_EMPTY_STRING (StringToDump)) {
    DEBUG ((ErrorLevel, "null string\n"));
    return;
  }

  if (!IS_EMPTY_STRING (Message)) {
    DEBUG ((ErrorLevel, "%a\n", Message));
  }

  StrLen = AsciiStrLen (StringToDump);
  Count  = StrLen / REDFISH_SECURE_BOOT_KEY_STR_LENGTH;
  Runner = StringToDump;
  for (Index = 0; Index < Count; Index++) {
    AsciiStrnCpyS (Buffer, (REDFISH_SECURE_BOOT_KEY_STR_LENGTH + 1), Runner, REDFISH_SECURE_BOOT_KEY_STR_LENGTH);
    Buffer[REDFISH_SECURE_BOOT_KEY_STR_LENGTH] = '\0';
    DEBUG ((ErrorLevel, "%a", Buffer));
    Runner += REDFISH_SECURE_BOOT_KEY_STR_LENGTH;
  }

  Count = StrLen % REDFISH_SECURE_BOOT_KEY_STR_LENGTH;
  if (Count > 0) {
    DEBUG ((ErrorLevel, "%a", Runner));
  }

  DEBUG ((ErrorLevel, "\n"));
}

/**

  This function dump key data on screen.

  @param[in]  ErrorLevel    DEBUG macro error level
  @param[in]  KeyData       Pointer to key data

  @retval     EFI_SUCCESS         secure boot key data is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootKeyDumpData (
  IN UINTN                         ErrorLevel,
  IN REDFISH_SECURE_BOOT_KEY_DATA  *KeyData
  )
{
  if (KeyData == NULL) {
    return EFI_SUCCESS;
  }

  DEBUG ((ErrorLevel, "%d) (%a) ", KeyData->Id, KeyData->State == RedfishSecureBootKeyStatePresented ? "*" : (KeyData->State == RedfishSecureBootKeyStateAddition ? "+" : "-")));
  DEBUG ((ErrorLevel, "S: %d ", KeyData->DataSize));
  DEBUG ((ErrorLevel, "H: %a\n", KeyData->Hash == NULL ? "unknown" : KeyData->Hash));
  DEBUG ((ErrorLevel, "U: %a\n", KeyData->Uri == NULL ? "unknown" : KeyData->Uri));

  return EFI_SUCCESS;
}

/**

  This function dump key list on screen.

  @param[in]  ErrorLevel    DEBUG macro error level
  @param[in]  KeyList       Pointer to key list
  @param[in]  IsCertificate TRUE if this is certificate. FALSE if this is signature.

  @retval     EFI_SUCCESS         secure boot key list is printed.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootKeyDumpKeyList (
  IN UINTN                              ErrorLevel,
  IN REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList,
  IN BOOLEAN                            IsCertificate
  )
{
  LIST_ENTRY                    *Node;
  REDFISH_SECURE_BOOT_KEY_DATA  *Data;

  if (KeyList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((ErrorLevel, "    %a\n", IsCertificate ? "Certificates" : "Signatures"));
  DEBUG ((ErrorLevel, "    Total count: %d\n", KeyList->TotalCount));
  DEBUG ((ErrorLevel, "    Addition count: %d\n", KeyList->AdditionCount));
  DEBUG ((ErrorLevel, "    Subtraction count: %d\n", KeyList->SubtractionCount));

  if (IsListEmpty (&KeyList->Head)) {
    DEBUG ((ErrorLevel, "    List is empty\n"));
    return EFI_SUCCESS;
  }

  Node = GetFirstNode (&KeyList->Head);
  while (!IsNull (&KeyList->Head, Node)) {
    Data = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);
    RfSecureBootKeyDumpData (ErrorLevel, Data);
    Node = GetNextNode (&KeyList->Head, Node);
  }

  return EFI_SUCCESS;
}

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
  )
{
  if (KeyInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((ErrorLevel, "Dump %s:\n", KeyInfo->KeyVarInfo->VariableName));
  DEBUG ((ErrorLevel, "  Blob: %s\n", KeyInfo->BlobName));
  DEBUG ((ErrorLevel, "  URI: %a\n", KeyInfo->KeyVarInfo->Uri));
  DEBUG ((ErrorLevel, "  Total count: %d\n", KeyInfo->TotalCount));
  RfSecureBootKeyDumpKeyList (ErrorLevel, &KeyInfo->Certificates, TRUE);
  RfSecureBootKeyDumpKeyList (ErrorLevel, &KeyInfo->Signatures, FALSE);

  return EFI_SUCCESS;
}

#endif

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
  )
{
  UINTN  Index;
  UINTN  Count;

  if ((SignatureType == NULL) || IS_EMPTY_STRING (TypeString)) {
    return EFI_INVALID_PARAMETER;
  }

  Count = ARRAY_SIZE (mSupportedSignatureTypes);
  for (Index = 0; Index < Count; Index++) {
    if (AsciiStrCmp (TypeString, mSupportedSignatureTypes[Index].TypeString) == 0) {
      CopyGuid (SignatureType, mSupportedSignatureTypes[Index].SignatureType);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

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
  )
{
  UINTN  Index;
  UINTN  Count;

  if ((SignatureType == NULL) || (TypeString == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *TypeString = NULL;

  Count = ARRAY_SIZE (mSupportedSignatureTypes);
  for (Index = 0; Index < Count; Index++) {
    if (CompareGuid (SignatureType, mSupportedSignatureTypes[Index].SignatureType)) {
      (*TypeString) = AllocateCopyPool (AsciiStrSize (mSupportedSignatureTypes[Index].TypeString), mSupportedSignatureTypes[Index].TypeString);
      if ((*TypeString) == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

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
  )
{
  EFI_STATUS        Status;
  UINTN             HashSize;
  EFI_HASH2_OUTPUT  HashOutput;

  if ((mSecureBootKeysPrivate == NULL) || (mSecureBootKeysPrivate->Hash2Protocol == NULL)) {
    return EFI_NOT_READY;
  }

  Status = mSecureBootKeysPrivate->Hash2Protocol->GetHashSize (
                                                    mSecureBootKeysPrivate->Hash2Protocol,
                                                    REDFISH_SECURE_BOOT_HASH_ALGORITHM,
                                                    &HashSize
                                                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: get hash size failed: %r\n", __func__, Status));
    return Status;
  }

  Status = mSecureBootKeysPrivate->Hash2Protocol->HashInit (mSecureBootKeysPrivate->Hash2Protocol, REDFISH_SECURE_BOOT_HASH_ALGORITHM);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: hash init failed: %r\n", __func__, Status));
    return Status;
  }

  Status = mSecureBootKeysPrivate->Hash2Protocol->HashUpdate (mSecureBootKeysPrivate->Hash2Protocol, Data, DataSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: hash update failed: %r\n", __func__, Status));
    return Status;
  }

  Status = mSecureBootKeysPrivate->Hash2Protocol->HashFinal (mSecureBootKeysPrivate->Hash2Protocol, &HashOutput);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: hash final failed: %r\n", __func__, Status));
    return Status;
  }

  if (HashSize < sizeof (HashOutput.Sha256Hash)) {
    DEBUG ((DEBUG_ERROR, "%a: hash size mismatch: %d vs. %d\n", __func__, HashSize, sizeof (HashOutput.Sha256Hash)));
    return EFI_PROTOCOL_ERROR;
  }

  *HashCtx = AllocateZeroPool (HashSize);
  if ((*HashCtx) == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *HashCtxSize = HashSize;
  CopyMem ((*HashCtx), HashOutput.Sha256Hash, sizeof (HashOutput.Sha256Hash));
  ASSERT (sizeof (HashOutput.Sha256Hash) == HashSize);

  return EFI_SUCCESS;
}

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
  )
{
  CHAR8  *BlobStr;
  UINTN  StrSize;
  UINTN  Index;
  UINTN  StrIndex;

  if ((Blob == NULL) || (BlobSize == 0)) {
    return NULL;
  }

  StrSize = (sizeof (CHAR8) * BlobSize * 2) + 1;
  BlobStr = AllocatePool (StrSize);
  if (BlobStr == NULL) {
    return NULL;
  }

  for (Index = 0, StrIndex = 0; Index < BlobSize; Index++, StrIndex += 2) {
    AsciiSPrint ((BlobStr + StrIndex), 3, "%02X", Blob[Index]);
  }

  return BlobStr;
}

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
  )
{
  UINTN       StringLen;
  UINT8       *Buffer;
  UINTN       BufferSize;
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (HexString)) {
    return EFI_INVALID_PARAMETER;
  }

  *BlobSize = 0;
  *Blob     = NULL;

  StringLen = AsciiStrLen (HexString);
  if (StringLen % 2 != 0) {
    DEBUG ((DEBUG_ERROR, "%a: invalid Hex string format. String length: %d\n", __func__, StringLen));
    return EFI_UNSUPPORTED;
  }

  BufferSize = StringLen / 2;
  Buffer     = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = AsciiStrHexToBytes (HexString, StringLen, Buffer, BufferSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: AsciiStrHexToBytes return failure: %r\n", __func__, Status));
    goto ON_ERROR;
  }

  *BlobSize = BufferSize;
  *Blob     = Buffer;

  return EFI_SUCCESS;

ON_ERROR:

  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  return Status;
}

/**
  This function release KeyData.

  @param[in]  KeyData      Instance to be released.

**/
VOID
RfSecureBootKeyFreeData (
  IN REDFISH_SECURE_BOOT_KEY_DATA  *KeyData
  )
{
  if (KeyData == NULL) {
    return;
  }

  if (KeyData->Hash != NULL) {
    FreePool (KeyData->Hash);
  }

  if (KeyData->Uri != NULL) {
    FreePool (KeyData->Uri);
  }

  if (KeyData->Data != NULL) {
    FreePool (KeyData->Data);
  }

  FreePool (KeyData);
}

/**
  This function create new KeyData with given values.

  @param[in]  Id      Id of this key data.
  @param[in]  Hash    Hash of this key data.
  @param[in]  Uri     Uri of this key data.
  @param[in]  State   State of this key data.

  @retval REDFISH_SECURE_BOOT_KEY_DATA *  The pointer to newly created data.
  @retval NULL                            Error occurs.
**/
REDFISH_SECURE_BOOT_KEY_DATA *
RfSecureBootKeyNewData (
  IN  UINT16                         Id,
  IN  CHAR8                          *Hash,
  IN  CHAR8                          *Uri,
  IN  REDFISH_SECURE_BOOT_KEY_STATE  State
  )
{
  REDFISH_SECURE_BOOT_KEY_DATA  *NewData;

  NewData = AllocateZeroPool (sizeof (REDFISH_SECURE_BOOT_KEY_DATA));
  if (NewData == NULL) {
    return NULL;
  }

  NewData->Id   = Id;
  NewData->Hash = AllocateCopyPool (AsciiStrSize (Hash), Hash);
  if (NewData->Hash == NULL) {
    goto ON_ERROR;
  }

  NewData->Uri = AllocateCopyPool (AsciiStrSize (Uri), Uri);
  if (NewData->Uri == NULL) {
    goto ON_ERROR;
  }

  NewData->State = State;

  return NewData;

ON_ERROR:

  RfSecureBootKeyFreeData (NewData);

  return NULL;
}

/**
  This function find target KeyData with given key hash value.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  Hash            Hash of target key.
  @param[in]  FindCertificate TRUE if this is search on certificate.
                              FALSE if this is search on signature.

  @retval REDFISH_SECURE_BOOT_KEY_DATA *  The pointer to target data.
  @retval NULL                            target data cannot be found.
**/
REDFISH_SECURE_BOOT_KEY_DATA  *
RfSecureBootKeyFindData (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo,
  IN CHAR8                         *Hash,
  IN BOOLEAN                       FindCertificate
  )
{
  LIST_ENTRY                         *Node;
  REDFISH_SECURE_BOOT_KEY_DATA       *Data;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if ((KeyInfo == NULL) || IS_EMPTY_STRING (Hash)) {
    return NULL;
  }

  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, FindCertificate);

  if (IsListEmpty (&KeyList->Head)) {
    return NULL;
  }

  Node = GetFirstNode (&KeyList->Head);
  while (!IsNull (&KeyList->Head, Node)) {
    Data = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);

    if (AsciiStrCmp (Data->Hash, Hash) == 0) {
      return Data;
    }

    Node = GetNextNode (&KeyList->Head, Node);
  }

  return NULL;
}

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
  )
{
  LIST_ENTRY                         *Node;
  REDFISH_SECURE_BOOT_KEY_DATA       *Data;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if ((KeyInfo == NULL)) {
    return NULL;
  }

  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, FindCertificate);

  if (IsListEmpty (&KeyList->Head)) {
    return NULL;
  }

  Node = GetFirstNode (&KeyList->Head);
  while (!IsNull (&KeyList->Head, Node)) {
    Data = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);

    if (Data->Id == Id) {
      return Data;
    }

    Node = GetNextNode (&KeyList->Head, Node);
  }

  return NULL;
}

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
  )
{
  LIST_ENTRY                         *Node;
  REDFISH_SECURE_BOOT_KEY_DATA       *Data;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if ((KeyInfo == NULL)) {
    return NULL;
  }

  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, FindCertificate);

  if (IsListEmpty (&KeyList->Head)) {
    return NULL;
  }

  Node = GetFirstNode (&KeyList->Head);
  while (!IsNull (&KeyList->Head, Node)) {
    Data = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);

    if (AsciiStrCmp (Data->Uri, Uri) == 0) {
      return Data;
    }

    Node = GetNextNode (&KeyList->Head, Node);
  }

  return NULL;
}

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
  )
{
  REDFISH_SECURE_BOOT_KEY_DATA       *NewData;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if (KeyInfo->TotalCount > KeyInfo->KeyVarInfo->MaxCount) {
    DEBUG ((DEBUG_ERROR, "%a: maximum number of key reached: %d/%d\n", __func__, KeyInfo->TotalCount, KeyInfo->KeyVarInfo->MaxCount));
    return EFI_ACCESS_DENIED;
  }

  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, IsCertificate);

  NewData = RfSecureBootKeyNewData ((UINT16)KeyInfo->TotalCount, Hash, Uri, State);
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&KeyList->Head, &NewData->List);
  KeyInfo->TotalCount += 1;
  KeyList->TotalCount += 1;
  switch (State) {
    case RedfishSecureBootKeyStateSubtraction:
      KeyList->SubtractionCount += 1;
      break;
    case RedfishSecureBootKeyStateAddition:
      KeyList->AdditionCount += 1;
      break;
    default:
      break;
  }

  return EFI_SUCCESS;
}

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
  )
{
  REDFISH_SECURE_BOOT_KEY_DATA       *NewData;
  REDFISH_SECURE_BOOT_KEY_DATA       *OldData;
  UINT8                              *HashCtx;
  UINTN                              HashCtxSize;
  CHAR8                              *HashStr;
  EFI_STATUS                         Status;
  BOOLEAN                            IsCertificate;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if ((KeyInfo == NULL) || (Type == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  NewData     = NULL;
  OldData     = NULL;
  HashCtx     = NULL;
  HashCtxSize = 0;
  HashStr     = NULL;

  if (KeyInfo->TotalCount > KeyInfo->KeyVarInfo->MaxCount) {
    DEBUG ((DEBUG_ERROR, "%a: maximum number of key reached: %d/%d\n", __func__, KeyInfo->TotalCount, KeyInfo->KeyVarInfo->MaxCount));
    return EFI_ACCESS_DENIED;
  }

  //
  // Get hash of signature data.
  //
  Status = RfHashSecureBootKey ((VOID *)Data, DataSize, &HashCtx, &HashCtxSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot get hash of key: %r\n", __func__, Status));
    return Status;
  }

  HashStr = RfBlobToHexString (HashCtx, HashCtxSize);
  if (HashStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  IsCertificate = CompareGuid (Type, &gEfiCertX509Guid);

  //
  // If this key is found in key list, this is the existing key and we
  // already handle it before.
  //
  OldData = RfSecureBootKeyFindData (KeyInfo, HashStr, IsCertificate);
  if (OldData != NULL) {
    //
    // This is not new key
    //
    RfSecureBootKeyUpdateState (KeyInfo, IsCertificate, OldData, RedfishSecureBootKeyStatePresented);
    //
    // Attach key data.
    //
    CopyGuid (&OldData->Type, Type);
    if ((Data != NULL) && (DataSize > 0)) {
      OldData->Data = AllocateCopyPool (DataSize, Data);
      if (OldData->Data != NULL) {
        OldData->DataSize = DataSize;
      }
    }

    goto ON_RELEASE;
  }

  NewData = RfSecureBootKeyNewData ((UINT16)KeyInfo->TotalCount, HashStr, Uri, RedfishSecureBootKeyStateAddition);
  if (NewData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  CopyGuid (&NewData->Type, Type);
  NewData->DataSize = DataSize;
  if ((Data != NULL) && (DataSize > 0)) {
    NewData->Data = AllocateCopyPool (DataSize, Data);
    if (NewData->Data == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_ERROR;
    }
  }

  //
  // Add to corresponding key list.
  //
  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, IsCertificate);

  InsertTailList (&KeyList->Head, &NewData->List);
  KeyInfo->TotalCount    += 1;
  KeyList->TotalCount    += 1;
  KeyList->AdditionCount += 1;

ON_RELEASE:

  if (HashCtx != NULL) {
    FreePool (HashCtx);
  }

  if (HashStr != NULL) {
    FreePool (HashStr);
  }

  return EFI_SUCCESS;

ON_ERROR:

  if (HashCtx != NULL) {
    FreePool (HashCtx);
  }

  if (HashStr != NULL) {
    FreePool (HashStr);
  }

  if (NewData != NULL) {
    RfSecureBootKeyFreeData (NewData);
  }

  return Status;
}

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
  )
{
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList;

  if ((KeyInfo == NULL) || (DataToUpdate == NULL) || (NewState >= RedfishSecureBootKeyStateMax)) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataToUpdate->State == NewState) {
    return EFI_SUCCESS;
  }

  KeyList = REDFISH_SECURE_BOOT_GET_KEY_LIST (KeyInfo, UpdateCertificate);
  //
  // Old state
  //
  switch (DataToUpdate->State) {
    case RedfishSecureBootKeyStateSubtraction:
      KeyList->SubtractionCount -= 1;
      break;
    case RedfishSecureBootKeyStateAddition:
      KeyList->AdditionCount -= 1;
      break;
    default:
      break;
  }

  //
  // New state
  //
  switch (NewState) {
    case RedfishSecureBootKeyStateSubtraction:
      KeyList->SubtractionCount += 1;
      break;
    case RedfishSecureBootKeyStateAddition:
      KeyList->AdditionCount += 1;
      break;
    default:
      break;
  }

  DataToUpdate->State = NewState;

  return EFI_SUCCESS;
}

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
  )
{
  if ((KeyInfo == NULL) || (KeyList == NULL) || (DataToDelete == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&DataToDelete->List);
  KeyInfo->TotalCount -= 1;
  KeyList->TotalCount -= 1;
  switch (DataToDelete->State) {
    case RedfishSecureBootKeyStateSubtraction:
      KeyList->SubtractionCount -= 1;
      break;
    case RedfishSecureBootKeyStateAddition:
      KeyList->AdditionCount -= 1;
      break;
    default:
      break;
  }

  RfSecureBootKeyFreeData (DataToDelete);

  return EFI_SUCCESS;
}

/**
  This function release all keys in key list.

  @param[in]  KeyInfo         Pointer to key info.
  @param[in]  KeyList         Pointer to key list.

  @retval EFI_SUCCESS         All keys in key list is released successfully.
  @retval Others              Error occurs.
**/
VOID
RfSecureBootKeyFreeKeyList (
  IN REDFISH_SECURE_BOOT_KEY_INFO       *KeyInfo,
  IN REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList
  )
{
  LIST_ENTRY                    *Node;
  LIST_ENTRY                    *NextNode;
  REDFISH_SECURE_BOOT_KEY_DATA  *Data;

  if ((KeyInfo == NULL) || (KeyList == NULL)) {
    return;
  }

  if (!IsListEmpty (&KeyList->Head)) {
    Node = GetFirstNode (&KeyList->Head);
    while (!IsNull (&KeyList->Head, Node)) {
      Data     = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);
      NextNode = GetNextNode (&KeyList->Head, Node);
      RfSecureBootKeyDeleteData (KeyInfo, KeyList, Data);
      Node = NextNode;
    }
  }
}

/**
  This function release key info instance.

  @param[in]  KeyInfo         Pointer to key info.

**/
VOID
RfSecureBootKeyFreeInfo (
  IN REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo
  )
{
  if (KeyInfo == NULL) {
    return;
  }

  if (KeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return;
  }

  //
  // Certificates
  //
  RfSecureBootKeyFreeKeyList (KeyInfo, &KeyInfo->Certificates);
  //
  // Signatures
  //
  RfSecureBootKeyFreeKeyList (KeyInfo, &KeyInfo->Signatures);

  if (KeyInfo->BlobName != NULL) {
    FreePool (KeyInfo->BlobName);
  }

  if (KeyInfo->KeyVarInfo->Uri != NULL) {
    FreePool (KeyInfo->KeyVarInfo->Uri);
    KeyInfo->KeyVarInfo->Uri = NULL;
  }

  KeyInfo->Signature = 0x0;
  FreePool (KeyInfo);
}

/**
  This function create new key info instance with given VarInfo.

  @param[in]  VarInfo         Pointer to variable information.

  @retval REDFISH_SECURE_BOOT_KEY_INFO * Pointer to newly created key info instance.
  @retval NULL                           Error occurs.
**/
REDFISH_SECURE_BOOT_KEY_INFO *
RfSecureBootKeyNewInfo (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo
  )
{
  REDFISH_SECURE_BOOT_KEY_INFO  *NewInfo;

  if (VarInfo == NULL) {
    return NULL;
  }

  NewInfo = AllocateZeroPool (sizeof (REDFISH_SECURE_BOOT_KEY_INFO));
  if (NewInfo == NULL) {
    return NULL;
  }

  NewInfo->Signature  = REDFISH_SECURE_BOOT_KEY_SIGNATURE;
  NewInfo->KeyVarInfo = VarInfo;
  InitializeListHead (&NewInfo->Certificates.Head);
  InitializeListHead (&NewInfo->Signatures.Head);

  return NewInfo;
}
