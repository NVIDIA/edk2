/** @file
  Redfish secure boot keys driver to track secure boot keys between system boots.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishSecureBootKeysDxe.h"
#include "RedfishSecureBootData.h"
#include "RedfishSecureBootKeyOp.h"

REDFISH_SECURE_BOOT_KEYS_PRIVATE  *mSecureBootKeysPrivate = NULL;
//
// All supported secure boot key variables
//
SECURE_BOOT_KEY_VARIABLE_INFO  mSecureBootKeyInfo[] = {
  {
    EFI_PK_DEFAULT_VARIABLE_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_PK_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyPkDefault,
    NULL
  },
  {
    EFI_KEK_DEFAULT_VARIABLE_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyKekDefault,
    NULL
  },
  {
    EFI_DB_DEFAULT_VARIABLE_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyDbDefault,
    NULL
  },
  {
    EFI_DBX_DEFAULT_VARIABLE_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    TRUE,
    RedfishSecureBootKeyDbxDefault,
    NULL
  },
  {
    EFI_DBT_DEFAULT_VARIABLE_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    TRUE,
    RedfishSecureBootKeyDbtDefault,
    NULL
  },
  {
    EFI_PLATFORM_KEY_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_PK_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyPk,
    NULL
  },
  {
    EFI_KEY_EXCHANGE_KEY_NAME,
    &gEfiGlobalVariableGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyKek,
    NULL
  },
  {
    EFI_IMAGE_SECURITY_DATABASE,
    &gEfiImageSecurityDatabaseGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyDb,
    NULL
  },
  {
    EFI_IMAGE_SECURITY_DATABASE1,
    &gEfiImageSecurityDatabaseGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyDbx,
    NULL
  },
  {
    EFI_IMAGE_SECURITY_DATABASE2,
    &gEfiImageSecurityDatabaseGuid,
    REDFISH_SECURE_BOOT_KEY_MAX_COUNT,
    FALSE,
    RedfishSecureBootKeyDbt,
    NULL
  }
};

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
  )
{
  UINTN  SecureBootKeyInfoSize;
  UINTN  Index;

  SecureBootKeyInfoSize = ARRAY_SIZE (mSecureBootKeyInfo);

  for (Index = 0; Index < SecureBootKeyInfoSize; Index++) {
    if (StrCmp (SecureBootKeyName, mSecureBootKeyInfo[Index].VariableName) == 0) {
      return &mSecureBootKeyInfo[Index];
    }
  }

  return NULL;
}

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
  )
{
  REDFISH_SECURE_BOOT_KEY_TYPE  DefaultKeyType;
  UINTN                         SecureBootKeyInfoSize;
  UINTN                         Index;

  SecureBootKeyInfoSize = ARRAY_SIZE (mSecureBootKeyInfo);

  switch (KeyInfo->Type) {
    case RedfishSecureBootKeyPk:
      DefaultKeyType = RedfishSecureBootKeyPkDefault;
      break;
    case RedfishSecureBootKeyKek:
      DefaultKeyType = RedfishSecureBootKeyKekDefault;
      break;
    case RedfishSecureBootKeyDb:
      DefaultKeyType = RedfishSecureBootKeyDbDefault;
      break;
    case RedfishSecureBootKeyDbt:
      DefaultKeyType = RedfishSecureBootKeyDbtDefault;
      break;
    case RedfishSecureBootKeyDbx:
      DefaultKeyType = RedfishSecureBootKeyDbxDefault;
      break;
    case RedfishSecureBootKeyDbr:
      DefaultKeyType = RedfishSecureBootKeyDbrDefault;
      break;
    default:
      return NULL;
  }

  for (Index = 0; Index < SecureBootKeyInfoSize; Index++) {
    if (mSecureBootKeyInfo[Index].Type == DefaultKeyType) {
      return &mSecureBootKeyInfo[Index];
    }
  }

  return NULL;
}

/**
  The function create name for JSON blob storage. It's call
  responsibility to release returned string by calling FreePool().

  @param[in]    VariableName  Secure boot variable name.

  @retval EFI_STRING          Returned blog name.
  @retval NULL                Error occurs.

**/
EFI_STRING
GenerateJsonBlobName (
  EFI_STRING  VariableName
  )
{
  UINTN       StringSize;
  EFI_STRING  NewName;

  if (IS_EMPTY_STRING (VariableName)) {
    return NULL;
  }

  StringSize = StrLen (VariableName) + StrLen (REDFISH_SECURE_BOOT_BLOB_PREFIX) + 1;
  StringSize = (StringSize * sizeof (CHAR16));
  NewName    = AllocatePool (StringSize);
  if (NewName == NULL) {
    return NULL;
  }

  UnicodeSPrint (NewName, StringSize, L"%s%s", REDFISH_SECURE_BOOT_BLOB_PREFIX, VariableName);

  return NewName;
}

