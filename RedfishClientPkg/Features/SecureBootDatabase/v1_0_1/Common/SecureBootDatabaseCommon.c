/** @file
  Redfish feature driver implementation - common functions

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SecureBootDatabaseCommon.h"

REDFISH_RESOURCE_COMMON_PRIVATE  *mRedfishResourcePrivate             = NULL;
EFI_HANDLE                       mRedfishResourceConfigProtocolHandle = NULL;
EDKII_REDFISH_TASK_PROTOCOL      *mRedfishTaskProtocol                = NULL;

/**
  This function return the "Members@odata.count" in given collection URI.

  @param[in]  RedfishService    Redfish service instance.
  @param[in]  CollectionUri     URI to collection resource.
  @param[out] MemberCount       Member count on return.

  @retval EFI_SUCCESS           Member count is returned successfully.
  @retval Others                Error occurs.
**/
EFI_STATUS
RedfishGetCollectionCount (
  IN  REDFISH_SERVICE  RedfishService,
  IN  EFI_STRING       CollectionUri,
  OUT UINTN            *MemberCount
  )
{
  EFI_STATUS        Status;
  REDFISH_RESPONSE  Response;
  EDKII_JSON_VALUE  CollectionObj;
  EDKII_JSON_VALUE  MemberCountObj;

  if ((RedfishService == NULL) || IS_EMPTY_STRING (CollectionUri) || (MemberCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *MemberCount = 0;

  ZeroMem (&Response, sizeof (Response));
  Status = RedfishHttpGetResource (RedfishService, CollectionUri, &Response, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: get resource from: %s failed\n", __func__, CollectionUri));
    goto ON_RELEASE;
  }

  if (Response.Payload == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_RELEASE;
  }

  CollectionObj = RedfishJsonInPayload (Response.Payload);
  if (CollectionObj == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  MemberCountObj = JsonObjectGetValue (JsonValueGetObject (CollectionObj), "Members@odata.count");
  if (MemberCountObj == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  *MemberCount = JsonValueGetInteger (MemberCountObj);

ON_RELEASE:

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  This function delete all members in collection resource.

  @param[in]  RedfishService    Redfish service instance.
  @param[in]  CollectionUri     URI to collection resource.

  @retval EFI_SUCCESS           Members are deleted successfully.
  @retval Others                Error occurs.
**/
EFI_STATUS
RedfishClearCollection (
  IN REDFISH_SERVICE  RedfishService,
  IN EFI_STRING       CollectionUri
  )
{
  EFI_STATUS        Status;
  REDFISH_RESPONSE  Response;
  REDFISH_RESPONSE  DelResponse;
  EDKII_JSON_VALUE  CollectionObj;
  EDKII_JSON_VALUE  MemberCountObj;
  EDKII_JSON_VALUE  MembersObj;
  EDKII_JSON_ARRAY  MemberArray;
  EDKII_JSON_VALUE  MemberUriObj;
  EDKII_JSON_VALUE  Value;
  UINTN             Index;
  UINTN             MemberCount;
  EFI_STRING        MemberUri;

  if ((RedfishService == NULL) || IS_EMPTY_STRING (CollectionUri)) {
    return EFI_INVALID_PARAMETER;
  }

  MemberUri   = NULL;
  MemberCount = 0;

  ZeroMem (&Response, sizeof (Response));
  Status = RedfishHttpGetResource (RedfishService, CollectionUri, &Response, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: get resource from: %s failed\n", __func__, CollectionUri));
    goto ON_RELEASE;
  }

  if (Response.Payload == NULL) {
    Status = EFI_DEVICE_ERROR;
    goto ON_RELEASE;
  }

  CollectionObj = RedfishJsonInPayload (Response.Payload);
  if (CollectionObj == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  MemberCountObj = JsonObjectGetValue (JsonValueGetObject (CollectionObj), "Members@odata.count");
  if (MemberCountObj == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  MemberCount = JsonValueGetInteger (MemberCountObj);
  if (MemberCount == 0) {
    Status = EFI_SUCCESS;
    goto ON_RELEASE;
  }

  MembersObj = JsonObjectGetValue (JsonValueGetObject (CollectionObj), "Members");
  if (MembersObj == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  MemberArray = JsonValueGetArray (MembersObj);
  if (MemberArray == NULL) {
    Status = EFI_VOLUME_CORRUPTED;
    goto ON_RELEASE;
  }

  EDKII_JSON_ARRAY_FOREACH (MemberArray, Index, Value) {
    MemberUriObj = JsonObjectGetValue (JsonValueGetObject (Value), "@odata.id");
    if (MemberUriObj == NULL) {
      continue;
    }

    MemberUri = JsonValueGetUnicodeString (MemberUriObj);
    if (MemberUri != NULL) {
      DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: delete %s\n", __func__, MemberUri));
      ZeroMem (&DelResponse, sizeof (DelResponse));
      Status = RedfishHttpDeleteResource (RedfishService, MemberUri, &DelResponse);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: delete %s failed: %r\n", __func__, MemberUri, Status));
      }

      RedfishHttpFreeResource (&DelResponse);
      FreePool (MemberUri);
    }
  }

ON_RELEASE:

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  This function remove escape character in input string.
  responsibility to release returned string by calling FreePool().

  @param[in]  InputStr   Input string to remove escape character.
  @param[out] OutputStr  Output string after remove escape character.

  @retval EFI_SUCCESS           Escape character is removed successfully.
  @retval Others                Error occurs.
**/
EFI_STATUS
RedfishRemoveEscapeCharacter (
  IN  CHAR8  *InputStr,
  OUT CHAR8  **OutputStr
  )
{
  CHAR8  *NewStr;
  UINTN  InputIndex;
  UINTN  OutputIndex;

  if (IS_EMPTY_STRING (InputStr) || (OutputStr == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *OutputStr  = NULL;
  InputIndex  = 0;
  OutputIndex = 0;

  NewStr = AllocatePool (AsciiStrSize (InputStr));
  if (NewStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  while (InputStr[InputIndex] != '\0') {
    if ((InputStr[InputIndex] == '\\') &&  (InputStr[InputIndex + 1] == '\\')) {
      InputIndex += 1;
    }

    NewStr[OutputIndex] = InputStr[InputIndex];
    InputIndex         += 1;
    OutputIndex        += 1;
  }

  NewStr[OutputIndex] = '\0';
  *OutputStr          = NewStr;

  return EFI_SUCCESS;
}

/**
  This function issues HTTP POST to add secure boot key to BMC.

  @param[in]  RedfishService   Redfish service instance.
  @param[in]  Uri              Target collection URI.
  @param[in]  KeyRecord        Key record to post.
  @param[in]  PostCertificate  TRUE to post key to certificates.
                               FALSE to post key to signatures.
  @param[out] Location         Per Redfish standard, BMC will return the location
                               of newly posted key.

  @retval EFI_SUCCESS           Post key to Uri successfully.
  @retval Others                Error occurs.
**/
EFI_STATUS
RedfishPostSecureBootKey (
  IN REDFISH_SERVICE                 RedfishService,
  IN EFI_STRING                      Uri,
  IN REDFISH_SECURE_BOOT_KEY_RECORD  *KeyRecord,
  IN BOOLEAN                         PostCertificate,
  OUT EFI_STRING                     *Location
  )
{
  EFI_STATUS        Status;
  EDKII_JSON_VALUE  KeyValue;
  EDKII_JSON_VALUE  OwnerValue;
  EDKII_JSON_VALUE  CertString;
  EDKII_JSON_VALUE  CertType;
  EDKII_JSON_VALUE  RegistryType;
  CHAR8             *Json;
  CHAR8             *JsonParsed;
  REDFISH_RESPONSE  Response;

  if ((RedfishService == NULL) || (KeyRecord == NULL) || IS_EMPTY_STRING (Uri) || (Location == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Location    = NULL;
  KeyValue     = NULL;
  OwnerValue   = NULL;
  CertString   = NULL;
  CertType     = NULL;
  RegistryType = NULL;
  Json         = NULL;
  JsonParsed   = NULL;
  ZeroMem (&Response, sizeof (Response));

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: Post key %d to: %s\n", __func__, KeyRecord->Id, Uri));

  //
  // UefiSignatureOwner
  //
  OwnerValue = JsonValueInitAsciiString (KeyRecord->Owner);
  if (OwnerValue == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // CertificateString
  //
  CertString = JsonValueInitAsciiString (KeyRecord->KeyString);
  if (CertString == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  //
  // CertificateType
  //
  CertType = JsonValueInitAsciiString (KeyRecord->TypeString);
  if (CertType == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  if (!PostCertificate) {
    //
    // SignatureTypeRegistry
    //
    RegistryType = JsonValueInitAsciiString (REDFISH_SIGNATURE_REGISTRY_VALUE);
    if (RegistryType == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_RELEASE;
    }
  }

  KeyValue = JsonValueInitObject ();
  if (KeyValue == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  if (PostCertificate) {
    //
    // Certificate attributes
    //
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_CERTIFICATE_OWNER, OwnerValue);
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_CERTIFICATE_TYPE, CertType);
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_CERTIFICATE_STRING, CertString);
  } else {
    //
    // Signatures attributes
    //
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_SIGNATURE_OWNER, OwnerValue);
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_SIGNATURE_TYPE, CertType);
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_SIGNATURE_STRING, CertString);
    JsonObjectSetValue (JsonValueGetObject (KeyValue), REDFISH_SIGNATURE_REGISTRY, RegistryType);
  }

  Json = JsonDumpString (JsonValueGetObject (KeyValue), EDKII_JSON_COMPACT);
  if (Json == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  if (PostCertificate) {
    //
    // JsonDumpString() will convert "\n" to "\\n" but this is not valid character in PEM format.
    //
    Status = RedfishRemoveEscapeCharacter (Json, &JsonParsed);
    if (EFI_ERROR (Status) || (JsonParsed == NULL)) {
      DEBUG ((DEBUG_ERROR, "%a: can not remove escape string: %r\n", __func__, Status));
      goto ON_RELEASE;
    }
  }

  Status = RedfishHttpPostResource (RedfishService, Uri, (PostCertificate ? JsonParsed : Json), &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: post secure boot key failed: %r\n", __func__, Status));
    DumpJsonValue (DEBUG_ERROR, KeyValue);
    goto ON_RELEASE;
  }

  //
  // per Redfish spec. the URL of new resource will be returned in "Location" header.
  //
  Status = GetEtagAndLocation (&Response, NULL, Location);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot find new location: %r\n", __FUNCTION__, Status));
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: key posted to: %s\n", __func__, *Location));

ON_RELEASE:

  if (OwnerValue != NULL) {
    JsonValueFree (OwnerValue);
  }

  if (CertString != NULL) {
    JsonValueFree (CertString);
  }

  if (CertType != NULL) {
    JsonValueFree (CertType);
  }

  if (KeyValue != NULL) {
    JsonValueFree (KeyValue);
  }

  if (Json != NULL) {
    FreePool (Json);
  }

  if (JsonParsed != NULL) {
    FreePool (JsonParsed);
  }

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  This function delete specific key at BMC.

  @param[in]   RedfishService       Pointer to Redfish service instance.
  @param[in]   KeyRecord            The key to be deleted.

  @retval EFI_SUCCESS              Secure boot key is deleted successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishDeleteKey (
  IN REDFISH_SERVICE                 RedfishService,
  IN REDFISH_SECURE_BOOT_KEY_RECORD  *KeyRecord
  )
{
  EFI_STATUS        Status;
  REDFISH_RESPONSE  Response;
  EFI_STRING        KeyUri;

  if ((RedfishService == NULL) || (KeyRecord == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (IS_EMPTY_STRING (KeyRecord->Uri)) {
    DEBUG ((DEBUG_ERROR, "%a: No URI to delete\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  KeyUri = StrAsciiToUnicode (KeyRecord->Uri);
  if (KeyUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&Response, sizeof (Response));
  Status = RedfishHttpDeleteResource (RedfishService, KeyUri, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: delete %s failed: %r\n", __func__, KeyUri, Status));
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: key is deleted: %s\n", __func__, KeyUri));

  RedfishHttpFreeResource (&Response);
  FreePool (KeyUri);

  return EFI_SUCCESS;
}

/**
  This function finds secure boot key collection URI

  @param[in]   Head   Linked-list header to search.

  @retval EFI_STRING   Collection URI string is returned.
  @retval NULL         Some error happened.

**/
EFI_STRING
GetKeyCollectionUri (
  IN RedfishCS_Link  *Head
  )
{
  RedfishCS_Link           *List;
  RedfishCS_Header         *Header;
  RedfishCS_Type_Uri_Data  *UriData;

  if (IsLinkEmpty (Head)) {
    return NULL;
  }

  List   = GetFirstLink (Head);
  Header = (RedfishCS_Header *)List;
  if (Header->ResourceType != RedfishCS_Type_Uri) {
    return NULL;
  }

  UriData = (RedfishCS_Type_Uri_Data *)Header;
  return StrAsciiToUnicode (UriData->Uri);
}

/**
  This function populate give secure boot key to BMC.

  @param[in]   Private              Pointer to private instance.
  @param[in]   SecureBootDatabaseCs Secure boot database CS structure.
  @param[in]   RedfishSecureBootKey Secure boot key to be handled.
  @param[in]   IsCertificate        TRUE if this is certificate type of key. FALSE otherwise.

  @retval EFI_SUCCESS              Secure boot key is populated successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
ProvisioningKeys (
  IN REDFISH_RESOURCE_COMMON_PRIVATE           *Private,
  IN EFI_REDFISH_SECUREBOOTDATABASE_V1_0_1_CS  *SecureBootDatabaseCs,
  IN REDFISH_SECURE_BOOT_KEY                   *RedfishSecureBootKey,
  IN BOOLEAN                                   IsCertificate
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_STRING                    KeyCollectionUri;
  REDFISH_SECURE_BOOT_KEY_LIST  *SecureBootKeyList;
  EFI_STRING                    NewLocation;
  UINTN                         MemberCount;

  if ((Private == NULL) || (SecureBootDatabaseCs == NULL) || (RedfishSecureBootKey == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: provisioning %a\n", __func__, (IsCertificate ? "certificate" : "signature")));

  KeyCollectionUri  = NULL;
  SecureBootKeyList = NULL;
  MemberCount       = 0;

  //
  // Get certificate uri
  //
  if (IsCertificate) {
    KeyCollectionUri = GetKeyCollectionUri (&SecureBootDatabaseCs->Certificates);
  } else {
    KeyCollectionUri = GetKeyCollectionUri (&SecureBootDatabaseCs->Signatures);
  }

  if (KeyCollectionUri == NULL) {
    DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: %a is not supported\n", __func__, (IsCertificate ? "Certificates" : "Signatures")));
    return EFI_NOT_FOUND;
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: collection URI: %s\n", __func__, KeyCollectionUri));

  //
  // Get key list for synchronization
  //
  Status = RedfishSecureBootKeyGetKeyList (
             RedfishSecureBootKey,
             IsCertificate,
             &SecureBootKeyList
             );
  if (EFI_ERROR (Status) || (SecureBootKeyList == NULL)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: no Redfish secure boot key list in system: %r\n", __func__, Status));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get Redfish secure boot key list: %r\n", __func__, Status));
    }

    return Status;
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: Total: %d Addition: %d Subtraction: %d\n", __func__, SecureBootKeyList->TotalKeyCount, SecureBootKeyList->AdditionCount, SecureBootKeyList->SubtractionCount));

  //
  // System default setting and BMC default setting detection.
  //
  if (SecureBootKeyList->AdditionCount == SecureBootKeyList->TotalKeyCount) {
    //
    // When all keys are addition keys, system may be reset to default settings.
    //
    DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: system default setting detected. Clear collection\n", __func__));
    Status = RedfishClearCollection (Private->RedfishService, KeyCollectionUri);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: can not clear collection: %r\n", __func__, Status));
      return Status;
    }
  } else {
    //
    // When number of addition keys is less than the number of keys, several keys
    // are updated to BMC already. Check the member count from BMC to see if
    // BMC is reset to default or not. If member count is zero, we have to update
    // all keys to BMC again.
    //
    Status = RedfishGetCollectionCount (
               Private->RedfishService,
               KeyCollectionUri,
               &MemberCount
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: cannot get member count from %s: %r\n", __func__, KeyCollectionUri));
      return Status;
    }

    if (MemberCount == 0) {
      DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: BMC default setting detected. Upload all keys to BMC again\n", __func__));
      Status = RedfishSecureBootKeyReset (RedfishSecureBootKey, IsCertificate);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: can not reset secure boot keys: %r\n", __func__, Status));
        return Status;
      }

      //
      // Get key list again. Now all keys should be addition keys.
      //
      RedfishSecureBootKeyFreeKeyList (SecureBootKeyList);
      Status = RedfishSecureBootKeyGetKeyList (
                 RedfishSecureBootKey,
                 IsCertificate,
                 &SecureBootKeyList
                 );
      if (EFI_ERROR (Status) || (SecureBootKeyList == NULL)) {
        DEBUG ((DEBUG_ERROR, "%a: can not get Redfish secure boot key list: %r\n", __func__, Status));
        return Status;
      }

      DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: Total: %d Addition: %d Subtraction: %d\n", __func__, SecureBootKeyList->TotalKeyCount, SecureBootKeyList->AdditionCount, SecureBootKeyList->SubtractionCount));
      ASSERT (SecureBootKeyList->TotalKeyCount == SecureBootKeyList->AdditionCount);
    }
  }

  //
  // Post addition key to BMC
  //
  ASSERT (SecureBootKeyList->IsCertificate == IsCertificate);
  if (SecureBootKeyList->AdditionCount > 0) {
    DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: upload addition %d key to BMC\n", __func__, SecureBootKeyList->AdditionCount));
    for (Index = 0; Index < SecureBootKeyList->AdditionCount; Index++) {
      Status = RedfishPostSecureBootKey (Private->RedfishService, KeyCollectionUri, &SecureBootKeyList->AdditionList[Index], IsCertificate, &NewLocation);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: can not post Redfish secure boot key: %r\n", __func__, Status));
        break;
      }

      //
      // Update key URI
      //
      if (NewLocation == NULL) {
        DEBUG ((DEBUG_ERROR, "%a: No URI returned from BMC!!\n", __func__));
      } else {
        Status =  RedfishSecureBootKeyUpdateKey (RedfishSecureBootKey, SecureBootKeyList->AdditionList[Index].Id, IsCertificate, NewLocation);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: can not update URI %s to key %d: %r\n", __func__, NewLocation, SecureBootKeyList->AdditionList[Index].Id, Status));
          break;
        }
      }
    }
  }

  //
  // Delete subtraction key on BMC
  //
  if (SecureBootKeyList->SubtractionCount > 0) {
    DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: delete removed %d key to BMC\n", __func__, SecureBootKeyList->SubtractionCount));
    for (Index = 0; Index < SecureBootKeyList->SubtractionCount; Index++) {
      Status = RedfishDeleteKey (Private->RedfishService, &SecureBootKeyList->SubtractionList[Index]);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: can not delete Redfish secure boot key: %r\n", __func__, Status));
        break;
      }
    }
  }

  if (KeyCollectionUri != NULL) {
    FreePool (KeyCollectionUri);
  }

  if (SecureBootKeyList != NULL) {
    RedfishSecureBootKeyFreeKeyList (SecureBootKeyList);
  }

  return EFI_SUCCESS;
}

/**
  Provisioning redfish resource by given URI.

  @param[in]   This                Pointer to EFI_HP_REDFISH_HII_PROTOCOL instance.
  @param[in]   ResourceExist       TRUE if resource exists, PUT method will be used.
                                   FALSE if resource does not exist POST method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishProvisioningResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     BOOLEAN                          ResourceExist
  )
{
  EFI_STATUS                                Status;
  EFI_REDFISH_SECUREBOOTDATABASE_V1_0_1     *SecureBootDatabase;
  EFI_REDFISH_SECUREBOOTDATABASE_V1_0_1_CS  *SecureBootDatabaseCs;
  REDFISH_SECURE_BOOT_KEY                   *RedfishSecureBootKey;
  EFI_STRING                                SecureBootKeyName;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private->Json = JsonDumpString (RedfishJsonInPayload (Private->Payload), EDKII_JSON_COMPACT);
  if (IS_EMPTY_STRING (Private->Json)) {
    return EFI_OUT_OF_RESOURCES;
  }

  RedfishSecureBootKey = NULL;
  SecureBootKeyName    = NULL;
  SecureBootDatabase   = NULL;
  SecureBootDatabaseCs = NULL;

  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Private->Json,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&SecureBootDatabase
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  SecureBootDatabaseCs = SecureBootDatabase->SecureBootDatabase;

  //
  // Get secure boot variable name
  //
  SecureBootKeyName = StrAsciiToUnicode (SecureBootDatabaseCs->DatabaseId);
  if (SecureBootKeyName == NULL) {
    goto ON_RELEASE;
  }

  //
  // Open secure boot key
  //
  Status = RedfishSecureBootKeyOpen (
             SecureBootKeyName,
             SecureBootDatabaseCs->odata_id,
             &RedfishSecureBootKey
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not open Redfish secure boot key: %s: %r\n", __func__, SecureBootKeyName, Status));
    goto ON_RELEASE;
  }

  //
  // Provisioning certificate
  //
  Status = ProvisioningKeys (Private, SecureBootDatabaseCs, RedfishSecureBootKey, TRUE);
  if (EFI_ERROR (Status)) {
    if (Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "%a: provisioning certificate failed: %r\n", __func__, Status));
      goto ON_RELEASE;
    }
  }

  //
  // Provisioning signature
  //
  Status = ProvisioningKeys (Private, SecureBootDatabaseCs, RedfishSecureBootKey, FALSE);
  if (EFI_ERROR (Status)) {
    if (Status != EFI_NOT_FOUND) {
      DEBUG ((DEBUG_ERROR, "%a: provisioning signature failed: %r\n", __func__, Status));
      goto ON_RELEASE;
    }
  }

  DEBUG ((REDFISH_SECURE_BOOT_DATABASE_DEBUG, "%a: provisioning secure boot key is done successfully\n", __func__));

ON_RELEASE:

  //
  // Release resource.
  //
  if (SecureBootDatabase != NULL) {
    Private->JsonStructProtocol->DestoryStructure (
                                   Private->JsonStructProtocol,
                                   (EFI_REST_JSON_STRUCTURE_HEADER *)SecureBootDatabase
                                   );
  }

  if (Private->Json != NULL) {
    FreePool (Private->Json);
  }

  if (SecureBootKeyName != NULL) {
    FreePool (SecureBootKeyName);
  }

  if (RedfishSecureBootKey != NULL) {
    RedfishSecureBootKeyClose (RedfishSecureBootKey);
  }

  return EFI_SUCCESS;
}

/**
  Parse the TargetUri to get secure boot key name and see if this is certificate request or not.
  It's caller's responsibility to release KeyName by calling FreePool().

  @param[in]   TargetUri     Target URI from BMC payload.
  @param[out]  KeyName       Secure boot key name.
  @param[out]  IsCertificate TRUE if this is certificate request. FALSE otherwise.

  @retval EFI_SUCCESS              Secure boot key name is retrieved successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
GetKeyNameFromUri (
  IN CHAR8     *TargetUri,
  OUT CHAR8    **KeyName,
  OUT BOOLEAN  *IsCertificate
  )
{
  CHAR8  *StrStart;
  CHAR8  *StrEnd;
  UINTN  StringSize;

  if (IS_EMPTY_STRING (TargetUri) || (KeyName == NULL) || (IsCertificate == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *IsCertificate = TRUE;
  *KeyName       = NULL;
  StrStart       = NULL;
  StrEnd         = NULL;

  //
  // Key name
  //
  StrStart = AsciiStrStr (TargetUri, REDFISH_TASK_TARGET_URI_SEARCH);
  if (StrStart == NULL) {
    return EFI_NOT_FOUND;
  }

  StrStart += 20;

  StrEnd = AsciiStrStr (StrStart, "/");
  if (StrEnd == NULL) {
    return EFI_NOT_FOUND;
  }

  StringSize = StrEnd - StrStart + 1;
  ASSERT (StringSize != 0);

  *KeyName = AllocateZeroPool (StringSize);
  if (*KeyName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AsciiStrnCpyS (*KeyName, StringSize, StrStart, (StringSize - 1));

  //
  // Find key type
  //
  StrStart = StrEnd + 1;
  if (*StrStart == '\0') {
    return EFI_DEVICE_ERROR;
  }

  if (*StrStart == 'C') {
    //
    // This is 'Certificates'
    //
    *IsCertificate = TRUE;
  } else if (*StrStart == 'S') {
    //
    // This is 'Signatures'
    //
    *IsCertificate = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  Parse the task payload and get task request.

  @param[in,out]   TaskRequest     On input, it carries task payload read from BMC.
                                   On output, secure boot action is retrieved.

  @retval EFI_SUCCESS              Task request is retrieved successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
GetTaskRequest (
  IN OUT REDFISH_SECURE_BOOT_TASK_REQUEST  *TaskRequest
  )
{
  EDKII_JSON_VALUE  JsonBodyObj;
  EDKII_JSON_VALUE  BufferObj;
  CONST CHAR8       *BufferString;
  CHAR8             *KeyName;
  EFI_STATUS        Status;

  if ((TaskRequest == NULL) || (TaskRequest->Payload == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  JsonBodyObj  = NULL;
  BufferObj    = NULL;
  BufferString = NULL;
  KeyName      = NULL;

  //
  // Determinate the request type
  //
  if (TaskRequest->Payload->HttpOperation == HttpMethodDelete) {
    TaskRequest->RequestType = RedfishTaskRequestDelete;
  } else if (TaskRequest->Payload->HttpOperation == HttpMethodPost) {
    if (AsciiStrStr (TaskRequest->Payload->TargetUri, REDFISH_TASK_ACTION_KEYWORD) == NULL) {
      TaskRequest->RequestType = RedfishTaskRequestEnroll;
    } else {
      //
      // If this is Action, we need to check JsonBody for the parameters.
      // Use RedfishTaskRequestDeleteAll as default type first.
      //
      TaskRequest->RequestType = RedfishTaskRequestDeleteAll;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  //
  // Get key name
  //
  Status = GetKeyNameFromUri (TaskRequest->Payload->TargetUri, &KeyName, &TaskRequest->IsCertificate);
  if (EFI_ERROR (Status)) {
    goto ON_RELEASE;
  }

  TaskRequest->KeyName = StrAsciiToUnicode (KeyName);
  if (TaskRequest->KeyName == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  if (TaskRequest->Payload->HttpOperation == HttpMethodDelete) {
    Status = EFI_SUCCESS;
    goto ON_RELEASE;
  }

  //
  // Check JsonBody for actions and enrollment.
  //
  JsonBodyObj = JsonLoadString (TaskRequest->Payload->JsonBody, 0, NULL);
  if (JsonBodyObj == NULL) {
    return EFI_VOLUME_CORRUPTED;
  }

  if (TaskRequest->RequestType == RedfishTaskRequestDeleteAll) {
    BufferObj = JsonObjectGetValue (JsonValueGetObject (JsonBodyObj), REDFISH_TASK_ACTION_ATTRIBUTE);
    if (BufferObj == NULL) {
      Status = EFI_DEVICE_ERROR;
      goto ON_RELEASE;
    }

    BufferString = JsonValueGetAsciiString (BufferObj);
    if (IS_EMPTY_STRING (BufferString)) {
      Status = EFI_DEVICE_ERROR;
      goto ON_RELEASE;
    }

    if (AsciiStrCmp (BufferString, REDFISH_TASK_ACTION_DELETE_ALL) == 0) {
      TaskRequest->RequestType = RedfishTaskRequestDeleteAll;
    } else {
      TaskRequest->RequestType = RedfishTaskRequestResetAll;
    }
  } else if (TaskRequest->RequestType == RedfishTaskRequestEnroll) {
    if (TaskRequest->IsCertificate) {
      //
      // certificate type
      //
      BufferObj = JsonObjectGetValue (JsonValueGetObject (JsonBodyObj), REDFISH_CERTIFICATE_TYPE);
      if (BufferObj == NULL) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      BufferString = JsonValueGetAsciiString (BufferObj);
      if (IS_EMPTY_STRING (BufferString)) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      TaskRequest->KeyType = AllocateCopyPool (AsciiStrSize (BufferString), BufferString);
      if (TaskRequest->KeyType == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_RELEASE;
      }

      //
      // certificate string
      //
      BufferObj = JsonObjectGetValue (JsonValueGetObject (JsonBodyObj), REDFISH_CERTIFICATE_STRING);
      if (BufferObj == NULL) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      BufferString = JsonValueGetAsciiString (BufferObj);
      if (IS_EMPTY_STRING (BufferString)) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      TaskRequest->KeyString = AllocateCopyPool (AsciiStrSize (BufferString), BufferString);
      if (TaskRequest->KeyString == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_RELEASE;
      }
    } else {
      //
      // signature type
      //
      BufferObj = JsonObjectGetValue (JsonValueGetObject (JsonBodyObj), REDFISH_SIGNATURE_TYPE);
      if (BufferObj == NULL) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      BufferString = JsonValueGetAsciiString (BufferObj);
      if (IS_EMPTY_STRING (BufferString)) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      TaskRequest->KeyType = AllocateCopyPool (AsciiStrSize (BufferString), BufferString);
      if (TaskRequest->KeyType == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_RELEASE;
      }

      //
      // signature string
      //
      BufferObj = JsonObjectGetValue (JsonValueGetObject (JsonBodyObj), REDFISH_SIGNATURE_STRING);
      if (BufferObj == NULL) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      BufferString = JsonValueGetAsciiString (BufferObj);
      if (IS_EMPTY_STRING (BufferString)) {
        Status = EFI_DEVICE_ERROR;
        goto ON_RELEASE;
      }

      TaskRequest->KeyString = AllocateCopyPool (AsciiStrSize (BufferString), BufferString);
      if (TaskRequest->KeyString == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ON_RELEASE;
      }
    }
  }

  Status = EFI_SUCCESS;

ON_RELEASE:

  if (JsonBodyObj != NULL) {
    JsonValueFree (JsonBodyObj);
  }

  if (KeyName != NULL) {
    FreePool (KeyName);
  }

  return Status;
}

/**
  Report failure to task so user knows why the task is failed.

  @param[in]   TaskId      The task ID to report failure.
  @param[in]   Message     String message of detail failure.

  @retval EFI_SUCCESS              Failure is reported.
  @retval Others                   Some error happened.

**/
EFI_STATUS
ReportTaskFailure (
  IN UINTN  TaskId,
  IN CHAR8  *Message
  )
{
  EFI_STATUS  Status;

  if (mRedfishTaskProtocol == NULL) {
    return EFI_NOT_READY;
  }

  Status = mRedfishTaskProtocol->ReportMessage (
                                   mRedfishTaskProtocol,
                                   TaskId,
                                   Message,
                                   RedfishMessageSeverityCritical
                                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot report Redfish message: %r\n", __func__, Status));
  }

  return Status;
}

/**
  The callback function provided by Redfish feature driver.

  @param[in]     TaskId              The Task ID.
  @param[in]     JsonText            The context of task resource in JSON format.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] Result              The pointer to REDFISH_TASK_RESULT.
                                     On input, it is "TaskState" and "TaskStatus" in task resource.
                                     On ouput, it is "TaskState" and "TaskStatus" that will be update
                                     to task resource.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskCallback (
  IN     UINTN                TaskId,
  IN     CHAR8                *JsonText,
  IN     VOID                 *Context,
  IN OUT REDFISH_TASK_RESULT  *Result
  )
{
  REDFISH_RESOURCE_COMMON_PRIVATE   *Private;
  REDFISH_SECURE_BOOT_TASK_REQUEST  TaskRequest;
  EFI_STATUS                        Status;
  CHAR8                             *ParsedKeyString;

  if (IS_EMPTY_STRING (JsonText) || (Context == NULL) || (Result == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_TASK_DEBUG, "%a: receive task %d\n", __func__, TaskId));

  ZeroMem (&TaskRequest, sizeof (TaskRequest));
  ParsedKeyString     = NULL;
  TaskRequest.Payload = NULL;
  Private             = (REDFISH_RESOURCE_COMMON_PRIVATE *)Context;

  if (mRedfishTaskProtocol == NULL) {
    return EFI_NOT_READY;
  }

  //
  // Get task payload
  //
  Status = mRedfishTaskProtocol->GetPayload (mRedfishTaskProtocol, JsonText, &TaskRequest.Payload);
  if (EFI_ERROR (Status)) {
    //
    // Report failure
    //
    ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_INTERNAL_MESSAGE);
    DEBUG ((DEBUG_ERROR, "%a: can not get task payload: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_SECURE_BOOT_TASK_DEBUG, "%a: handle %a method: 0x%x\n", __func__, TaskRequest.Payload->TargetUri, TaskRequest.Payload->HttpOperation));

  Status = GetTaskRequest (&TaskRequest);
  if (EFI_ERROR (Status)) {
    //
    // Report failure
    //
    ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_INTERNAL_MESSAGE);
    DEBUG ((DEBUG_ERROR, "%a: failed to get task request: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_SECURE_BOOT_TASK_DEBUG, "%a: Certificate: 0x%x Key: %s Request: 0x%x Type: %a\n", __func__, TaskRequest.IsCertificate, TaskRequest.KeyName, TaskRequest.RequestType, TaskRequest.KeyType));
  if ((TaskRequest.RequestType == RedfishTaskRequestEnroll)) {
    if (TaskRequest.IsCertificate) {
      if (AsciiStrCmp (TaskRequest.KeyType, REDFISH_TASK_SUPPORTED_CERTIFICATE_TYPE) != 0) {
        //
        // Report failure
        //
        ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_WRONG_CERT_FORMAT_MESSAGE);
        DEBUG ((DEBUG_ERROR, "%a: only support PEM type of certificate\n", __func__));
        Status = EFI_UNSUPPORTED;
        goto ON_RELEASE;
      }

      //
      // JsonDumpString() will convert "\n" to "\\n" but this is not valid character in PEM format.
      //
      Status = RedfishRemoveEscapeCharacter (TaskRequest.KeyString, &ParsedKeyString);
      if (EFI_ERROR (Status) || (ParsedKeyString == NULL)) {
        DEBUG ((DEBUG_ERROR, "%a: can not remove escape string: %r\n", __func__, Status));
        goto ON_RELEASE;
      }

      FreePool (TaskRequest.KeyString);
      TaskRequest.KeyString = ParsedKeyString;
    } else {
      if (AsciiStrCmp (TaskRequest.KeyType, REDFISH_TASK_SUPPORTED_SIGNATURE_TYPE) != 0) {
        //
        // Report failure
        //
        ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_WRONG_SIG_FORMAT_MESSAGE);
        DEBUG ((DEBUG_ERROR, "%a: only support EFI_CERT_SHA256_GUID type of signature\n", __func__));
        Status = EFI_UNSUPPORTED;
        goto ON_RELEASE;
      }
    }
  }

  //
  // Perform request
  //
  switch (TaskRequest.RequestType) {
    case RedfishTaskRequestEnroll:
      Status = RedfishSecureBootEnrollKey (TaskRequest.KeyName, TaskRequest.KeyType, TaskRequest.KeyString);
      break;
    case RedfishTaskRequestDelete:
      Status = RedfishSecureBootDeleteKey (TaskRequest.KeyName, TaskRequest.Payload->TargetUri);
      break;
    case RedfishTaskRequestDeleteAll:
      Status = RedfishSecureBootDeleteAllKeys (TaskRequest.KeyName);
      break;
    case RedfishTaskRequestResetAll:
      Status = RedfishSecureBootResetAllKeys (TaskRequest.KeyName);
      break;
    default:
      DEBUG ((DEBUG_ERROR, "%a: unsupported request: 0x%x\n", __func__, TaskRequest.RequestType));
      Status = EFI_UNSUPPORTED;
      break;
  }

  if (EFI_ERROR (Status)) {
    //
    // Report failure
    //
    if (Status == EFI_NOT_FOUND) {
      ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_NOT_FOUND_MESSAGE);
    } else if (Status == EFI_UNSUPPORTED) {
      ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_UNSUPPORTED_MESSAGE);
    } else if ((Status == EFI_ACCESS_DENIED) || (Status == EFI_SECURITY_VIOLATION) || (Status == EFI_WRITE_PROTECTED)) {
      ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_ACCESS_DENIED_MESSAGE);
    }

    ReportTaskFailure (TaskId, REDFISH_TASK_FAILURE_GENERIC_MESSAGE);
    DEBUG ((DEBUG_ERROR, "%a: perform request: 0x%x to %s failed: %r\n", __func__, TaskRequest.RequestType, TaskRequest.KeyName, Status));
  }

ON_RELEASE:

  //
  // Report task result
  //
  if (EFI_ERROR (Status)) {
    Result->TaskState  = RedfishTaskStateException;
    Result->TaskStatus = RedfishTaskStatusCritical;
  } else {
    Result->TaskState  = RedfishTaskStateCompleted;
    Result->TaskStatus = RedfishTaskStatusOk;
  }

  if (TaskRequest.Payload != NULL) {
    mRedfishTaskProtocol->FreePayload (mRedfishTaskProtocol, TaskRequest.Payload);
  }

  if (TaskRequest.KeyName != NULL) {
    FreePool (TaskRequest.KeyName);
  }

  if (TaskRequest.KeyType != NULL) {
    FreePool (TaskRequest.KeyType);
  }

  if (TaskRequest.KeyString != NULL) {
    FreePool (TaskRequest.KeyString);
  }

  return Status;
}
