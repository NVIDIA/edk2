/** @file
  Redfish secure boot keys driver to track secure boot keys between system boots.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "RedfishSecureBootKeysDxe.h"
#include "RedfishSecureBootKeyOp.h"

/**

  This function get current time on system.

  @param[in]  Time    Pointer to time instance.

  @retval     EFI_SUCCESS         Time is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootGetTime (
  IN EFI_TIME  *Time
  )
{
  EFI_STATUS  Status;

  ZeroMem (Time, sizeof (EFI_TIME));
  Status = gRT->GetTime (Time, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot get current time: %r\n", __func__, Status));
    return Status;
  }

  Time->Pad1       = 0;
  Time->Nanosecond = 0;
  Time->TimeZone   = 0;
  Time->Daylight   = 0;
  Time->Pad2       = 0;

  return EFI_SUCCESS;
}

/**
  Convert GUID to ASCII string.

  @param[in]     Guid          Pointer to GUID to print.

  @retval    CHAR8 *   GUID in ASCII string returned.
  @retval    NULL      Error occurs.

**/
CHAR8 *
RfSecureBootGuidToString (
  IN  EFI_GUID  *Guid
  )
{
  CHAR8  *GuidStr;
  UINTN  StrSize;

  if (Guid == NULL) {
    return NULL;
  }

  StrSize = sizeof (CHAR8) * REDFISH_SECURE_BOOT_GUID_STR_SIZE;
  GuidStr = AllocatePool (StrSize);
  if (GuidStr == NULL) {
    return NULL;
  }

  AsciiSPrint (
    GuidStr,
    StrSize,
    "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    (UINTN)Guid->Data1,
    (UINTN)Guid->Data2,
    (UINTN)Guid->Data3,
    (UINTN)Guid->Data4[0],
    (UINTN)Guid->Data4[1],
    (UINTN)Guid->Data4[2],
    (UINTN)Guid->Data4[3],
    (UINTN)Guid->Data4[4],
    (UINTN)Guid->Data4[5],
    (UINTN)Guid->Data4[6],
    (UINTN)Guid->Data4[7]
    );

  return GuidStr;
}