/**
  The function release key record instance.

  @param[in]    KeyRecords   Pointer to key record instance.
  @param[in]    KeyCount     Number of key record in KeyRecords

  @retval EFI_SUCCESS        KeyRecords are released successfully.
  @retval Others             Error occurs.

**/
EFI_STATUS
RfReleaseKeyRecords (
  IN REDFISH_SECURE_BOOT_KEY_RECORD  *KeyRecords,
  IN UINTN                           KeyCount
  )
{
  UINTN  Index;

  if (KeyRecords == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (KeyCount == 0) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < KeyCount; Index++) {
    if (KeyRecords[Index].Uri != NULL) {
      FreePool (KeyRecords[Index].Uri);
    }

    if (KeyRecords[Index].Owner != NULL) {
      FreePool (KeyRecords[Index].Owner);
    }

    if (KeyRecords[Index].KeyString != NULL) {
      FreePool (KeyRecords[Index].KeyString);
    }

    if (KeyRecords[Index].TypeString != NULL) {
      FreePool (KeyRecords[Index].TypeString);
    }
  }

  FreePool (KeyRecords);

  return EFI_SUCCESS;
}

/**
  The function convert data from history table into REDFISH_SECURE_BOOT_KEY_RECORD
  instance. It's call responsibility to release key record by calling RfReleaseKeyRecords().

  @param[in]    KeyListInfo   Pointer to key list instance.
  @param[in]    State         Target state of key to convert.
  @param[in]    IsCertificate TRUE if this is to convert certificate.
                              FALSE if this is to convert signature.

  @retval REDFISH_SECURE_BOOT_KEY_RECORD * Pointer to converted key record.
  @retval NULL                             Some errors happened.

**/
REDFISH_SECURE_BOOT_KEY_RECORD *
RfGenerateKeyRecords (
  IN REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyListInfo,
  IN REDFISH_SECURE_BOOT_KEY_STATE      State,
  IN BOOLEAN                            IsCertificate
  )
{
  REDFISH_SECURE_BOOT_KEY_RECORD  *KeyArray;
  UINTN                           KeyCount;
  UINTN                           Index;
  LIST_ENTRY                      *Node;
  REDFISH_SECURE_BOOT_KEY_DATA    *Data;
  EFI_STATUS                      Status;

  if (KeyListInfo == NULL) {
    return NULL;
  }

  KeyCount = 0;
  if (State == RedfishSecureBootKeyStateAddition) {
    KeyCount = KeyListInfo->AdditionCount;
  } else if (State == RedfishSecureBootKeyStateSubtraction) {
    KeyCount = KeyListInfo->SubtractionCount;
  }

  if (KeyCount == 0) {
    return NULL;
  }

  KeyArray = AllocateZeroPool (sizeof (REDFISH_SECURE_BOOT_KEY_RECORD) * KeyCount);
  if (KeyArray == NULL) {
    return NULL;
  }

  Index = 0;
  Node  = GetFirstNode (&KeyListInfo->Head);
  while (!IsNull (&KeyListInfo->Head, Node)) {
    Data = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);

    if (Data->State == State) {
      ASSERT (Index < KeyCount);

      KeyArray[Index].Id  = Data->Id;
      KeyArray[Index].Uri = AllocateCopyPool (AsciiStrSize (Data->Uri), Data->Uri);
      if (KeyArray[Index].Uri == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        DEBUG ((DEBUG_ERROR, "%a: cannot get URI string: %r\n", __func__, Status));
        goto ON_ERROR;
      }

      //
      // There is no key data for subtraction key.
      //
      if (Data->State == RedfishSecureBootKeyStateAddition) {
        KeyArray[Index].Owner = RfSecureBootGuidToString (&Data->Data->SignatureOwner);
        if (KeyArray[Index].Owner == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          DEBUG ((DEBUG_ERROR, "%a: cannot get owner guid string: %r\n", __func__, Status));
          goto ON_ERROR;
        }

        Status = RfGetSignatureTypeString (&Data->Type, &KeyArray[Index].TypeString);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: cannot get key type string: %r\n", __func__, Status));
          goto ON_ERROR;
        }

        if (IsCertificate) {
          //
          // Convert certificate from DER format to PEM format because
          // Redfish dose not support DER format per standard.
          //
          Status = RfSecureBootDerToPem ((UINT8 *)(Data->Data->SignatureData), (Data->DataSize - sizeof (EFI_GUID)), &KeyArray[Index].KeyString);
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "%a: cannot convert DER to PEM: %r\n", __func__, Status));
          }
        } else {
          KeyArray[Index].KeyString = RfBlobToHexString ((UINT8 *)(Data->Data->SignatureData), (Data->DataSize - sizeof (EFI_GUID)));
          if (KeyArray[Index].KeyString == NULL) {
            DEBUG ((DEBUG_ERROR, "%a: cannot convert Signature to string: %r\n", __func__, Status));
          }
        }
      }

      Index += 1;
    }

    Node = GetNextNode (&KeyListInfo->Head, Node);
  }

  return KeyArray;

ON_ERROR:

  RfReleaseKeyRecords (KeyArray, KeyCount);

  return NULL;
}

/**
  The function convert data from history table into JSON format object.

  @param[in]    KeyList       Pointer to key list instance.
  @param[in]    JsonValue     Data in JSON object.
  @param[in]    ArrayName     The name of array in JSON object to decode.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
RedfishSecureBootKeyStructureParser (
  IN     REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyList,
  IN OUT EDKII_JSON_VALUE                   JsonValue,
  IN     CHAR8                              *ArrayName
  )
{
  LIST_ENTRY                    *Node;
  LIST_ENTRY                    *NextNode;
  REDFISH_SECURE_BOOT_KEY_DATA  *Data;
  EDKII_JSON_VALUE              HashJson;
  EDKII_JSON_VALUE              UriJson;
  EDKII_JSON_VALUE              JsonRecord;
  EDKII_JSON_VALUE              JsonArray;

  if ((KeyList == NULL) || IS_EMPTY_STRING (ArrayName)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: write %a\n", __func__, ArrayName));

  JsonArray = JsonValueInitArray ();
  if (JsonArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!IsListEmpty (&KeyList->Head)) {
    Node = GetFirstNode (&KeyList->Head);
    while (!IsNull (&KeyList->Head, Node)) {
      Data     = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);
      NextNode = GetNextNode (&KeyList->Head, Node);

      if (Data->State == RedfishSecureBootKeyStatePresented) {
        HashJson = JsonValueInitAsciiString (Data->Hash);
        if (HashJson == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        UriJson = JsonValueInitAsciiString (Data->Uri);
        if (UriJson == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        JsonRecord = JsonValueInitObject ();
        if (JsonRecord == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        JsonObjectSetValue (JsonRecord, REDFISH_SECURE_BOOT_JSON_HASH_NAME, HashJson);
        JsonObjectSetValue (JsonRecord, REDFISH_SECURE_BOOT_JSON_URI_NAME, UriJson);
        JsonArrayAppendValue (JsonArray, JsonRecord);
      }

      Node = NextNode;
    }
  } else {
    DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: %a is empty\n", __func__, ArrayName));
  }

  JsonObjectSetValue (JsonValue, ArrayName, JsonArray);
  return EFI_SUCCESS;
}

/**
  The callback function to convert data from private structure to JSON object.

  @param[in, out] JsonValue         Json object which carries data converted from StructureData.
  @param[in]      Context           Pointer to context from caller.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval EFI_NOT_FOUND            No data to be converted to JSON object.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
RedfishSecureBootKeyToJson (
  IN OUT EDKII_JSON_VALUE  JsonValue,
  IN     VOID              *Context
  )
{
  EFI_STATUS                    Status;
  REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo;

  if (Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)Context;
  if (KeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  if (KeyInfo->TotalCount == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Certificates
  //
  Status = RedfishSecureBootKeyStructureParser (&KeyInfo->Certificates, JsonValue, REDFISH_SECURE_BOOT_JSON_CERT_NAME);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: write %a failed: %r\n", __func__, REDFISH_SECURE_BOOT_JSON_CERT_NAME, Status));
    return Status;
  }

  //
  // Signatures
  //
  Status = RedfishSecureBootKeyStructureParser (&KeyInfo->Signatures, JsonValue, REDFISH_SECURE_BOOT_JSON_SIG_NAME);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: write %a failed: %r\n", __func__, REDFISH_SECURE_BOOT_JSON_SIG_NAME, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  The function parse input data in JSON format and add data to history table.

  @param[in]    KeyInfo       Pointer to key info instance.
  @param[in]    JsonValue     Data in JSON object.
  @param[in]    ArrayName     The name of array in JSON object to decode.
  @param[in]    IsCertificate TRUE if this is certificate data.
                              FALSE if this is signature data.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
RedfishSecureBootKeyJsonParser (
  IN  REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo,
  IN EDKII_JSON_VALUE               JsonValue,
  IN CHAR8                          *ArrayName,
  IN BOOLEAN                        IsCertificate
  )
{
  EDKII_JSON_VALUE  ArrayValue;
  EDKII_JSON_ARRAY  JsonArray;
  EDKII_JSON_VALUE  Hash;
  EDKII_JSON_VALUE  Uri;
  UINTN             Index;
  EDKII_JSON_VALUE  Value;
  CHAR8             *HashStr;
  CHAR8             *UriStr;
  EFI_STATUS        Status;

  if ((JsonValue == NULL) || IS_EMPTY_STRING (ArrayName)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: parse %a\n", __func__, ArrayName));

  ArrayValue = JsonObjectGetValue (JsonValueGetObject (JsonValue), ArrayName);
  if ((ArrayValue != NULL) && JsonValueIsArray (ArrayValue) && (JsonArrayCount (ArrayValue) > 0)) {
    JsonArray = JsonValueGetArray (ArrayValue);
    if (JsonArray == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: invalid JSON array\n", __func__));
      return EFI_VOLUME_CORRUPTED;
    }

    EDKII_JSON_ARRAY_FOREACH (JsonArray, Index, Value) {
      Hash = JsonObjectGetValue (JsonValueGetObject (Value), REDFISH_SECURE_BOOT_JSON_HASH_NAME);
      if (Hash == NULL) {
        return EFI_NOT_FOUND;
      }

      Uri = JsonObjectGetValue (JsonValueGetObject (Value), REDFISH_SECURE_BOOT_JSON_URI_NAME);
      if (Uri == NULL) {
        return EFI_NOT_FOUND;
      }

      HashStr = (CHAR8 *)JsonValueGetAsciiString (Hash);
      if (HashStr == NULL) {
        return EFI_NOT_FOUND;
      }

      UriStr = (CHAR8 *)JsonValueGetAsciiString (Uri);
      if (UriStr == NULL) {
        return EFI_NOT_FOUND;
      }

      //
      // Add new data
      //
      Status = RfSecureBootKeyAddData (KeyInfo, HashStr, UriStr, IsCertificate, RedfishSecureBootKeyStateSubtraction);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: failed to add key data: %r\n", __func__, Status));
        return Status;
      }
    }
  } else {
    DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: %a is empty\n", __func__, ArrayName));
  }

  return EFI_SUCCESS;
}

/**
  The callback function to convert data from JSON object to private structure.

  @param[in]    JsonValue         Json object to be converted to private structure.
  @param[in]    Context           Pointer to context from caller.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval EFI_NOT_FOUND            No data to be converted to private structure.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
RedfishSecureBootKeyToStructure (
  IN EDKII_JSON_VALUE  JsonValue,
  IN VOID              *Context
  )
{
  REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo;
  EFI_STATUS                    Status;

  if (Context == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)Context;
  if (KeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  //
  // Certificates
  //
  Status = RedfishSecureBootKeyJsonParser (KeyInfo, JsonValue, REDFISH_SECURE_BOOT_JSON_CERT_NAME, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: read %a failed: %r\n", __func__, REDFISH_SECURE_BOOT_JSON_CERT_NAME, Status));
    return Status;
  }

  //
  // Signatures
  //
  Status = RedfishSecureBootKeyJsonParser (KeyInfo, JsonValue, REDFISH_SECURE_BOOT_JSON_SIG_NAME, FALSE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: read %a failed: %r\n", __func__, REDFISH_SECURE_BOOT_JSON_SIG_NAME, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

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
  )
{
  if (SecureBootKeyInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (SecureBootKeyInfo->JsonBlob == NULL) {
    return EFI_NOT_STARTED;
  }

  //
  // Close and write Redfish secure boot keys to JSON blob.
  //
  if (SecureBootKeyInfo->JsonBlob != NULL) {
    RedfishJsonBlobClose (SecureBootKeyInfo->JsonBlob, DoWrite);
  }

  RfSecureBootKeyFreeInfo (SecureBootKeyInfo);

  return EFI_SUCCESS;
}

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
  )
{
  REDFISH_SECURE_BOOT_KEY_INFO  *KeyInfo;

  if (SecureBootKeyInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *SecureBootKeyInfo = NULL;
  KeyInfo            = RfSecureBootKeyNewInfo (KeyVariableInfo);
  if (KeyInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Load history table
  //
  KeyInfo->BlobName = GenerateJsonBlobName (KeyInfo->KeyVarInfo->VariableName);
  KeyInfo->BlobGuid = &gEfiRedfishClientVariableGuid;
  KeyInfo->JsonBlob = RedfishJsonBlobOpen (
                        KeyInfo->BlobGuid,
                        KeyInfo->BlobName,
                        RedfishSecureBootKeyToStructure,
                        RedfishSecureBootKeyToJson,
                        (VOID *)KeyInfo
                        );

  *SecureBootKeyInfo = KeyInfo;
  return EFI_SUCCESS;
}

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
EFI_STATUS
RedfishSecureBootKeyReset (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey,
  IN BOOLEAN                                  ResetCertificate
  )
{
  REDFISH_SECURE_BOOT_KEY_INFO       *SecureBootKeyInfo;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyListInfo;
  LIST_ENTRY                         *Node;
  LIST_ENTRY                         *NextNode;
  REDFISH_SECURE_BOOT_KEY_DATA       *Data;

  if (RedfishSecureBootKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SecureBootKeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)RedfishSecureBootKey;
  if (SecureBootKeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  KeyListInfo = REDFISH_SECURE_BOOT_GET_KEY_LIST (SecureBootKeyInfo, ResetCertificate);

  Node = GetFirstNode (&KeyListInfo->Head);
  while (!IsNull (&KeyListInfo->Head, Node)) {
    Data     = REDFISH_SECURE_BOOT_KEY_DATA_FROM_LIST (Node);
    NextNode = GetNextNode (&KeyListInfo->Head, Node);

    //
    // Change all key presented in database to be addition key
    // because we like to upload them to BMC again.
    // Delete subtraction key because they are removed in BMC
    // already.
    //
    if (Data->State == RedfishSecureBootKeyStatePresented) {
      RfSecureBootKeyUpdateState (SecureBootKeyInfo, ResetCertificate, Data, RedfishSecureBootKeyStateAddition);
    } else if (Data->State == RedfishSecureBootKeyStateSubtraction) {
      RfSecureBootKeyDeleteData (SecureBootKeyInfo, KeyListInfo, Data);
    }

    Node = NextNode;
  }

 #if REDFISH_SECURE_BOOT_KEY_DUMP
  RfSecureBootKeyDumpInfo (REDFISH_SECURE_BOOT_KEY_DEBUG, SecureBootKeyInfo);
 #endif

  return EFI_SUCCESS;
}

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
EFI_STATUS
RedfishSecureBootKeyUpdateKey (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey,
  IN UINT16                                   KeyId,
  IN BOOLEAN                                  UpdateCertificate,
  IN EFI_STRING                               KeyUri
  )
{
  REDFISH_SECURE_BOOT_KEY_INFO  *SecureBootKeyInfo;
  REDFISH_SECURE_BOOT_KEY_DATA  *TargetData;
  UINTN                         AsciiStrSize;

  if ((RedfishSecureBootKey == NULL) || IS_EMPTY_STRING (KeyUri)) {
    return EFI_INVALID_PARAMETER;
  }

  SecureBootKeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)RedfishSecureBootKey;
  if (SecureBootKeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: Update %s %a at ID %d\n", __func__, SecureBootKeyInfo->KeyVarInfo->VariableName, (UpdateCertificate ? "Certificate" : "Signature"), KeyId));

  TargetData = RfSecureBootKeyFindId (SecureBootKeyInfo, KeyId, UpdateCertificate);
  if (TargetData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (TargetData->Uri != NULL) {
    FreePool (TargetData->Uri);
  }

  AsciiStrSize    = StrLen (KeyUri) * sizeof (CHAR8) + 1;
  TargetData->Uri = AllocatePool (AsciiStrSize);
  if (TargetData->Uri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  UnicodeStrToAsciiStrS (KeyUri, TargetData->Uri, AsciiStrSize);

  //
  // This key is updated to BMC.
  //
  RfSecureBootKeyUpdateState (SecureBootKeyInfo, UpdateCertificate, TargetData, RedfishSecureBootKeyStatePresented);

  return EFI_SUCCESS;
}

/**
  Release the resource of KeyList. After this function is returned, KeyList
  is no longer available.

  @param[in] This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in] KeyList              KeyList to release.

  @retval EFI_SUCCESS           Redfish secure boot key list is released successfully.
  @retval Others                Error occurs.

**/
VOID
RedfishSecureBootKeyFreeKeyList (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY_LIST             *KeyList
  )
{
  if (KeyList == NULL) {
    return;
  }

  if (KeyList->AdditionList != NULL) {
    RfReleaseKeyRecords (KeyList->AdditionList, KeyList->AdditionCount);
  }

  if (KeyList->SubtractionList != NULL) {
    RfReleaseKeyRecords (KeyList->SubtractionList, KeyList->SubtractionCount);
  }

  FreePool (KeyList);
}

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
EFI_STATUS
RedfishSecureBootKeyGetKeyList (
  IN  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN  REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey,
  IN  BOOLEAN                                  GetCertificate,
  OUT REDFISH_SECURE_BOOT_KEY_LIST             **KeyList
  )
{
  EFI_STATUS                         Status;
  REDFISH_SECURE_BOOT_KEY_INFO       *SecureBootKeyInfo;
  REDFISH_SECURE_BOOT_KEY_LIST       *NewList;
  REDFISH_SECURE_BOOT_KEY_LIST_INFO  *KeyListInfo;

  if ((RedfishSecureBootKey == NULL) || (KeyList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  SecureBootKeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)RedfishSecureBootKey;
  if (SecureBootKeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: Get %s %a\n", __func__, SecureBootKeyInfo->KeyVarInfo->VariableName, (GetCertificate ? "Certificate" : "Signature")));

  *KeyList = NULL;

  //
  // Check and see if we have key to return or not.
  //
  KeyListInfo = REDFISH_SECURE_BOOT_GET_KEY_LIST (SecureBootKeyInfo, GetCertificate);
  if (KeyListInfo->TotalCount == 0) {
    return EFI_NOT_FOUND;
  }

  NewList = AllocateZeroPool (sizeof (REDFISH_SECURE_BOOT_KEY_LIST));
  if (NewList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (KeyListInfo->AdditionCount > 0) {
    NewList->AdditionList = RfGenerateKeyRecords (KeyListInfo, RedfishSecureBootKeyStateAddition, GetCertificate);
    if (NewList->AdditionList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_ERROR;
    }

    NewList->AdditionCount = KeyListInfo->AdditionCount;
  }

  if (KeyListInfo->SubtractionCount > 0) {
    NewList->SubtractionList = RfGenerateKeyRecords (KeyListInfo, RedfishSecureBootKeyStateSubtraction, GetCertificate);
    if (NewList->SubtractionList == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_ERROR;
    }

    NewList->SubtractionCount = KeyListInfo->SubtractionCount;
  }

  NewList->IsCertificate = GetCertificate;
  NewList->TotalKeyCount = KeyListInfo->TotalCount;
  *KeyList               = NewList;

  return EFI_SUCCESS;

ON_ERROR:

  RedfishSecureBootKeyFreeKeyList (This, NewList);

  return Status;
}

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
EFI_STATUS
RedfishSecureBootKeyOpen (
  IN  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN  EFI_STRING                               SecureBootKeyName,
  IN  CHAR8                                    *SecureBootUri,
  OUT REDFISH_SECURE_BOOT_KEY                  **RedfishSecureBootKey
  )
{
  EFI_STATUS                     Status;
  SECURE_BOOT_KEY_VARIABLE_INFO  *KeyVariableInfo;
  REDFISH_SECURE_BOOT_KEY_INFO   *KeyInfo;

  if (IS_EMPTY_STRING (SecureBootKeyName) || IS_EMPTY_STRING (SecureBootUri) || (RedfishSecureBootKey == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *RedfishSecureBootKey = NULL;

  //
  // Check and see if this is supported secure boot key
  //
  KeyVariableInfo = RfGetSecureBootKeyInfo (SecureBootKeyName);
  if (KeyVariableInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: open %s %g\n", __func__, KeyVariableInfo->VariableName, KeyVariableInfo->VariableGuid));

  //
  // Update Redfish URI
  //
  if (KeyVariableInfo->Uri == NULL) {
    KeyVariableInfo->Uri = AllocateCopyPool (AsciiStrSize (SecureBootUri), SecureBootUri);
    if (KeyVariableInfo->Uri == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: copy URI failed: %a\n", __func__, SecureBootUri));
    }
  }

  //
  // Load history table
  //
  Status = RfOpenHistoryTable (KeyVariableInfo, &KeyInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: load history table %s failed: %r\n", __func__, KeyInfo->KeyVarInfo->VariableName, Status));
    return Status;
  }

  //
  // Load variable
  //
  Status = RfSecureBootLoadVariable (KeyInfo);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: no secure boot variable %s found: %r\n", __func__, KeyInfo->KeyVarInfo->VariableName, Status));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: load secure boot variable %s failed: %r\n", __func__, KeyInfo->KeyVarInfo->VariableName, Status));
    }
  }

 #if REDFISH_SECURE_BOOT_KEY_DUMP
  RfSecureBootKeyDumpInfo (REDFISH_SECURE_BOOT_KEY_DEBUG, KeyInfo);
 #endif

  *RedfishSecureBootKey = (REDFISH_SECURE_BOOT_KEY *)KeyInfo;

  return EFI_SUCCESS;
}

/**
  Close Redfish secure boot key with given key handler. After this call,
  RedfishSecureBootKey is no longer usable.

  @param[in]   This                 Pointer to EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL instance.
  @param[in]   RedfishSecureBootKey Secure boot key handle.

  @retval EFI_SUCCESS           Redfish secure boot key is closed successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishSecureBootKeyClose (
  IN EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  *This,
  IN REDFISH_SECURE_BOOT_KEY                  *RedfishSecureBootKey
  )
{
  EFI_STATUS                    Status;
  REDFISH_SECURE_BOOT_KEY_INFO  *SecureBootKeyInfo;

  if (RedfishSecureBootKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SecureBootKeyInfo = (REDFISH_SECURE_BOOT_KEY_INFO *)RedfishSecureBootKey;
  if (SecureBootKeyInfo->Signature != REDFISH_SECURE_BOOT_KEY_SIGNATURE) {
    return EFI_VOLUME_CORRUPTED;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: close %s\n", __func__, SecureBootKeyInfo->KeyVarInfo->VariableName));

 #if REDFISH_SECURE_BOOT_KEY_DUMP
  RfSecureBootKeyDumpInfo (REDFISH_SECURE_BOOT_KEY_DEBUG, SecureBootKeyInfo);
 #endif

  //
  // Close and write Redfish secure boot keys to JSON blob.
  //
  Status = RfCloseHistoryTable (SecureBootKeyInfo, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot close history table: %s\n", __func__, SecureBootKeyInfo->BlobName));
  }

  return EFI_SUCCESS;
}

/**
  This is a EFI_REDFISH_PLATFORM_CONFIG_PROTOCOL notification event handler.

  Install HII package notification.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
Hash2ServiceBindingProtocolInstalled (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS  Status;

  if (mSecureBootKeysPrivate == NULL) {
    return;
  }

  //
  // Locate EFI_SERVICE_BINDING_PROTOCOL.
  //
  Status = gBS->LocateProtocol (
                  &gEfiHash2ServiceBindingProtocolGuid,
                  NULL,
                  (VOID **)&mSecureBootKeysPrivate->ServiceBinding
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: locate Hash2 service binding failure: %r\n", __func__, Status));
    return;
  }

  //
  // Get Hash2 protocol
  //
  mSecureBootKeysPrivate->Hash2Handle = NULL;
  Status                              = mSecureBootKeysPrivate->ServiceBinding->CreateChild (
                                                                                  mSecureBootKeysPrivate->ServiceBinding,
                                                                                  &mSecureBootKeysPrivate->Hash2Handle
                                                                                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot create child for hash2 protocol: %r\n", __func__, Status));
    return;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: Hash2 child handle: %p\n", __func__, mSecureBootKeysPrivate->Hash2Handle));

  Status = gBS->OpenProtocol (
                  mSecureBootKeysPrivate->Hash2Handle,
                  &gEfiHash2ProtocolGuid,
                  (VOID **)&mSecureBootKeysPrivate->Hash2Protocol,
                  mSecureBootKeysPrivate->ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot open hash2 protocol: %r\n", __func__, Status));
    mSecureBootKeysPrivate->ServiceBinding->DestroyChild (mSecureBootKeysPrivate->ServiceBinding, mSecureBootKeysPrivate->Hash2Handle);
    mSecureBootKeysPrivate->Hash2Handle = NULL;
    return;
  }

  gBS->CloseEvent (Event);
  mSecureBootKeysPrivate->ProtocolNotify = NULL;
}

EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL  mRedfishSecureBootKeys = {
  EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL_REVISION,
  RedfishSecureBootKeyOpen,
  RedfishSecureBootKeyClose,
  RedfishSecureBootKeyUpdateKey,
  RedfishSecureBootKeyReset,
  RedfishSecureBootKeyFreeKeyList,
  RedfishSecureBootKeyGetKeyList,
  RedfishSecureBootDeleteAllKeys,
  RedfishSecureBootResetAllKeys,
  RedfishSecureBootEnrollKey,
  RedfishSecureBootDeleteKey
};

/**

  Initial redfish secure boot keys instance.

  @param[in] ImageHandle     The image handle.

  @retval  EFI_SUCCESS  Create redfish secure boot key private data successfully.
  @retval  Other        Return error status.

**/
EFI_STATUS
RedfishSecureBootKeysInitialize (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;

  if (mSecureBootKeysPrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mSecureBootKeysPrivate = AllocateZeroPool (sizeof (REDFISH_SECURE_BOOT_KEYS_PRIVATE));
  if (mSecureBootKeysPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mSecureBootKeysPrivate->ImageHandle = ImageHandle;
  CopyMem (&mSecureBootKeysPrivate->SecureBootKeysProtocol, &mRedfishSecureBootKeys, sizeof (EDKII_REDFISH_SECURE_BOOT_KEYS_PROTOCOL));
  mSecureBootKeysPrivate->ProtocolNotify = EfiCreateProtocolNotifyEvent (
                                             &gEfiHash2ServiceBindingProtocolGuid,
                                             TPL_CALLBACK,
                                             Hash2ServiceBindingProtocolInstalled,
                                             NULL,
                                             &mSecureBootKeysPrivate->Registration
                                             );

  //
  // Install the gEdkIIRedfishTaskProtocolGuid onto Handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSecureBootKeysPrivate->ImageHandle,
                  &gEdkIIRedfishSecureBootKeysProtocolGuid,
                  &mSecureBootKeysPrivate->SecureBootKeysProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot install Redfish task protocol: %r\n", __func__, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Release allocated resource.

  @retval EFI_SUCCESS      The resource is released.

**/
EFI_STATUS
RedfishSecureBootKeysRelease (
  VOID
  )
{
  if (mSecureBootKeysPrivate == NULL) {
    return EFI_SUCCESS;
  }

  if (mSecureBootKeysPrivate->ProtocolNotify != NULL) {
    gBS->CloseEvent (mSecureBootKeysPrivate->ProtocolNotify);
  }

  if ((mSecureBootKeysPrivate->ServiceBinding != NULL) && (mSecureBootKeysPrivate->Hash2Handle != NULL)) {
    mSecureBootKeysPrivate->ServiceBinding->DestroyChild (mSecureBootKeysPrivate->ServiceBinding, mSecureBootKeysPrivate->Hash2Handle);
  }

  gBS->UninstallProtocolInterface (
         mSecureBootKeysPrivate->ImageHandle,
         &gEdkIIRedfishSecureBootKeysProtocolGuid,
         (VOID *)&mSecureBootKeysPrivate->SecureBootKeysProtocol
         );

  FreePool (mSecureBootKeysPrivate);

  return EFI_SUCCESS;
}

/**
  Main entry for this driver.

  @param[in] ImageHandle     Image handle this driver.
  @param[in] SystemTable     Pointer to SystemTable.

  @retval EFI_SUCCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
RedfishSecureBootKeysEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return RedfishSecureBootKeysInitialize (ImageHandle);
}

/**
  Unloads an image.

  @param[in]  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishSecureBootKeysUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  return RedfishSecureBootKeysRelease ();
}