/**

  This function converts certificate from PEM format to DER format.
  It's call responsibility to release PemStr by calling FreePool().

  @param[in]  CertData      Certificate binary in DER format.
  @param[in]  CertDataSize  The size of CertData.
  @param[out] PemStr        Certificate in PEM format.

  @retval     EFI_SUCCESS   Certificate is converted to PEM format.
  @retval     Others        Errors occur.

**/
EFI_STATUS
RfSecureBootPemToDer (
  IN CHAR8   *PemStr,
  OUT UINTN  *DerSize,
  OUT UINT8  **DerData
  )
{
  UINTN       StrLen;
  EFI_STATUS  Status;
  UINTN       DataSize;
  UINT8       *Buffer;
  CHAR8       *StrStart;
  CHAR8       *StrEnd;

  if (IS_EMPTY_STRING (PemStr) || (DerSize == NULL) || (DerData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DataSize = 0;
  *DerSize = 0;
  *DerData = NULL;
  StrStart = NULL;
  StrEnd   = NULL;

  StrStart = AsciiStrStr (PemStr, REDFISH_SECURE_BOOT_PEM_PREFIX);
  if (StrStart == NULL) {
    return EFI_VOLUME_CORRUPTED;
  }

  StrEnd = AsciiStrStr (PemStr, REDFISH_SECURE_BOOT_PEM_POSTFIX);
  if (StrEnd == NULL) {
    return EFI_VOLUME_CORRUPTED;
  }

  StrStart += (sizeof (REDFISH_SECURE_BOOT_PEM_PREFIX) - 1);
  *StrEnd   = '\0';
  StrLen    = AsciiStrLen (StrStart);
  if (StrLen == 0) {
    return EFI_DEVICE_ERROR;
  }

  Status = Base64Decode (StrStart, StrLen, NULL, &DataSize);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (DataSize == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: Base64Decode cannot get size: %r\n", __func__, Status));
    return Status;
  }

  Buffer = AllocatePool (DataSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = Base64Decode (StrStart, StrLen, Buffer, &DataSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Base64Decode: %r\n", __func__, Status));
    return Status;
  }

  *DerSize = DataSize;
  *DerData = Buffer;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS  Status;
  UINTN       DestSize;
  CHAR8       *DestStr;
  CHAR8       *PemData;
  UINTN       PemDataSize;

  if ((CertData == NULL) || (CertDataSize == 0) || (PemStr == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *PemStr  = NULL;
  DestStr  = NULL;
  DestSize = 0;
  Status   = Base64Encode (
               CertData,
               CertDataSize,
               DestStr,
               &DestSize
               );
  if ((Status != EFI_BUFFER_TOO_SMALL) || (DestSize == 0)) {
    return Status;
  }

  DestStr = AllocatePool (DestSize);
  if (DestStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = Base64Encode (
             CertData,
             CertDataSize,
             DestStr,
             &DestSize
             );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  PemDataSize = AsciiStrLen (REDFISH_SECURE_BOOT_PEM_PREFIX) + AsciiStrLen (REDFISH_SECURE_BOOT_PEM_POSTFIX) + DestSize + 5; // two \\n and one \0
  PemData     = AllocatePool (PemDataSize);
  if (PemData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_ERROR;
  }

  AsciiSPrint (PemData, PemDataSize, "%a\\n%a\\n%a", REDFISH_SECURE_BOOT_PEM_PREFIX, DestStr, REDFISH_SECURE_BOOT_PEM_POSTFIX);
  *PemStr = PemData;

ON_ERROR:

  if (DestStr != NULL) {
    FreePool (DestStr);
  }

  return Status;
}

/**

  This function enroll secure boot key from input data.

  @param[in]  KeyInfo    Key variable information.
  @param[in]  DataSize   The size of data.
  @param[in]  Data       Pointer to data.

  @retval     EFI_SUCCESS         Key is enrolled successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootEnrollKeyFromData (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *KeyInfo,
  IN UINTN                          DataSize,
  IN VOID                           *Data
  )
{
  EFI_STATUS  Status;

  if ((KeyInfo == NULL) || (DataSize == 0) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EnrollFromInput (
             KeyInfo->VariableName,
             KeyInfo->VariableGuid,
             DataSize,
             Data
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot enroll secure boot variable: %s: %r\n", __func__, KeyInfo->VariableName, Status));
    return Status;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                     Status;
  EFI_SIGNATURE_LIST             *CertList;
  EFI_SIGNATURE_DATA             *Cert;
  UINT8                          *VarData;
  UINTN                          VarSize;
  UINTN                          CertDataSize;
  UINTN                          SigCount;
  UINTN                          SigListCount;
  UINTN                          Index;
  SECURE_BOOT_KEY_VARIABLE_INFO  *KeyVarInfo;

  if (KeyInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KeyVarInfo = KeyInfo->KeyVarInfo;
  DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: load variable: %s\n", __func__, KeyVarInfo->VariableName));

  Status = GetVariable2 (
             KeyVarInfo->VariableName,
             KeyVarInfo->VariableGuid,
             (VOID **)&VarData,
             &VarSize
             );
  if (EFI_ERROR (Status) || (VarSize == 0)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: %s does not exist: %r\n", __func__, KeyVarInfo->VariableName, Status));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: cannot find %s: %r\n", __func__, KeyVarInfo->VariableName, Status));
    }

    return Status;
  }

  //
  // Enumerate all data.
  //
  SigListCount = 0;
  CertDataSize = VarSize;
  CertList     = (EFI_SIGNATURE_LIST *)VarData;

  while ((CertDataSize > 0) && (CertDataSize >= CertList->SignatureListSize)) {
    SigCount = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    DEBUG_CODE (
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: SignatureType: %g\n", __func__, &CertList->SignatureType));
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: SignatureListSize: 0x%x\n", __func__, CertList->SignatureListSize));
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: SignatureHeaderSize: 0x%x\n", __func__, CertList->SignatureHeaderSize));
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: SignatureSize: 0x%x\n", __func__, CertList->SignatureSize));
      DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: Signature cout: 0%d\n", __func__, SigCount));
      );
    for (Index = 0; Index < SigCount; Index++) {
      Cert = (EFI_SIGNATURE_DATA *)((UINT8 *)CertList
                                    + sizeof (EFI_SIGNATURE_LIST)
                                    + CertList->SignatureHeaderSize
                                    + Index * CertList->SignatureSize);
      DEBUG_CODE (
        DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: %d) SignatureOwner: %g\n", __func__, Index, &Cert->SignatureOwner));
        );

      Status = RfSecureBootKeyAddDataFromVariable (KeyInfo, KeyVarInfo->Uri, &CertList->SignatureType, Cert, CertList->SignatureSize);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: cannot add key from variable: %r\n", __func__, Status));
        break;
      }
    }

    SigListCount += 1;
    CertDataSize -= CertList->SignatureListSize;
    CertList      = (EFI_SIGNATURE_LIST *)((UINT8 *)CertList + CertList->SignatureListSize);
  }

  DEBUG_CODE (
    DEBUG ((REDFISH_SECURE_BOOT_VARIABLE_DEBUG, "%a: Signature list count: %d\n", __func__, SigListCount));
    );

  FreePool (VarData);

  return EFI_SUCCESS;
}

/**

  This function delete secure boot variable from given variable name
  and GUID.

  @param[in]  KeyInfo    Key variable information.

  @retval     EFI_SUCCESS         Key variable is deleted successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootDeleteVariable (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo
  )
{
  EFI_STATUS  Status;

  if (VarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (VarInfo->Type) {
    case RedfishSecureBootKeyPk:
      Status = DisablePKProtection ();
      if (!EFI_ERROR (Status)) {
        Status = DeletePlatformKey ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: DisablePKProtection: %r\n", __func__, Status));
      }

      break;
    case RedfishSecureBootKeyKek:
      Status = DeleteKEK ();
      break;
    case RedfishSecureBootKeyDb:
      Status = DeleteDb ();
      break;
    case RedfishSecureBootKeyDbt:
      Status = DeleteDbt ();
      break;
    case RedfishSecureBootKeyDbx:
      Status = DeleteDbx ();
      break;
    default:
      return EFI_UNSUPPORTED;
  }

  return Status;
}

/**

  This function enroll single certificate or signature to secure
  boot variable.

  @param[in]  VarInfo       Key variable information.
  @param[in]  KeyType       The type of key to enroll.
  @param[in]  CertData      Key data.
  @param[in]  CertDataSize  The size of key data.

  @retval     EFI_SUCCESS         Key is enrolled successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootEnrollSingleKey (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo,
  IN EFI_GUID                       *KeyType,
  IN UINT8                          *CertData,
  IN UINTN                          CertDataSize
  )
{
  UINTN               SigListSize;
  EFI_SIGNATURE_DATA  *SigData;
  EFI_SIGNATURE_LIST  *SigList;
  EFI_STATUS          Status;
  UINT32              VariableAttribute;
  EFI_TIME            CurrentTime;
  UINTN               VarSize;

  if ((VarInfo == NULL) || (KeyType == NULL) || (CertData == NULL) || (CertDataSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: enroll certificate: %s size: 0x%x type: %g\n", __func__, VarInfo->VariableName, CertDataSize, KeyType));

  //
  // Initialization
  //
  SigListSize       = 0;
  VarSize           = 0;
  SigData           = NULL;
  SigList           = NULL;
  VariableAttribute = EFI_VARIABLE_NON_VOLATILE |
                      EFI_VARIABLE_RUNTIME_ACCESS |
                      EFI_VARIABLE_BOOTSERVICE_ACCESS |
                      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |
                      EFI_VARIABLE_APPEND_WRITE;

  //
  // Prepare signature list
  //
  SigListSize = sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + CertDataSize;
  SigList     = (EFI_SIGNATURE_LIST *)AllocateZeroPool (SigListSize);
  if (SigList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SigList->SignatureListSize   = (UINT32)SigListSize;
  SigList->SignatureHeaderSize = 0;
  SigList->SignatureSize       = (UINT32)(sizeof (EFI_SIGNATURE_DATA) - 1 + CertDataSize);
  CopyGuid (&SigList->SignatureType, KeyType);

  SigData = (EFI_SIGNATURE_DATA *)((UINT8 *)SigList + sizeof (EFI_SIGNATURE_LIST));

  //
  // TODO: we don't get signature owner GUID from Redfish
  //
  CopyGuid (&SigData->SignatureOwner, VarInfo->VariableGuid);
  CopyMem (SigData->SignatureData, CertData, CertDataSize);

  //
  // There is only one key in PK
  //
  if (VarInfo->Type == RedfishSecureBootKeyPk) {
    //
    // Check to see if we have PK already
    //
    Status = gRT->GetVariable (
                    VarInfo->VariableName,
                    VarInfo->VariableGuid,
                    NULL,
                    &VarSize,
                    NULL
                    );
    if (Status != EFI_NOT_FOUND) {
      Status = EFI_SECURITY_VIOLATION;
      DEBUG ((DEBUG_ERROR, "%a: There is PK in system already. We cannot enroll multiple PK in system: %r\n", __func__, VarInfo->VariableName, Status));
      goto ON_RELEASE;
    }

    Status = EnrollFromInput (
               VarInfo->VariableName,
               VarInfo->VariableGuid,
               SigListSize,
               (VOID *)SigList
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: cannot enroll secure boot variable: %s: %r\n", __func__, VarInfo->VariableName, Status));
      goto ON_RELEASE;
    }

    DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: enroll certificate: %s successfully\n", __func__, VarInfo->VariableName));
  } else {
    //
    // Check to see if key already exists or not
    //
    Status = gRT->GetVariable (
                    VarInfo->VariableName,
                    VarInfo->VariableGuid,
                    NULL,
                    &VarSize,
                    NULL
                    );
    if (Status == EFI_NOT_FOUND) {
      //
      // There is no key in system, create new one.
      //
      Status = EnrollFromInput (
                 VarInfo->VariableName,
                 VarInfo->VariableGuid,
                 SigListSize,
                 (VOID *)SigList
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: cannot enroll secure boot variable: %s: %r\n", __func__, VarInfo->VariableName, Status));
        goto ON_RELEASE;
      }

      DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: enroll certificate: %s successfully\n", __func__, VarInfo->VariableName));
    } else if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // There is key in system, append new key to variable.
      //
      Status = RfSecureBootGetTime (&CurrentTime);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: cannot get current time: %r", __func__, Status));
        goto ON_RELEASE;
      }

      Status = CreateTimeBasedPayload (&SigListSize, (UINT8 **)&SigList, &CurrentTime);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: cannot create time-based payload: %r", Status));
        goto ON_RELEASE;
      }

      Status = gRT->SetVariable (
                      VarInfo->VariableName,
                      VarInfo->VariableGuid,
                      VariableAttribute,
                      SigListSize,
                      SigList
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: cannot set variable: %s: %r", __func__, VarInfo->VariableName, Status));
        goto ON_RELEASE;
      }

      DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: enroll certificate: %s successfully\n", __func__, VarInfo->VariableName));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: failed to get variable: %s: %r\n", __func__, VarInfo->VariableName, Status));
    }
  }

ON_RELEASE:

  FreePool (SigList);

  return Status;
}

/**

  This function remove single certificate or signature from existing
  secure boot variable.

  @param[in]  VarInfo       Key variable information.
  @param[in]  HashToDelete  Hash of targe key to delete.

  @retval     EFI_SUCCESS         Key is deleted successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RfSecureBootDeleteSingleKey (
  IN SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo,
  IN CHAR8                          *HashToDelete
  )
{
  EFI_STATUS          Status;
  EFI_SIGNATURE_LIST  *CertList;
  EFI_SIGNATURE_DATA  *Cert;
  UINT8               *VarData;
  UINTN               VarSize;
  UINTN               CertDataSize;
  UINTN               SigCount;
  UINTN               Index;
  UINT8               *HashCtx;
  UINTN               HashCtxSize;
  CHAR8               *HashStr;
  BOOLEAN             KeyDeleted;
  UINT8               *NewData;
  UINTN               NewDataSize;
  EFI_SIGNATURE_LIST  *NewCertList;
  UINTN               NewDataOffset;
  UINTN               NewSigCount;
  UINTN               ThisCertOffset;
  UINTN               SigListCount;
  UINTN               SigDataCount;

  if ((VarInfo == NULL) || IS_EMPTY_STRING (HashToDelete)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: delete key: %a in %s\n", __func__, HashToDelete, VarInfo->VariableName));

  VarData        = NULL;
  VarSize        = 0;
  HashCtxSize    = 0;
  HashCtx        = NULL;
  HashStr        = NULL;
  NewData        = NULL;
  NewDataSize    = 0;
  NewDataOffset  = 0;
  ThisCertOffset = 0;
  NewSigCount    = 0;
  SigListCount   = 0;
  SigDataCount   = 0;
  NewCertList    = NULL;
  KeyDeleted     = FALSE;

  //
  // Go through secure boot variable and find desired key to delete.
  //
  Status = GetVariable2 (
             VarInfo->VariableName,
             VarInfo->VariableGuid,
             (VOID **)&VarData,
             &VarSize
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot find %s: %r\n", __func__, VarInfo->VariableName, Status));
    return Status;
  }

  //
  // Allocate new buffer to keep variable after delete key
  //
  NewData = AllocatePool (VarSize);
  if (NewData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  //
  // Enumerate all data.
  //
  CertDataSize = VarSize;
  CertList     = (EFI_SIGNATURE_LIST *)VarData;

  while ((CertDataSize > 0) && (CertDataSize >= CertList->SignatureListSize)) {
    //
    // Copy EFI_SIGNATURE_LIST from old buffer to new buffer.
    // Keep current offset because we may need to:
    //  1) update signature list size if we delete key in this signature list.
    //  2) remove this signature list when there is no key left.
    //
    CopyMem (NewData + NewDataOffset, CertList, (sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize));
    ThisCertOffset = NewDataOffset;
    NewDataOffset += (sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
    SigListCount  += 1;
    SigDataCount   = 0;

    //
    // Go through EFI_SIGNATURE_DATA array
    //
    SigCount = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    for (Index = 0; Index < SigCount; Index++) {
      SigDataCount += 1;
      Cert          = (EFI_SIGNATURE_DATA *)((UINT8 *)CertList
                                             + sizeof (EFI_SIGNATURE_LIST)
                                             + CertList->SignatureHeaderSize
                                             + Index * CertList->SignatureSize);
      //
      // We only delete one key in this function. If key is deleted,
      // we don't do the check to speed up this function.
      //
      if (!KeyDeleted) {
        //
        // Get the hash of this key
        //
        Status = RfHashSecureBootKey ((VOID *)Cert, CertList->SignatureSize, &HashCtx, &HashCtxSize);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: cannot get hash of key: %r\n", __func__, Status));
          continue;
        }

        HashStr = RfBlobToHexString (HashCtx, HashCtxSize);
        if (HashStr == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto ON_RELEASE;
        }

        if (AsciiStrCmp (HashStr, HashToDelete) == 0) {
          DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: key to delete is found in %d-th list and %d-th data array\n", __func__, SigListCount, SigDataCount));
          KeyDeleted = TRUE;
          //
          // Skip copying this EFI_SIGNATURE_DATA to new data. We have to update
          // SignatureListSize of this signature list.
          //
          NewCertList                     = (EFI_SIGNATURE_LIST *)(NewData + ThisCertOffset);
          NewCertList->SignatureListSize -= CertList->SignatureSize;
        } else {
          //
          // Copy EFI_SIGNATURE_DATA from old buffer to new buffer
          //
          CopyMem (NewData + NewDataOffset, Cert, CertList->SignatureSize);
          NewDataOffset += CertList->SignatureSize;
        }

        FreePool (HashCtx);
        FreePool (HashStr);
      } else {
        //
        // Copy EFI_SIGNATURE_DATA from old buffer to new buffer
        //
        CopyMem (NewData + NewDataOffset, Cert, CertList->SignatureSize);
        NewDataOffset += CertList->SignatureSize;
      }
    }

    //
    // Let's check the signature array count in new data after for-loop. If we remove the key and the array count is 0,
    // we have to drop this signature list.
    //
    NewCertList = (EFI_SIGNATURE_LIST *)(NewData + ThisCertOffset);
    NewSigCount = (NewCertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - NewCertList->SignatureHeaderSize) / NewCertList->SignatureSize;
    DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: Signature count: old %d new %d\n", __func__, SigCount, NewSigCount));
    if (NewSigCount == 0) {
      //
      // There is no EFI_SIGNATURE_DATA in this list anymore. Reset NewDataOffset to drop this signature list.
      //
      NewDataOffset = ThisCertOffset;
    }

    CertDataSize -= CertList->SignatureListSize;
    CertList      = (EFI_SIGNATURE_LIST *)((UINT8 *)CertList + CertList->SignatureListSize);
  }

  if (!KeyDeleted) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  NewDataSize = NewDataOffset;
  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: old data size: 0x%x new data size: 0x%x\n", __func__, VarSize, NewDataSize));

  if (NewDataSize == 0) {
    //
    // There is nothing in this variable. Delete it
    //
    Status = RfSecureBootDeleteVariable (VarInfo);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: delete %s failed: %r\n", __func__, VarInfo->VariableName, Status));
    } else {
      DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: delete key: %a successfully\n", __func__, HashToDelete, VarInfo->VariableName));
    }

    goto ON_RELEASE;
  }

  //
  // Enroll new data
  //
  Status = EnrollFromInput (
             VarInfo->VariableName,
             VarInfo->VariableGuid,
             NewDataSize,
             (VOID *)NewData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot enroll secure boot variable: %s: %r\n", __func__, VarInfo->VariableName, Status));
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: delete key: %a successfully\n", __func__, HashToDelete, VarInfo->VariableName));

ON_RELEASE:

  if (VarData != NULL) {
    FreePool (VarData);
  }

  if (NewData != NULL) {
    FreePool (NewData);
  }

  return Status;
}

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
  )
{
  SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo;
  BOOLEAN                        IsCertificate;
  EFI_STATUS                     Status;
  UINTN                          KeySize;
  UINT8                          *KeyData;
  EFI_GUID                       KeyTypeGuid;

  if (IS_EMPTY_STRING (KeyName) || IS_EMPTY_STRING (KeyType) || IS_EMPTY_STRING (KeyString)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: enroll key: %s\n", __func__, KeyName));
  KeySize = 0;
  KeyData = NULL;
  VarInfo = RfGetSecureBootKeyInfo (KeyName);
  if (VarInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // We can not modify default keys
  //
  if (VarInfo->IsDefaultKey) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Get key type
  //
  Status = RfGetSignatureTypeGuid (KeyType, &KeyTypeGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unknown type: %a\n", __func__, KeyType));
    return Status;
  }

  //
  // We only support gEfiCertX509Guid and gEfiCertSha256Guid now
  //
  if (!CompareGuid (&KeyTypeGuid, &gEfiCertX509Guid) && !CompareGuid (&KeyTypeGuid, &gEfiCertSha256Guid)) {
    DEBUG ((DEBUG_ERROR, "%a: unsupported type: %a\n", __func__, KeyType));
    return EFI_UNSUPPORTED;
  }

  if (CompareGuid (&KeyTypeGuid, &gEfiCertX509Guid)) {
    IsCertificate = TRUE;
  } else {
    IsCertificate = FALSE;
  }

  if (IsCertificate) {
    //
    // Convert PEM to DER format
    //
 #if REDFISH_SECURE_BOOT_KEY_DUMP
    RfDebugDumpString (DEBUG_ERROR, "Certificate string:", KeyString);
 #endif
    Status = RfSecureBootPemToDer (KeyString, &KeySize, &KeyData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

 #if REDFISH_SECURE_BOOT_KEY_DUMP
    RfDebugDumpBlob (DEBUG_ERROR, "Certificate blob:", KeyData, KeySize);
 #endif
  } else {
    //
    // Convert ASCII hash to binary digit
    //
 #if REDFISH_SECURE_BOOT_KEY_DUMP
    RfDebugDumpString (DEBUG_ERROR, "Signature string:", KeyString);
 #endif
    Status = RfHexStringToBlob (KeyString, &KeySize, &KeyData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

 #if REDFISH_SECURE_BOOT_KEY_DUMP
    RfDebugDumpBlob (DEBUG_ERROR, "Signature blob:", KeyData, KeySize);
 #endif
  }

  //
  // TODO: check to see if key is in secure boot variable or not.
  // This takes time to parse secure boot variable.
  //

  //
  // Enroll single key to secure boot variable
  //
  Status = RfSecureBootEnrollSingleKey (VarInfo, &KeyTypeGuid, KeyData, KeySize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to enroll key to secure boot variable: %r\n", __func__, Status));
  }

  if (KeyData != NULL) {
    FreePool (KeyData);
  }

  return Status;
}

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
  )
{
  EFI_STATUS                     Status;
  REDFISH_SECURE_BOOT_KEY_INFO   *SecureBootKeyInfo;
  SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo;
  REDFISH_SECURE_BOOT_KEY_DATA   *KeyData;
  BOOLEAN                        IsCertificate;

  if (IS_EMPTY_STRING (KeyName) || IS_EMPTY_STRING (KeyUri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: delete key: %s at: %a\n", __func__, KeyName, KeyUri));

  VarInfo = RfGetSecureBootKeyInfo (KeyName);
  if (VarInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // We can not modify default keys
  //
  if (VarInfo->IsDefaultKey) {
    return EFI_ACCESS_DENIED;
  }

  if (AsciiStrStr (KeyUri, REDFISH_SECURE_BOOT_JSON_CERT_NAME) != NULL) {
    IsCertificate = TRUE;
  } else {
    IsCertificate = FALSE;
  }

  //
  // Load history table to see if this key exists or not.
  //
  Status = RfOpenHistoryTable (VarInfo, &SecureBootKeyInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: load history table %s failed: %r\n", __func__, KeyName, Status));
    return Status;
  }

  //
  // Check to see if we have this key in database or not
  //
  KeyData = RfSecureBootKeyFindUri (SecureBootKeyInfo, KeyUri, IsCertificate);
  if (KeyData == NULL) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: key found in history table and its hash is: %a\n", __func__, KeyData->Hash));
  //
  // There is only one key in PK. Remove PK variable
  //
  if (VarInfo->Type == RedfishSecureBootKeyPk) {
    Status = DisablePKProtection ();
    if (!EFI_ERROR (Status)) {
      Status = DeletePlatformKey ();
    }

    goto ON_RELEASE;
  }

  Status = RfSecureBootDeleteSingleKey (VarInfo, KeyData->Hash);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to delete key in secure boot variable: %r\n", __func__, Status));
  }

ON_RELEASE:

  //
  // Close history table without touching it.
  //
  RfCloseHistoryTable (SecureBootKeyInfo, FALSE);

  return Status;
}

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
  )
{
  SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo;
  EFI_STATUS                     Status;
  UINTN                          VarSize;

  if (IS_EMPTY_STRING (KeyName)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: delete secure boot variable: %s\n", __func__, KeyName));

  VarInfo = RfGetSecureBootKeyInfo (KeyName);
  if (VarInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // We can not modify default keys
  //
  if (VarInfo->IsDefaultKey) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Check to see if variable exists or not.
  //
  VarSize = 0;
  Status  = gRT->GetVariable (
                   VarInfo->VariableName,
                   VarInfo->VariableGuid,
                   NULL,
                   &VarSize,
                   NULL
                   );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    if (Status == EFI_NOT_FOUND) {
      //
      // secure boot variable does not exist.
      //
      return EFI_SUCCESS;
    }

    DEBUG ((DEBUG_ERROR, "%a: check variable: %s failed: %r\n", __func__, VarInfo->VariableName, Status));
    return Status;
  }

  //
  // Delete secure boot variable
  //
  Status = RfSecureBootDeleteVariable (VarInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: delete %s failed: %r\n", __func__, KeyName, Status));
  }

  return Status;
}

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
  )
{
  SECURE_BOOT_KEY_VARIABLE_INFO  *VarInfo;
  SECURE_BOOT_KEY_VARIABLE_INFO  *DefaultVarInfo;
  EFI_STATUS                     Status;
  VOID                           *Data;
  UINTN                          DataSize;

  if (IS_EMPTY_STRING (KeyName)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_SECURE_BOOT_KEY_DEBUG, "%a: reset secure boot variable: %s\n", __func__, KeyName));

  VarInfo = RfGetSecureBootKeyInfo (KeyName);
  if (VarInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // We can not modify default keys
  //
  if (VarInfo->IsDefaultKey) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Delete key first
  //
  Status = RedfishSecureBootDeleteAllKeys (This, KeyName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot delete key: %s:%r\n", __func__, KeyName, Status));
    return Status;
  }

  //
  // Find default key
  //
  DefaultVarInfo = RfGetSecureBootDefaultKeyInfo (VarInfo);
  if (VarInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Read key from default variable.
  //
  DataSize = 0;
  Data     = NULL;
  Status   = GetVariable2 (DefaultVarInfo->VariableName, DefaultVarInfo->VariableGuid, &Data, &DataSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot read default key: %s: %r\n", __func__, DefaultVarInfo->VariableName, Status));
    return Status;
  }

  //
  // Enroll key from data
  //
  if (Data != NULL) {
    Status = RfSecureBootEnrollKeyFromData (VarInfo, DataSize, Data);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: cannot enroll key: %s: %r\n", __func__, DefaultVarInfo->VariableName, Status));
    }

    FreePool (Data);
  }

  return Status;
}
