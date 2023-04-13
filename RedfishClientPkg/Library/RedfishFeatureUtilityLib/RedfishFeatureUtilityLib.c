/** @file
  Redfish feature utility library implementation

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishFeatureUtilityInternal.h"

EDKII_REDFISH_ETAG_PROTOCOL             *mEtagProtocol          = NULL;
EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *mConfigLangMapProtocol = NULL;

/**

  This function locates protocol with given protocol GUID and return
  protocol instance back to caller.

  @param[in,out]  ProtocolInstance  Protocol instances to keep located protocol.
  @param[in]      ProtocolGuid      Protocol GUID to locate protocol.

  @retval     EFI_SUCCESS         Protocol is located and returned.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RedfishLocateProtocol (
  IN OUT  VOID      **ProtocolInstance,
  IN      EFI_GUID  *ProtocolGuid
  )
{
  EFI_STATUS  Status;

  if ((ProtocolInstance == NULL) || (ProtocolGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*ProtocolInstance != NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  ProtocolGuid,
                  NULL,
                  ProtocolInstance
                  );
  return Status;
}

/**

  Get array key by parsing the URI.

  @param[in]  Uri     URI with array key.
  @param[out] ArrayKey  Array key in given URI string.

  @retval     EFI_SUCCESS         Array key is found.
  @retval     Others              Errors occur.

**/
EFI_STATUS
GetArraykeyFromUri (
  IN  CHAR8  *Uri,
  OUT CHAR8  **ArrayKey
  )
{
  CHAR8  *LeftBracket;
  UINTN  Index;

  if (IS_EMPTY_STRING (Uri) || (ArrayKey == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *ArrayKey = NULL;

  //
  // Loop through Uri and find last '['
  //
  LeftBracket = NULL;
  for (Index = 0; Uri[Index] != '\0'; Index++) {
    if (Uri[Index] == '[') {
      LeftBracket = &Uri[Index];
    }
  }

  if (LeftBracket == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // skip '/'
  //
  ++LeftBracket;

  *ArrayKey = AllocateCopyPool (AsciiStrSize (LeftBracket), LeftBracket);
  if (*ArrayKey == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // remove ']'
  //
  *(*ArrayKey + AsciiStrLen (*ArrayKey) - 1) = '\0';

  return EFI_SUCCESS;
}

/**

  This function query ETag from given URI string and keep it in database.

  @param[in]  Service               Redfish service instance to make query.
  @param[in]  Uri                   URI to query ETag.
  @param[in]  CheckPendingSettings  Set this to true and @Redfish.Settings will
                                    be handled together. FALSE otherwise.

  @retval     EFI_SUCCESS     ETAG and URI are applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
SetEtagFromUri (
  IN  REDFISH_SERVICE  *RedfishService,
  IN  EFI_STRING       Uri,
  IN  BOOLEAN          CheckPendingSettings
  )
{
  EFI_STATUS        Status;
  CHAR8             *Etag;
  CHAR8             *AsciiUri;
  REDFISH_RESPONSE  Response;
  EFI_STRING        PendingSettingUri;

  if ((RedfishService == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  AsciiUri          = NULL;
  Etag              = NULL;
  PendingSettingUri = NULL;

  Status = RedfishLocateProtocol ((VOID **)&mEtagProtocol, &gEdkIIRedfishETagProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
    return Status;
  }

  Status = GetResourceByUri (RedfishService, Uri, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: get resource from: %s failed\n", __FUNCTION__, Uri));
    return Status;
  }

  //
  // Find etag in HTTP response header
  //
  Status = GetEtagAndLocation (&Response, &Etag, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get ETag from HTTP header\n", __FUNCTION__));
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  AsciiUri = StrUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  //
  // Save ETag to variable.
  //
  Status = mEtagProtocol->Set (mEtagProtocol, AsciiUri, Etag);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to set ETag %a -> %a\n", __FUNCTION__, Etag, AsciiUri));
  }

  if (CheckPendingSettings && (Response.Payload != NULL)) {
    //
    // Check to see if there is @Redfish.Settings
    //
    Status = GetPendingSettings (
               RedfishService,
               Response.Payload,
               NULL,
               &PendingSettingUri
               );
    if (!EFI_ERROR (Status)) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: @Redfish.Settings found: %s\n", __FUNCTION__, PendingSettingUri));

      Status = SetEtagFromUri (RedfishService, PendingSettingUri, FALSE);
    }
  }

ON_RELEASE:

  if (AsciiUri != NULL) {
    FreePool (AsciiUri);
  }

  if (Etag != NULL) {
    FreePool (Etag);
  }

  if (PendingSettingUri != NULL) {
    FreePool (PendingSettingUri);
  }

  if (Response.Payload != NULL) {
    RedfishFreeResponse (
      Response.StatusCode,
      Response.HeaderCount,
      Response.Headers,
      Response.Payload
      );
  }

  return Status;
}

/**

  Keep ETAG string and URI string in database.

  @param[in]  EtagStr   ETAG string.
  @param[in]  Uri       URI string.

  @retval     EFI_SUCCESS     ETAG and URI are applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
SetEtagWithUri (
  IN  CHAR8       *EtagStr,
  IN  EFI_STRING  Uri
  )
{
  EFI_STATUS  Status;
  CHAR8       *AsciiUri;

  if (IS_EMPTY_STRING (EtagStr) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishLocateProtocol ((VOID **)&mEtagProtocol, &gEdkIIRedfishETagProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
    return Status;
  }

  AsciiUri = StrUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mEtagProtocol->Set (mEtagProtocol, AsciiUri, EtagStr);

  FreePool (AsciiUri);

  return EFI_SUCCESS;
}

/**

  Find ETAG string that refers to given URI.

  @param[in]  Uri       Target URI string.

  @retval     CHAR8 *   ETAG string
  @retval     NULL      No ETAG is found.

**/
CHAR8 *
GetEtagWithUri (
  IN  EFI_STRING  Uri
  )
{
  EFI_STATUS  Status;
  CHAR8       *AsciiUri;
  CHAR8       *EtagStr;

  if (IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  Status = RedfishLocateProtocol ((VOID **)&mEtagProtocol, &gEdkIIRedfishETagProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
    return NULL;
  }

  AsciiUri = StrUnicodeToAscii (Uri);
  if (AsciiUri == NULL) {
    return NULL;
  }

  Status = mEtagProtocol->Get (mEtagProtocol, AsciiUri, &EtagStr);

  FreePool (AsciiUri);

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  return EtagStr;
}

/**

  Save ETag list to variable immediately.

**/
VOID
SaveETagList (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = RedfishLocateProtocol ((VOID **)&mEtagProtocol, &gEdkIIRedfishETagProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishETagProtocolGuid: %r\n", __FUNCTION__, Status));
    return;
  }

  mEtagProtocol->Flush (mEtagProtocol);
}

/**

  Convert Unicode string to ASCII string. It's call responsibility to release returned buffer.

  @param[in]  UnicodeStr      Unicode string to convert.

  @retval     CHAR8 *         ASCII string returned.
  @retval     NULL            Errors occur.

**/
CHAR8 *
StrUnicodeToAscii (
  IN EFI_STRING  UnicodeStr
  )
{
  CHAR8       *AsciiStr;
  UINTN       AsciiStrSize;
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (UnicodeStr)) {
    return NULL;
  }

  AsciiStrSize = StrLen (UnicodeStr) + 1;
  AsciiStr     = AllocatePool (AsciiStrSize);
  if (AsciiStr == NULL) {
    return NULL;
  }

  Status = UnicodeStrToAsciiStrS (UnicodeStr, AsciiStr, AsciiStrSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UnicodeStrToAsciiStrS failed: %r\n", Status));
    FreePool (AsciiStr);
    return NULL;
  }

  return AsciiStr;
}

/**

  Convert ASCII string to Unicode string. It's call responsibility to release returned buffer.

  @param[in]  AsciiStr        ASCII string to convert.

  @retval     EFI_STRING      Unicode string returned.
  @retval     NULL            Errors occur.

**/
EFI_STRING
StrAsciiToUnicode (
  IN CHAR8  *AsciiStr
  )
{
  EFI_STRING  UnicodeStr;
  UINTN       UnicodeStrSize;
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (AsciiStr)) {
    return NULL;
  }

  UnicodeStrSize = (AsciiStrLen (AsciiStr) + 1) * sizeof (CHAR16);
  UnicodeStr     = AllocatePool (UnicodeStrSize);
  if (UnicodeStr == NULL) {
    return NULL;
  }

  Status = AsciiStrToUnicodeStrS (AsciiStr, UnicodeStr, UnicodeStrSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "t failed: %r\n", Status));
    FreePool (UnicodeStr);
    return NULL;
  }

  return UnicodeStr;
}

/**

  Apply property value to UEFI HII database in string type.

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  FeatureValue  New value to set.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsStringType (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  ConfigureLang,
  IN  CHAR8       *FeatureValue
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || (FeatureValue == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_STRING) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    if (AsciiStrCmp (FeatureValue, RedfishValue.Value.Buffer) != 0) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s from %a to %a\n", __FUNCTION__, Schema, Version, ConfigureLang, RedfishValue.Value.Buffer, FeatureValue));

      FreePool (RedfishValue.Value.Buffer);
      RedfishValue.Value.Buffer = FeatureValue;

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s to %s failed: %r\n", __FUNCTION__, ConfigureLang, FeatureValue, Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is: %s\n", __FUNCTION__, Schema, Version, ConfigureLang, RedfishValue.Value.Buffer, Status));
    }
  }

  return Status;
}

/**

  Apply property value to UEFI HII database in numeric type.

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  FeatureValue  New value to set.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsNumericType (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  ConfigureLang,
  IN  INTN        FeatureValue
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_INTEGER) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not numeric type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    if (RedfishValue.Value.Integer != FeatureValue) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s from 0x%x to 0x%x\n", __FUNCTION__, Schema, Version, ConfigureLang, RedfishValue.Value.Integer, FeatureValue));

      RedfishValue.Value.Integer = (INT64)FeatureValue;

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s to 0x%x failed: %r\n", __FUNCTION__, ConfigureLang, FeatureValue, Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is: 0x%x\n", __FUNCTION__, Schema, Version, ConfigureLang, RedfishValue.Value.Integer, Status));
    }
  }

  return Status;
}

/**

  Apply property value to UEFI HII database in boolean type.

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  FeatureValue  New value to set.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsBooleanType (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  ConfigureLang,
  IN  BOOLEAN     FeatureValue
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_BOOLEAN) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not boolean type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    if (RedfishValue.Value.Boolean != FeatureValue) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s from %a to %a\n", __FUNCTION__, Schema, Version, ConfigureLang, (RedfishValue.Value.Boolean ? "True" : "False"), (FeatureValue ? "True" : "False")));

      RedfishValue.Value.Boolean = FeatureValue;

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s to %a failed: %r\n", __FUNCTION__, ConfigureLang, (FeatureValue ? "True" : "False"), Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is: %a\n", __FUNCTION__, Schema, Version, ConfigureLang, (RedfishValue.Value.Boolean ? "True" : "False"), Status));
    }
  }

  return Status;
}

/**

  Apply property value to UEFI HII database in vague type.

  @param[in]  Schema          Property schema.
  @param[in]  Version         Property schema version.
  @param[in]  ConfigureLang   Configure language refers to this property.
  @param[in]  VagueValuePtr   Pointer of vague values to to set.
  @param[in]  NumVagueValues  Number of vague values.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsVagueType (
  IN  CHAR8                         *Schema,
  IN  CHAR8                         *Version,
  IN  EFI_STRING                    ConfigureLang,
  IN  RedfishCS_EmptyProp_KeyValue  *VagueValuePtr,
  IN  UINT32                        NumberOfVagueValues
  )
{
  EFI_STATUS                    Status;
  UINTN                         StrSize;
  CHAR8                         *ConfigureLangAscii;
  CHAR8                         *ConfigureLangKeyAscii;
  EFI_STRING                    ConfigureKeyLang;
  EDKII_REDFISH_VALUE           RedfishValue;
  EDKII_REDFISH_VALUE_TYPES     PropertyDatatype;
  RedfishCS_EmptyProp_KeyValue  *CurrentVagueValuePtr;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || (VagueValuePtr == NULL) || (NumberOfVagueValues == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: schema: %a %a config lang: %s NumberOfVagueValues: %d\n", __FUNCTION__, Schema, Version, ConfigureLang, NumberOfVagueValues));

  ConfigureLangAscii = AllocatePool (StrLen (ConfigureLang) + 1);
  if (ConfigureLangAscii == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG ((DEBUG_ERROR, "%a: Allocate memory for generate ConfigureLang of vague key of %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
    return Status;
  }

  Status = UnicodeStrToAsciiStrS (ConfigureLang, ConfigureLangAscii, StrLen (ConfigureLang) + 1);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Convert the configureLang of vague key of %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
    return Status;
  }

  CurrentVagueValuePtr = VagueValuePtr;
  while (CurrentVagueValuePtr != NULL) {
    //
    // Generate ConfigureLang with the key name
    //
    // ConfigureKeyLang = GetConfigureLang (ConfigureLangAscii, CurrentVagueValuePtr->KeyNamePtr);
    StrSize               = AsciiStrLen (ConfigureLangAscii) + AsciiStrLen (CurrentVagueValuePtr->KeyNamePtr) + 2;
    ConfigureLangKeyAscii = AllocateZeroPool (StrSize);
    ConfigureKeyLang      = AllocateZeroPool (StrSize * sizeof (CHAR16));
    if ((ConfigureLangKeyAscii == NULL) || (ConfigureKeyLang == NULL)) {
      DEBUG ((DEBUG_ERROR, "%a: Generate ConfigureLang of vague key of %a.%a %s %a failed!\n", __FUNCTION__, Schema, Version, ConfigureLang, CurrentVagueValuePtr->KeyNamePtr));
      goto ErrorContinue;
    }

    AsciiStrCatS (ConfigureLangKeyAscii, StrSize, ConfigureLangAscii);
    AsciiStrCatS (ConfigureLangKeyAscii, StrSize, "/");
    AsciiStrCatS (ConfigureLangKeyAscii, StrSize, CurrentVagueValuePtr->KeyNamePtr);
    AsciiStrToUnicodeStrS (ConfigureLangKeyAscii, ConfigureKeyLang, StrSize);
    FreePool (ConfigureLangKeyAscii);
    ConfigureLangKeyAscii = NULL;
    //
    // Initial property data type and value.
    //
    if (CurrentVagueValuePtr->Value->DataType == RedfishCS_Vague_DataType_String) {
      PropertyDatatype = REDFISH_VALUE_TYPE_STRING;
    } else if (CurrentVagueValuePtr->Value->DataType == RedfishCS_Vague_DataType_Bool) {
      PropertyDatatype = REDFISH_VALUE_TYPE_BOOLEAN;
    } else if (CurrentVagueValuePtr->Value->DataType == RedfishCS_Vague_DataType_Int64) {
      PropertyDatatype = REDFISH_VALUE_TYPE_INTEGER;
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s Unsupported Redfish property data type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      goto ErrorContinue;
    }

    //
    // Get the current value from HII
    //
    Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureKeyLang, &RedfishValue);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, Status));
    } else {
      if (RedfishValue.Type != PropertyDatatype) {
        DEBUG ((DEBUG_ERROR, "%a: %a.%a %s mismatched data type\n", __FUNCTION__, Schema, Version, ConfigureKeyLang));
        goto ErrorContinue;
      }

      if (PropertyDatatype == REDFISH_VALUE_TYPE_STRING) {
        //
        // This is a string property.
        //
        if (AsciiStrCmp (CurrentVagueValuePtr->Value->DataValue.CharPtr, RedfishValue.Value.Buffer) != 0) {
          //
          // Apply settings from redfish
          //
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a.%a apply %s from %a to %a\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, RedfishValue.Value.Buffer, CurrentVagueValuePtr->Value->DataValue.CharPtr));
          FreePool (RedfishValue.Value.Buffer);
          RedfishValue.Value.Buffer = CurrentVagueValuePtr->Value->DataValue.CharPtr;
          Status                    = RedfishPlatformConfigSetValue (Schema, Version, ConfigureKeyLang, RedfishValue);
          if (!EFI_ERROR (Status)) {
            //
            // Configuration changed. Enable system reboot flag.
            //
            REDFISH_ENABLE_SYSTEM_REBOOT ();
          } else {
            DEBUG ((DEBUG_ERROR, "%a: apply %a to %a failed: %r\n", __FUNCTION__, ConfigureKeyLang, CurrentVagueValuePtr->Value->DataValue.CharPtr, Status));
          }
        } else {
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a.%a %s value is: %a\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, RedfishValue.Value.Buffer, Status));
        }
      } else if (PropertyDatatype == REDFISH_VALUE_TYPE_BOOLEAN) {
        //
        // This is a boolean property.
        //
        if (RedfishValue.Value.Boolean != *CurrentVagueValuePtr->Value->DataValue.BoolPtr) {
          //
          // Apply settings from redfish
          //
          DEBUG ((
            REDFISH_DEBUG_TRACE,
            "%a: %a.%a apply %s from %a to %a\n",
            __FUNCTION__,
            Schema,
            Version,
            ConfigureKeyLang,
            (RedfishValue.Value.Boolean ? "True" : "False"),
            (*CurrentVagueValuePtr->Value->DataValue.BoolPtr ? "True" : "False")
            ));

          RedfishValue.Value.Boolean = (BOOLEAN)*CurrentVagueValuePtr->Value->DataValue.BoolPtr;
          Status                     = RedfishPlatformConfigSetValue (Schema, Version, ConfigureKeyLang, RedfishValue);
          if (!EFI_ERROR (Status)) {
            //
            // Configuration changed. Enable system reboot flag.
            //
            REDFISH_ENABLE_SYSTEM_REBOOT ();
          } else {
            DEBUG ((DEBUG_ERROR, "%a: apply %s to %a failed: %r\n", __FUNCTION__, ConfigureKeyLang, (*CurrentVagueValuePtr->Value->DataValue.BoolPtr ? "True" : "False"), Status));
          }
        } else {
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a.%a %s value is: %a\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, (RedfishValue.Value.Boolean ? "True" : "False"), Status));
        }
      } else if (PropertyDatatype == REDFISH_VALUE_TYPE_INTEGER) {
        //
        // This is a integer property.
        //
        if (RedfishValue.Value.Integer != *CurrentVagueValuePtr->Value->DataValue.Int64Ptr) {
          //
          // Apply settings from redfish
          //
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a.%a apply %s from 0x%x to 0x%x\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, RedfishValue.Value.Integer, *CurrentVagueValuePtr->Value->DataValue.Int64Ptr));

          RedfishValue.Value.Integer = (INT64)*CurrentVagueValuePtr->Value->DataValue.Int64Ptr;
          Status                     = RedfishPlatformConfigSetValue (Schema, Version, ConfigureKeyLang, RedfishValue);
          if (!EFI_ERROR (Status)) {
            //
            // Configuration changed. Enable system reboot flag.
            //
            REDFISH_ENABLE_SYSTEM_REBOOT ();
          } else {
            DEBUG ((DEBUG_ERROR, "%a: apply %s to 0x%x failed: %r\n", __FUNCTION__, ConfigureKeyLang, *CurrentVagueValuePtr->Value->DataValue.Int64Ptr, Status));
          }
        } else {
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a.%a %s value is: 0x%x\n", __FUNCTION__, Schema, Version, ConfigureKeyLang, RedfishValue.Value.Integer, Status));
        }
      } else {
        DEBUG ((DEBUG_ERROR, "%a: %a.%a %s Unsupported Redfish property data type\n", __FUNCTION__, Schema, Version, ConfigureLang));
        goto ErrorContinue;
      }
    }

ErrorContinue:;
    if (ConfigureLangKeyAscii != NULL) {
      FreePool (ConfigureLangKeyAscii);
      ConfigureLangKeyAscii = NULL;
    }

    if (ConfigureKeyLang != NULL) {
      FreePool (ConfigureKeyLang);
      ConfigureKeyLang = NULL;
    }

    CurrentVagueValuePtr = CurrentVagueValuePtr->NextKeyValuePtr;
  }

  if (ConfigureLangAscii != NULL) {
    FreePool (ConfigureLangAscii);
  }

  if (ConfigureLangKeyAscii != NULL) {
    FreePool (ConfigureLangKeyAscii);
  }

  if (ConfigureKeyLang != NULL) {
    FreePool (ConfigureKeyLang);
  }

  return EFI_SUCCESS;
}

/**

  Release the memory in RedfishValue while value type is array.

  @param[in]  RedfishValue   Pointer to Redfish value

**/
VOID
FreeArrayTypeRedfishValue (
  EDKII_REDFISH_VALUE  *RedfishValue
  )
{
  UINTN  Index;

  if (RedfishValue == NULL) {
    return;
  }

  if ((RedfishValue->Type != REDFISH_VALUE_TYPE_INTEGER_ARRAY) && (RedfishValue->Type != REDFISH_VALUE_TYPE_STRING_ARRAY)) {
    return;
  }

  switch (RedfishValue->Type) {
    case REDFISH_VALUE_TYPE_STRING_ARRAY:
      for (Index = 0; Index < RedfishValue->ArrayCount; Index++) {
        FreePool (RedfishValue->Value.StringArray[Index]);
      }

      FreePool (RedfishValue->Value.StringArray);
      RedfishValue->Value.StringArray = NULL;
      break;

    case REDFISH_VALUE_TYPE_INTEGER_ARRAY:
      FreePool (RedfishValue->Value.IntegerArray);
      RedfishValue->Value.IntegerArray = NULL;
      break;

    case REDFISH_VALUE_TYPE_BOOLEAN_ARRAY:
      FreePool (RedfishValue->Value.BooleanArray);
      RedfishValue->Value.BooleanArray = NULL;
      break;

    default:
      return;
  }

  RedfishValue->ArrayCount = 0;
}

/**

  Apply property value to UEFI HII database in string array type.

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  ArrayHead     Head of array value.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsStringArrayType (
  IN  CHAR8                 *Schema,
  IN  CHAR8                 *Version,
  IN  EFI_STRING            ConfigureLang,
  IN  RedfishCS_char_Array  *ArrayHead
  )
{
  EFI_STATUS            Status;
  EDKII_REDFISH_VALUE   RedfishValue;
  UINTN                 Index;
  RedfishCS_char_Array  *Buffer;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || (ArrayHead == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_STRING_ARRAY) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    //
    // If there is no change in array, do nothing
    //
    if (!CompareRedfishStringArrayValues (ArrayHead, RedfishValue.Value.StringArray, RedfishValue.ArrayCount)) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s for array\n", __FUNCTION__, Schema, Version, ConfigureLang));
      FreeArrayTypeRedfishValue (&RedfishValue);

      //
      // Convert array from RedfishCS_char_Array to EDKII_REDFISH_VALUE
      //
      RedfishValue.ArrayCount = 0;
      Buffer                  = ArrayHead;
      while (Buffer != NULL) {
        RedfishValue.ArrayCount += 1;
        Buffer                   = Buffer->Next;
      }

      //
      // Allocate pool for new values
      //
      RedfishValue.Value.StringArray = AllocatePool (RedfishValue.ArrayCount *sizeof (CHAR8 *));
      if (RedfishValue.Value.StringArray == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }

      Buffer = ArrayHead;
      Index  = 0;
      while (Buffer != NULL) {
        RedfishValue.Value.StringArray[Index] = AllocateCopyPool (AsciiStrSize (Buffer->ArrayValue), Buffer->ArrayValue);
        if (RedfishValue.Value.StringArray[Index] == NULL) {
          ASSERT (FALSE);
          return EFI_OUT_OF_RESOURCES;
        }

        Buffer = Buffer->Next;
        Index++;
      }

      ASSERT (Index <= RedfishValue.ArrayCount);

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s array failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s array value has no change\n", __FUNCTION__, Schema, Version, ConfigureLang));
    }
  }

  return Status;
}

/**

  Apply property value to UEFI HII database in numeric array type (INT64).

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  ArrayHead     Head of array value.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsNumericArrayType (
  IN  CHAR8                  *Schema,
  IN  CHAR8                  *Version,
  IN  EFI_STRING             ConfigureLang,
  IN  RedfishCS_int64_Array  *ArrayHead
  )
{
  EFI_STATUS             Status;
  EDKII_REDFISH_VALUE    RedfishValue;
  UINTN                  Index;
  RedfishCS_int64_Array  *Buffer;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || (ArrayHead == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_INTEGER_ARRAY) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    //
    // If there is no change in array, do nothing
    //
    if (!CompareRedfishNumericArrayValues (ArrayHead, RedfishValue.Value.IntegerArray, RedfishValue.ArrayCount)) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s for array\n", __FUNCTION__, Schema, Version, ConfigureLang));
      FreeArrayTypeRedfishValue (&RedfishValue);

      //
      // Convert array from RedfishCS_int64_Array to EDKII_REDFISH_VALUE
      //
      RedfishValue.ArrayCount = 0;
      Buffer                  = ArrayHead;
      while (Buffer != NULL) {
        RedfishValue.ArrayCount += 1;
        Buffer                   = Buffer->Next;
      }

      //
      // Allocate pool for new values
      //
      RedfishValue.Value.IntegerArray = AllocatePool (RedfishValue.ArrayCount * sizeof (INT64));
      if (RedfishValue.Value.IntegerArray == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }

      Buffer = ArrayHead;
      Index  = 0;
      while (Buffer != NULL) {
        RedfishValue.Value.IntegerArray[Index] = (INT64)*Buffer->ArrayValue;
        Buffer                                 = Buffer->Next;
        Index++;
      }

      ASSERT (Index <= RedfishValue.ArrayCount);

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s array failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s array value has no change\n", __FUNCTION__, Schema, Version, ConfigureLang));
    }
  }

  return Status;
}

/**

  Apply property value to UEFI HII database in boolean array type (INT64).

  @param[in]  Schema        Property schema.
  @param[in]  Version       Property schema version.
  @param[in]  ConfigureLang Configure language refers to this property.
  @param[in]  ArrayHead     Head of Redfish CS boolean array value.

  @retval     EFI_SUCCESS     New value is applied successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
ApplyFeatureSettingsBooleanArrayType (
  IN  CHAR8                 *Schema,
  IN  CHAR8                 *Version,
  IN  EFI_STRING            ConfigureLang,
  IN  RedfishCS_bool_Array  *ArrayHead
  )
{
  EFI_STATUS            Status;
  EDKII_REDFISH_VALUE   RedfishValue;
  UINTN                 Index;
  RedfishCS_bool_Array  *Buffer;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || (ArrayHead == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the current value from HII
  //
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLang, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLang, Status));
  } else {
    if (RedfishValue.Type != REDFISH_VALUE_TYPE_BOOLEAN_ARRAY) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
      return EFI_DEVICE_ERROR;
    }

    //
    // If there is no change in array, do nothing
    //
    if (!CompareRedfishBooleanArrayValues (ArrayHead, RedfishValue.Value.BooleanArray, RedfishValue.ArrayCount)) {
      //
      // Apply settings from redfish
      //
      DEBUG ((DEBUG_INFO, "%a: %a.%a apply %s for array\n", __FUNCTION__, Schema, Version, ConfigureLang));
      FreeArrayTypeRedfishValue (&RedfishValue);

      //
      // Convert array from RedfishCS_int64_Array to EDKII_REDFISH_VALUE
      //
      RedfishValue.ArrayCount = 0;
      Buffer                  = ArrayHead;
      while (Buffer != NULL) {
        RedfishValue.ArrayCount += 1;
        Buffer                   = Buffer->Next;
      }

      //
      // Allocate pool for new values
      //
      RedfishValue.Value.BooleanArray = AllocatePool (RedfishValue.ArrayCount * sizeof (BOOLEAN));
      if (RedfishValue.Value.BooleanArray == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }

      Buffer = ArrayHead;
      Index  = 0;
      while (Buffer != NULL) {
        RedfishValue.Value.BooleanArray[Index] = (BOOLEAN)*Buffer->ArrayValue;
        Buffer                                 = Buffer->Next;
        Index++;
      }

      ASSERT (Index <= RedfishValue.ArrayCount);

      Status = RedfishPlatformConfigSetValue (Schema, Version, ConfigureLang, RedfishValue);
      if (!EFI_ERROR (Status)) {
        //
        // Configuration changed. Enable system reboot flag.
        //
        REDFISH_ENABLE_SYSTEM_REBOOT ();
      } else {
        DEBUG ((DEBUG_ERROR, "%a: apply %s array failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }
    } else {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a %s array value has no change\n", __FUNCTION__, Schema, Version, ConfigureLang));
    }
  }

  return Status;
}

/**

  Read redfish resource by given resource URI.

  @param[in]  Service       Redfish service instance to make query.
  @param[in]  ResourceUri   Target resource URI.
  @param[out] Response      HTTP response from redfish service.

  @retval     EFI_SUCCESS     Resrouce is returned successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
GetResourceByUri (
  IN  REDFISH_SERVICE   *Service,
  IN  EFI_STRING        ResourceUri,
  OUT REDFISH_RESPONSE  *Response
  )
{
  EFI_STATUS  Status;
  CHAR8       *AsciiResourceUri;

  if ((Service == NULL) || (Response == NULL) || IS_EMPTY_STRING (ResourceUri)) {
    return EFI_INVALID_PARAMETER;
  }

  AsciiResourceUri = StrUnicodeToAscii (ResourceUri);
  if (AsciiResourceUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get resource from redfish service.
  //
  Status = RedfishGetByUri (
             Service,
             AsciiResourceUri,
             Response
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: RedfishGetByUri to %a failed: %r\n", __FUNCTION__, AsciiResourceUri, Status));
    if (Response->Payload != NULL) {
      RedfishDumpPayload (Response->Payload);
      RedfishFreeResponse (
        NULL,
        0,
        NULL,
        Response->Payload
        );
      Response->Payload = NULL;
    }
  }

  if (AsciiResourceUri != NULL) {
    FreePool (AsciiResourceUri);
  }

  return Status;
}

/**

  Check if this is the Redpath array. Usually the Redpath array represents
  the collection member. Return

  @param[in]  ConfigureLang             The Redpath to check
  @param[out] ArraySignatureOpen        String to the open of array signature.
  @param[out] ArraySignatureClose       String to the close of array signature.

  @retval     EFI_SUCCESS            Index is found.
  @retval     EFI_NOT_FOUND          The non-array configure language string is returned.
  @retval     EFI_INVALID_PARAMETER  The format of input ConfigureLang is wrong.
  @retval     Others                 Errors occur.

**/
EFI_STATUS
IsRedpathArray (
  IN EFI_STRING   ConfigureLang,
  OUT EFI_STRING  *ArraySignatureOpen OPTIONAL,
  OUT EFI_STRING  *ArraySignatureClose OPTIONAL
  )
{
  CHAR16  *IndexString;

  if (ConfigureLang == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ArraySignatureOpen != NULL) {
    *ArraySignatureOpen = NULL;
  }

  if (ArraySignatureClose != NULL) {
    *ArraySignatureClose = NULL;
  }

  //
  // looking for index signature "{""
  //
  IndexString = StrStr (ConfigureLang, BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_START_SIGNATURE);
  if (IndexString != NULL) {
    if (ArraySignatureOpen != NULL) {
      *ArraySignatureOpen = IndexString;
    }

    //
    // Skip "{"
    //
    IndexString = IndexString + StrLen (BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_START_SIGNATURE);
    //
    // Looking for "}"
    //
    IndexString = StrStr (IndexString, BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_END_SIGNATURE);
    if (IndexString == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    if (ArraySignatureClose != NULL) {
      *ArraySignatureClose = IndexString;
    }

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**

  Get number of node from the string. Node is separated by '/'.

  @param[in]  NodeString             The node string to parse.

  @retval     UINTN                  Number of nodes in the string.

**/
UINTN
GetNumberOfRedpathNodes (
  IN EFI_STRING  NodeString
  )
{
  UINTN  Index;
  UINTN  NumberNodes;
  UINTN  StringLen;

  NumberNodes = 0;
  StringLen   = StrLen (NodeString);
  Index       = 1; // ConfigLang always starts with '/'.
  while (Index < StringLen) {
    if (*(NodeString + Index) == L'/') {
      NumberNodes++;
    }

    Index++;
  }

  NumberNodes++;

  return (NumberNodes);
}

/**

  Get the node string by index

  @param[in]  NodeString             The node string to parse.
  @param[in]  Index                  Zero-based index of the node.
  @param[out] EndOfNodePtr           Pointer to receive the pointer to
                                     the last character of node string.

  @retval     EFI_STRING             the beginning of the node string.

**/
EFI_STRING
GetRedpathNodeByIndex (
  IN  EFI_STRING  NodeString,
  IN  UINTN       Index,
  OUT EFI_STRING  *EndOfNodePtr OPTIONAL
  )
{
  UINTN       NumberNodes;
  UINTN       StringLen;
  UINTN       StringIndex;
  EFI_STRING  NodeStart;
  EFI_STRING  NodeEnd;

  NumberNodes = 0;
  StringLen   = StrLen (NodeString);
  StringIndex = 1; // ConfigLang always starts with '/'.
  NodeStart   = NodeString;
  if (EndOfNodePtr != NULL) {
    *EndOfNodePtr = NULL;
  }

  while (StringIndex < StringLen) {
    if (*(NodeString + StringIndex) == L'/') {
      NodeEnd = NodeString + StringIndex - 1;
      if (NumberNodes == Index) {
        if (EndOfNodePtr != NULL) {
          *EndOfNodePtr = NodeEnd;
        }

        return NodeStart;
      } else {
        NodeStart = NodeString + StringIndex + 1;
      }
    }

    StringIndex++;
  }

  return (NULL);
}

/**

  Find array index from given configure language string.

  @param[in]  ConfigureLang         Configure language string to parse.
  @param[out] UnifiedConfigureLang  The configure language in array.
  @param[out] Index                 The array index number.

  @retval     EFI_SUCCESS            Index is found.
  @retval     EFI_NOT_FOUND          The non-array configure language string is returned.
  @retval     EFI_INVALID_PARAMETER  The format of input ConfigureLang is wrong.
  @retval     Others                 Errors occur.

**/
EFI_STATUS
GetArrayIndexFromArrayTypeConfigureLang (
  IN  CHAR16  *ConfigureLang,
  OUT CHAR16  **UnifiedConfigureLang,
  OUT UINTN   *Index
  )
{
  EFI_STATUS  Status;
  CHAR16      *TmpConfigureLang;
  CHAR16      *ArrayOpenStr;
  CHAR16      *ArrayCloseStr;
  INTN        StringIndex;

  if ((ConfigureLang == NULL) || (UnifiedConfigureLang == NULL) || (Index == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  TmpConfigureLang = AllocateCopyPool (StrSize (ConfigureLang), ConfigureLang);
  if (TmpConfigureLang == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = IsRedpathArray (TmpConfigureLang, &ArrayOpenStr, &ArrayCloseStr);
  if (!EFI_ERROR (Status)) {
    //
    // Append '\0' for converting decimal string to integer.
    //
    ArrayCloseStr[0] = '\0';

    //
    // Convert decimal string to integer
    //
    *Index = StrDecimalToUintn (ArrayOpenStr + StrLen (BIOS_CONFIG_TO_REDFISH_REDPATH_ARRAY_START_SIGNATURE));

    //
    // Resotre the '}' character and remove rest of string.
    //
    ArrayCloseStr[0]      = L'}';
    ArrayCloseStr[1]      = '\0';
    *UnifiedConfigureLang = TmpConfigureLang;
  } else {
    if (Status == EFI_NOT_FOUND) {
      //
      // This is not the redpath array. Search "/" for the parent root.
      //
      *Index      = 0;
      StringIndex = StrLen (TmpConfigureLang) - 1;
      while (StringIndex >= 0 && *(TmpConfigureLang + StringIndex) != '/') {
        StringIndex--;
      }

      if (StringIndex >= 0 ) {
        *(TmpConfigureLang + StringIndex) = '\0';
        *UnifiedConfigureLang             = TmpConfigureLang;
        Status                            = EFI_SUCCESS;
      } else {
        Status = EFI_INVALID_PARAMETER;
      }
    }
  }

  return Status;
}

/**

  Clone the configure language list.

  @param[in]  ConfigureLangList      The source REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST.
  @param[out] DestConfigureLangList  The destination REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST.

  @retval     EFI_SUCCESS     REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST is copied.
  @retval     Others          Errors occur.

**/
EFI_STATUS
CopyConfiglanguageList (
  IN   REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *SourceConfigureLangList,
  OUT  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *DestConfigureLangList
  )
{
  UINTN  Index;

  if ((SourceConfigureLangList == NULL) || (DestConfigureLangList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DestConfigureLangList->Count = SourceConfigureLangList->Count;
  DestConfigureLangList->List  =
    (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG *)AllocateZeroPool (sizeof (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG) * DestConfigureLangList->Count);
  if (DestConfigureLangList->List == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG.\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < SourceConfigureLangList->Count; Index++) {
    DestConfigureLangList->List[Index].Index         = SourceConfigureLangList->List[Index].Index;
    DestConfigureLangList->List[Index].ConfigureLang =
      (EFI_STRING)AllocateCopyPool (StrSize (SourceConfigureLangList->List[Index].ConfigureLang), (VOID *)SourceConfigureLangList->List[Index].ConfigureLang);
  }

  return EFI_SUCCESS;
}

/**

  Clone the configure language list.

  @param[in]  ConfigureLang      The pointer to configuration language.

  @retval     UINTN       The index of collection member instance.
                          Value of 0 means no instance is found.
**/
UINTN
ConfiglanguageGetInstanceIndex (
  IN EFI_STRING  ConfigureLang
  )
{
  INTN        LeftBracketIndex;
  INTN        RightBracketIndex;
  INTN        Index;
  UINT64      Instance;
  EFI_STATUS  Status;

  if (ConfigureLang == NULL) {
    return 0;
  }

  LeftBracketIndex  = 0;
  RightBracketIndex = 0;
  Index             = StrLen (ConfigureLang) - 1;
  while (Index >= 0) {
    if (*(ConfigureLang + Index) == L'{') {
      LeftBracketIndex = Index;
      break;
    }

    if (*(ConfigureLang + Index) == L'}') {
      RightBracketIndex = Index;
    }

    Index--;
  }

  if ((RightBracketIndex - LeftBracketIndex) <= 1) {
    return 0;
  }

  *(ConfigureLang + RightBracketIndex) = 0;
  Status                               = StrDecimalToUint64S (ConfigureLang + LeftBracketIndex + 1, NULL, &Instance);
  if (EFI_ERROR (Status)) {
    Instance = 0;
  }

  //
  // Restore right curly bracket.
  //
  *(ConfigureLang + RightBracketIndex) = L'}';
  return (UINTN)Instance;
}

/**

  Destroy the configure language list.

  @param[in]  ConfigureLangList      The REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST
                                     instance to destroy.

  @retval     EFI_SUCCESS     REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST is copied.
  @retval     Others          Errors occur.

**/
EFI_STATUS
DestroyConfiglanguageList (
  IN   REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *ConfigureLangList
  )
{
  UINTN  Index;

  if (ConfigureLangList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ConfigureLangList->List != NULL) {
    for (Index = 0; Index < ConfigureLangList->Count; Index++) {
      if (ConfigureLangList->List[Index].ConfigureLang != NULL) {
        FreePool (ConfigureLangList->List[Index].ConfigureLang);
      }
    }

    FreePool (ConfigureLangList->List);
    ConfigureLangList->List = NULL;
  }

  return EFI_SUCCESS;
}

/**

  Set the node instance.

  @param[in]  DestConfigLang        Pointer to the node's configure language string.
                                    The memory pointed by ConfigLang must be allocated
                                    through memory allocation interface. Becasue we will replace
                                    the pointer in this function.
  @param[in]  MaxtLengthConfigLang  The maximum length of ConfigLang.
  @param[in]  ConfigLangInstance    Pointer to Collection member instance.

  @retval     EFI_SUCCESS     The instance is inserted to the configure language.
  @retval     Others          Errors occur.

**/
EFI_STATUS
SetResourceConfigLangMemberInstance (
  IN EFI_STRING                              *DestConfigLang,
  IN UINTN                                   MaxtLengthConfigLang,
  IN REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG  *ConfigLangInstance
  )
{
  EFI_STRING  ThisConfigLang;
  EFI_STRING  NewConfigLang;
  CHAR16      InstanceStr[10];
  INTN        Index;
  UINTN       Length;
  UINTN       MaxStrLength;

  if ((DestConfigLang == NULL) || (ConfigLangInstance == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  UnicodeSPrint ((CHAR16 *)&InstanceStr, 10, L"%d", ConfigLangInstance->Index);

  ThisConfigLang = *DestConfigLang;
  if (ThisConfigLang[0] == 0) {
    //
    // Return ConfigLangInstance->ConfigureLang
    //
    if (ConfigLangInstance->ConfigureLang == NULL) {
      return EFI_INVALID_PARAMETER;
    } else {
      StrCatS (*DestConfigLang, MaxtLengthConfigLang, ConfigLangInstance->ConfigureLang);
      return EFI_SUCCESS;
    }
  }

  MaxStrLength  = StrSize (ThisConfigLang) + StrSize ((EFI_STRING)&InstanceStr);
  NewConfigLang = ThisConfigLang;
  if (MaxtLengthConfigLang < MaxStrLength) {
    NewConfigLang = (EFI_STRING)AllocateZeroPool (MaxStrLength);
    if (NewConfigLang == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for NewConfigLang.\n", __FUNCTION__));
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Search the last "{"
  //
  Index = StrLen (ThisConfigLang) - 1;
  while ((ThisConfigLang[Index] != '{') && (Index >= 0)) {
    Index--;
  }

  if (Index == -1) {
    if (NewConfigLang != ThisConfigLang) {
      FreePool (NewConfigLang);
    }

    return EFI_NOT_FOUND;
  }

  //
  // Copy the string to a new string.
  //
  Length = 0;
  while (Index >= 0) {
    NewConfigLang[Index] = ThisConfigLang[Index];
    Index--;
    Length++;
  }

  UnicodeSPrint ((CHAR16 *)(NewConfigLang + Length), MaxStrLength, L"%d", ConfigLangInstance->Index);
  StrCatS (NewConfigLang, MaxStrLength, L"}");
  if (NewConfigLang != ThisConfigLang) {
    FreePool (ThisConfigLang);
  }

  *DestConfigLang = NewConfigLang;
  return EFI_SUCCESS;
}

/**

  Search HII database with given Configure Language pattern. Data is handled and
  returned in array.

  @param[in]  Schema                    The schema to search.
  @param[in]  Version                   The schema version.
  @param[in]  Pattern                   Configure Language pattern to search.
  @param[out] UnifiedConfigureLangList  The data returned by HII database.

  @retval     EFI_SUCCESS     Data is found and returned.
  @retval     Others          Errors occur.

**/
EFI_STATUS
RedfishFeatureGetUnifiedArrayTypeConfigureLang (
  IN     CHAR8 *Schema,
  IN     CHAR8 *Version,
  IN     EFI_STRING Pattern, OPTIONAL
  OUT    REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *UnifiedConfigureLangList
  )
{
  EFI_STATUS                              Status;
  EFI_STRING                              *ConfigureLangList;
  UINTN                                   Count;
  UINTN                                   Index;
  UINTN                                   Index2;
  UINTN                                   ArrayIndex;
  EFI_STRING                              UnifiedConfigureLang;
  BOOLEAN                                 Duplicated;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG  UnifiedConfigureLangPool[BIOS_CONFIG_TO_REDFISH_REDPATH_POOL_SIZE];

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || (UnifiedConfigureLangList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  UnifiedConfigureLangList->Count = 0;
  UnifiedConfigureLangList->List  = NULL;
  ZeroMem (UnifiedConfigureLangPool, sizeof (UnifiedConfigureLangPool));

  Status = RedfishPlatformConfigGetConfigureLang (Schema, Version, Pattern, &ConfigureLangList, &Count);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: RedfishFeatureGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = GetArrayIndexFromArrayTypeConfigureLang (ConfigureLangList[Index], &UnifiedConfigureLang, &ArrayIndex);
    if (EFI_ERROR (Status) && (Status == EFI_INVALID_PARAMETER)) {
      ASSERT (FALSE);
      continue;
    }

    //
    // Check if this configure language is duplicated.
    //
    Duplicated = FALSE;
    for (Index2 = 0; Index2 < BIOS_CONFIG_TO_REDFISH_REDPATH_POOL_SIZE; Index2++) {
      if (UnifiedConfigureLangPool[Index2].ConfigureLang == NULL) {
        break;
      }

      if (StrCmp (UnifiedConfigureLangPool[Index2].ConfigureLang, UnifiedConfigureLang) == 0) {
        Duplicated = TRUE;
        break;
      }
    }

    if (Duplicated) {
      FreePool (UnifiedConfigureLang);
      continue;
    }

    if (UnifiedConfigureLangList->Count >= BIOS_CONFIG_TO_REDFISH_REDPATH_POOL_SIZE) {
      FreePool (UnifiedConfigureLang);
      Status = EFI_BUFFER_TOO_SMALL;
      break;
    }

    //
    // New configure language. Keep it in Pool
    //

    UnifiedConfigureLangPool[UnifiedConfigureLangList->Count].ConfigureLang = UnifiedConfigureLang;
    UnifiedConfigureLangPool[UnifiedConfigureLangList->Count].Index         = ArrayIndex;
    ++UnifiedConfigureLangList->Count;
  }

  FreePool (ConfigureLangList);

  //
  // Prepare the result to caller.
  //
  UnifiedConfigureLangList->List = AllocateCopyPool (sizeof (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG) * UnifiedConfigureLangList->Count, UnifiedConfigureLangPool);

  return Status;
}

/**

  Find "ETag" and "Location" from either HTTP header or Redfish response.

  @param[in]  Response    HTTP response
  @param[out] Etag        String buffer to return ETag
  @param[out] Location    String buffer to return Location

  @retval     EFI_SUCCESS     Data is found and returned.
  @retval     Others          Errors occur.

**/
EFI_STATUS
GetEtagAndLocation (
  IN  REDFISH_RESPONSE *Response,
  OUT CHAR8 **Etag, OPTIONAL
  OUT EFI_STRING        *Location    OPTIONAL
  )
{
  EDKII_JSON_VALUE  JsonValue;
  EDKII_JSON_VALUE  OdataValue;
  CHAR8             *OdataString;
  CHAR8             *AsciiLocation;
  EFI_HTTP_HEADER   *Header;
  EFI_STATUS        Status;

  if (Response == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Etag == NULL) && (Location == NULL)) {
    return EFI_SUCCESS;
  }

  Status = EFI_SUCCESS;

  if (Etag != NULL) {
    *Etag = NULL;

    if (*(Response->StatusCode) == HTTP_STATUS_200_OK) {
      Header = HttpFindHeader (Response->HeaderCount, Response->Headers, HTTP_HEADER_ETAG);
      if (Header != NULL) {
        *Etag = AllocateCopyPool (AsciiStrSize (Header->FieldValue), Header->FieldValue);
        ASSERT (*Etag != NULL);
      }
    }

    //
    // No header is returned. Search payload for location.
    //
    if ((*Etag == NULL) && (Response->Payload != NULL)) {
      JsonValue = RedfishJsonInPayload (Response->Payload);
      if (JsonValue != NULL) {
        OdataValue = JsonObjectGetValue (JsonValueGetObject (JsonValue), "@odata.etag");
        if (OdataValue != NULL) {
          OdataString = (CHAR8 *)JsonValueGetAsciiString (OdataValue);
          if (OdataString != NULL) {
            *Etag = AllocateCopyPool (AsciiStrSize (OdataString), OdataString);
            ASSERT (*Etag != NULL);
          }
        }

        JsonValueFree (JsonValue);
      }
    }

    if (*Etag == NULL) {
      Status = EFI_NOT_FOUND;
    }
  }

  if (Location != NULL) {
    *Location = NULL;

    if (*(Response->StatusCode) == HTTP_STATUS_200_OK) {
      Header = HttpFindHeader (Response->HeaderCount, Response->Headers, HTTP_HEADER_LOCATION);
      if (Header != NULL) {
        AsciiLocation = AllocateCopyPool (AsciiStrSize (Header->FieldValue), Header->FieldValue);
        ASSERT (AsciiLocation != NULL);
      }
    }

    //
    // No header is returned. Search payload for location.
    //
    if ((*Location == NULL) && (Response->Payload != NULL)) {
      JsonValue = RedfishJsonInPayload (Response->Payload);
      if (JsonValue != NULL) {
        OdataValue = JsonObjectGetValue (JsonValueGetObject (JsonValue), "@odata.id");
        if (OdataValue != NULL) {
          OdataString = (CHAR8 *)JsonValueGetAsciiString (OdataValue);
          if (OdataString != NULL) {
            AsciiLocation = AllocateCopyPool (AsciiStrSize (OdataString), OdataString);
            ASSERT (AsciiLocation != NULL);
          }
        }

        JsonValueFree (JsonValue);
      }
    }

    if (AsciiLocation != NULL) {
      *Location = StrAsciiToUnicode (AsciiLocation);
      FreePool (AsciiLocation);
    } else {
      Status = EFI_NOT_FOUND;
    }
  }

  return Status;
}

/**

  Create HTTP payload and send them to redfish service with PUT method.

  @param[in]  Service         Redfish service.
  @param[in]  TargetPayload   Target payload
  @param[in]  Json            Data in JSON format.
  @param[out] Etag            Returned ETAG string from Redfish service.

  @retval     EFI_SUCCESS     Data is sent to redfish service successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
CreatePayloadToPutResource (
  IN  REDFISH_SERVICE  *Service,
  IN  REDFISH_PAYLOAD  *TargetPayload,
  IN  CHAR8            *Json,
  OUT CHAR8            **Etag
  )
{
  REDFISH_PAYLOAD   Payload;
  EDKII_JSON_VALUE  ResourceJsonValue;
  REDFISH_RESPONSE  PostResponse;
  EFI_STATUS        Status;

  if ((Service == NULL) || (TargetPayload == NULL) || IS_EMPTY_STRING (Json) || (Etag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ResourceJsonValue = JsonLoadString (Json, 0, NULL);
  Payload           = RedfishCreatePayload (ResourceJsonValue, Service);
  if (Payload == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to create JSON payload from JSON value!\n", __FUNCTION__, __LINE__));
    Status =  EFI_DEVICE_ERROR;
    goto EXIT_FREE_JSON_VALUE;
  }

  ZeroMem (&PostResponse, sizeof (REDFISH_RESPONSE));
  Status = RedfishPutToPayload (TargetPayload, Payload, &PostResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to PUT payload to Redfish service.\n", __FUNCTION__, __LINE__));

    DEBUG_CODE_BEGIN ();
    DEBUG ((DEBUG_ERROR, "%a: Request:\n", __FUNCTION__));
    DumpRedfishPayload (DEBUG_ERROR, Payload);
    DEBUG ((DEBUG_ERROR, "%a: Response:\n", __FUNCTION__));
    DumpRedfishResponse (__FUNCTION__, DEBUG_ERROR, &PostResponse);
    DEBUG_CODE_END ();

    goto EXIT_FREE_JSON_VALUE;
  }

  //
  // Find ETag
  //
  Status = GetEtagAndLocation (&PostResponse, Etag, NULL);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
  }

  RedfishFreeResponse (
    PostResponse.StatusCode,
    PostResponse.HeaderCount,
    PostResponse.Headers,
    PostResponse.Payload
    );

EXIT_FREE_JSON_VALUE:
  if (Payload != NULL) {
    RedfishCleanupPayload (Payload);
  }

  JsonValueFree (ResourceJsonValue);

  return Status;
}

/**

  Create HTTP payload and send them to redfish service with PUT method.

  @param[in]  Service         Redfish service.
  @param[in]  TargetPayload   Target payload
  @param[in]  Json            Data in JSON format.
  @param[out] Etag            Returned ETAG string from Redfish service.

  @retval     EFI_SUCCESS     Data is sent to redfish service successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
CreatePayloadToPatchResource (
  IN  REDFISH_SERVICE  *Service,
  IN  REDFISH_PAYLOAD  *TargetPayload,
  IN  CHAR8            *Json,
  OUT CHAR8            **Etag
  )
{
  REDFISH_PAYLOAD   Payload;
  EDKII_JSON_VALUE  ResourceJsonValue;
  REDFISH_RESPONSE  PostResponse;
  EFI_STATUS        Status;

  if ((Service == NULL) || (TargetPayload == NULL) || IS_EMPTY_STRING (Json) || (Etag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ResourceJsonValue = JsonLoadString (Json, 0, NULL);
  Payload           = RedfishCreatePayload (ResourceJsonValue, Service);
  if (Payload == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to create JSON payload from JSON value!\n", __FUNCTION__, __LINE__));
    Status =  EFI_DEVICE_ERROR;
    goto EXIT_FREE_JSON_VALUE;
  }

  ZeroMem (&PostResponse, sizeof (REDFISH_RESPONSE));
  Status = RedfishPatchToPayload (TargetPayload, Payload, &PostResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to PATCH payload to Redfish service.\n", __FUNCTION__, __LINE__));

    DEBUG_CODE_BEGIN ();
    DEBUG ((DEBUG_ERROR, "%a: Request:\n", __FUNCTION__));
    DumpRedfishPayload (DEBUG_ERROR, Payload);
    DEBUG ((DEBUG_ERROR, "%a: Response:\n", __FUNCTION__));
    DumpRedfishResponse (__FUNCTION__, DEBUG_ERROR, &PostResponse);
    DEBUG_CODE_END ();

    goto EXIT_FREE_JSON_VALUE;
  }

  //
  // Find ETag
  //
  Status = GetEtagAndLocation (&PostResponse, Etag, NULL);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
  }

  RedfishFreeResponse (
    PostResponse.StatusCode,
    PostResponse.HeaderCount,
    PostResponse.Headers,
    PostResponse.Payload
    );

EXIT_FREE_JSON_VALUE:
  if (Payload != NULL) {
    RedfishCleanupPayload (Payload);
  }

  JsonValueFree (ResourceJsonValue);

  return Status;
}

/**

  Create HTTP payload and send them to redfish service with POST method.

  @param[in]  Service         Redfish service.
  @param[in]  TargetPayload   Target payload
  @param[in]  Json            Data in JSON format.
  @param[out] Location        Returned location string from Redfish service.
  @param[out] Etag            Returned ETAG string from Redfish service.

  @retval     EFI_SUCCESS     Data is sent to redfish service successfully.
  @retval     Others          Errors occur.

**/
EFI_STATUS
CreatePayloadToPostResource (
  IN  REDFISH_SERVICE  *Service,
  IN  REDFISH_PAYLOAD  *TargetPayload,
  IN  CHAR8            *Json,
  OUT EFI_STRING       *Location,
  OUT CHAR8            **Etag
  )
{
  REDFISH_PAYLOAD   Payload;
  EDKII_JSON_VALUE  ResourceJsonValue;
  REDFISH_RESPONSE  PostResponse;
  EFI_STATUS        Status;

  if ((Service == NULL) || (TargetPayload == NULL) || IS_EMPTY_STRING (Json) || (Location == NULL) || (Etag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ResourceJsonValue = JsonLoadString (Json, 0, NULL);
  Payload           = RedfishCreatePayload (ResourceJsonValue, Service);
  if (Payload == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to create JSON payload from JSON value!\n", __FUNCTION__, __LINE__));
    Status =  EFI_DEVICE_ERROR;
    goto EXIT_FREE_JSON_VALUE;
  }

  ZeroMem (&PostResponse, sizeof (REDFISH_RESPONSE));
  Status = RedfishPostToPayload (TargetPayload, Payload, &PostResponse);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Failed to POST payload to Redfish service.\n", __FUNCTION__, __LINE__));

    DEBUG_CODE_BEGIN ();
    DEBUG ((DEBUG_ERROR, "%a: Request:\n", __FUNCTION__));
    DumpRedfishPayload (DEBUG_ERROR, Payload);
    DEBUG ((DEBUG_ERROR, "%a: Response:\n", __FUNCTION__));
    DumpRedfishResponse (__FUNCTION__, DEBUG_ERROR, &PostResponse);
    DEBUG_CODE_END ();

    goto EXIT_FREE_JSON_VALUE;
  }

  //
  // per Redfish spec. the URL of new resource will be returned in "Location" header.
  //
  Status = GetEtagAndLocation (&PostResponse, Etag, Location);
  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
  }

  RedfishFreeResponse (
    PostResponse.StatusCode,
    PostResponse.HeaderCount,
    PostResponse.Headers,
    PostResponse.Payload
    );

  RedfishCleanupPayload (Payload);

EXIT_FREE_JSON_VALUE:
  JsonValueFree (ResourceJsonValue);

  return Status;
}

/**

  Return redfish URI by given config language. It's call responsibility to release returned buffer.

  @param[in]  ConfigLang    ConfigLang to search.

  @retval  NULL     Can not find redfish uri.
  @retval  Other    redfish uri is returned.

**/
EFI_STRING
RedfishGetUri (
  IN  EFI_STRING  ConfigLang
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Target;
  EFI_STRING  Found;
  EFI_STRING  TempStr;
  EFI_STRING  ResultStr;
  EFI_STRING  Head;
  EFI_STRING  CloseBracket;
  UINTN       TempStrSize;
  UINTN       RemainingLen;
  UINTN       ConfigLangLen;

  Status = RedfishLocateProtocol ((VOID **)&mConfigLangMapProtocol, &gEdkIIRedfishConfigLangMapProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __FUNCTION__, Status));
    return NULL;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: Get: %s\n", __FUNCTION__, ConfigLang));

  CloseBracket = StrStr (ConfigLang, L"{");
  if (CloseBracket == NULL) {
    return AllocateCopyPool (StrSize (ConfigLang), ConfigLang);
  }

  //
  // Remove leading "/v1" or "/redfish/v1" because we don't code
  // configure language in this way.
  //
  Head = StrStr (ConfigLang, REDFISH_ROOT_PATH_UNICODE);
  if (Head == NULL) {
    Head = ConfigLang;
  } else {
    Head += 3;
  }

  ResultStr = AllocateZeroPool (sizeof (CHAR16) * MAX_REDFISH_URL_LEN);
  if (ResultStr == NULL) {
    return NULL;
  }

  //
  // Go though ConfigLang and replace each {} with URL
  //
  do {
    ConfigLangLen = StrLen (Head);
    Target        = CloseBracket;

    //
    // Look for next ConfigLang
    //
    do {
      Target += 1;
    } while (*Target != '\0' && *Target != '}');

    //
    // Invalid format. No '}' found
    //
    if (*Target == '\0') {
      DEBUG ((DEBUG_ERROR, "%a: invalid format: %s\n", __FUNCTION__, ConfigLang));
      return NULL;
    }

    //
    // Copy current ConfigLang to temporary string and do a query
    //
    Target      += 1;
    RemainingLen = StrLen (Target);
    TempStrSize  = (ConfigLangLen - RemainingLen + 1) * sizeof (CHAR16);
    TempStr      = AllocateCopyPool (TempStrSize, Head);
    if (TempStr == NULL) {
      return NULL;
    }

    TempStr[ConfigLangLen - RemainingLen] = '\0';

    Status = mConfigLangMapProtocol->Get (
                                       mConfigLangMapProtocol,
                                       RedfishGetTypeConfigLang,
                                       TempStr,
                                       &Found
                                       );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Can not find: %s\n", __FUNCTION__, TempStr));
      return NULL;
    }

    DEBUG ((REDFISH_DEBUG_TRACE, "%a: Found: %s\n", __FUNCTION__, Found));

    //
    // Keep result in final string pool
    //
    StrCatS (ResultStr, MAX_REDFISH_URL_LEN, Found);
    FreePool (TempStr);

    //
    // Prepare for next ConfigLang
    //
    Head         = Target;
    CloseBracket = StrStr (Head, L"{");
  } while (CloseBracket != NULL);

  //
  // String which has no ConfigLang remaining
  //
  if (*Head != L'\0') {
    StrCatS (ResultStr, MAX_REDFISH_URL_LEN, Head);
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: return: %s\n", __FUNCTION__, ResultStr));

  return ResultStr;
}

/**

  Return config language by given URI. It's call responsibility to release returned buffer.

  @param[in]  Uri   Uri to search.

  @retval  NULL     Can not find redfish uri.
  @retval  Other    redfish uri is returned.

**/
EFI_STRING
RedfishGetConfigLanguage (
  IN  EFI_STRING  Uri
  )
{
  EFI_STATUS  Status;
  EFI_STRING  ConfigLang;

  if (IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: search config lang for URI: %s\n", __FUNCTION__, Uri));

  Status = RedfishLocateProtocol ((VOID **)&mConfigLangMapProtocol, &gEdkIIRedfishConfigLangMapProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __FUNCTION__, Status));
    return NULL;
  }

  ConfigLang = NULL;
  Status     = mConfigLangMapProtocol->Get (
                                         mConfigLangMapProtocol,
                                         RedfishGetTypeUri,
                                         Uri,
                                         &ConfigLang
                                         );

  return ConfigLang;
}

/**

  Return config language from given URI and property name. It's call responsibility to release returned buffer.

  @param[in] Uri            The URI to match
  @param[in] PropertyName   The property name of resource. This is optional.

  @retval  NULL     Can not find redfish uri.
  @retval  Other    redfish uri is returned.

**/
EFI_STRING
GetConfigureLang (
  IN  CHAR8  *Uri,
  IN  CHAR8  *PropertyName   OPTIONAL
  )
{
  EFI_STRING  ConfigLang;
  UINTN       StringSize;
  EFI_STRING  ResultStr;
  EFI_STRING  UnicodeUri;
  EFI_STATUS  Status;
  EFI_STRING  StrFound;

  if (IS_EMPTY_STRING (Uri)) {
    return NULL;
  }

  StringSize = AsciiStrSize (Uri);
  UnicodeUri = AllocatePool (StringSize * sizeof (CHAR16));
  if (UnicodeUri == NULL) {
    return NULL;
  }

  Status = AsciiStrToUnicodeStrS (Uri, UnicodeUri, StringSize);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  ConfigLang = RedfishGetConfigLanguage (UnicodeUri);
  if (ConfigLang == NULL) {
    //
    // @Redfish.Settings share the same schema as its parent.
    // Remove "Settings" and try again.
    //
    StrFound = StrStr (UnicodeUri, L"/Settings");
    if (StrFound != NULL) {
      StrFound[0] = L'\0';
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: \"Settings\" found in URI, try: %s\n", __FUNCTION__, UnicodeUri));
      ConfigLang = RedfishGetConfigLanguage (UnicodeUri);
    }

    if (ConfigLang == NULL) {
      return NULL;
    }
  }

  if (IS_EMPTY_STRING (PropertyName)) {
    return ConfigLang;
  }

  StringSize = StrSize (ConfigLang) + ((AsciiStrLen (PropertyName) + 1) * sizeof (CHAR16));
  ResultStr  = AllocatePool (StringSize);
  if (ResultStr == NULL) {
    FreePool (ConfigLang);
    return NULL;
  }

  UnicodeSPrint (ResultStr, StringSize, L"%s/%a", ConfigLang, PropertyName);
  FreePool (ConfigLang);

  return ResultStr;
}

/**

  Save Redfish URI in database for further use.

  @param[in]    ConfigLang        ConfigLang to save
  @param[in]    Uri               Redfish Uri to save

  @retval  EFI_INVALID_PARAMETER  SystemId is NULL or EMPTY
  @retval  EFI_SUCCESS            Redfish uri is saved

**/
EFI_STATUS
RedfishSetRedfishUri (
  IN    EFI_STRING  ConfigLang,
  IN    EFI_STRING  Uri
  )
{
  EFI_STATUS  Status;

  if (IS_EMPTY_STRING (ConfigLang) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishLocateProtocol ((VOID **)&mConfigLangMapProtocol, &gEdkIIRedfishConfigLangMapProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __FUNCTION__, Status));
    return Status;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: Saved: %s -> %s\n", __FUNCTION__, ConfigLang, Uri));

  return mConfigLangMapProtocol->Set (mConfigLangMapProtocol, ConfigLang, Uri);
}

/**

  Save Config Language list to variable immediately.

**/
VOID
RedfishSaveConfigList (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = RedfishLocateProtocol ((VOID **)&mConfigLangMapProtocol, &gEdkIIRedfishConfigLangMapProtocolGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: fail to locate gEdkIIRedfishConfigLangMapProtocolGuid: %r\n", __FUNCTION__, Status));
    return;
  }

  mConfigLangMapProtocol->Flush (mConfigLangMapProtocol);
}

/**

  Get @odata.id from give HTTP payload. It's call responsibility to release returned buffer.

  @param[in]  Payload             HTTP payload

  @retval     NULL                Can not find @odata.id from given payload.
  @retval     Others              odata.id string is returned.

**/
EFI_STRING
GetOdataId (
  IN  REDFISH_PAYLOAD  *Payload
  )
{
  EDKII_JSON_VALUE  *JsonValue;
  EDKII_JSON_VALUE  *OdataId;
  EFI_STRING        OdataIdString;

  if (Payload == NULL) {
    return NULL;
  }

  JsonValue = RedfishJsonInPayload (Payload);
  if (!JsonValueIsObject (JsonValue)) {
    return NULL;
  }

  OdataId = JsonObjectGetValue (JsonValueGetObject (JsonValue), "@odata.id");
  if (!JsonValueIsString (OdataId)) {
    return NULL;
  }

  OdataIdString = JsonValueGetUnicodeString (OdataId);
  if (OdataIdString == NULL) {
    return NULL;
  }

  return AllocateCopyPool (StrSize (OdataIdString), OdataIdString);
}

/**

  Get the property name by given Configure Language.

  @param[in]  ResourceUri              URI of root of resource.
  @param[in]  ConfigureLang            Configure Language string.

  @retval     EFI_STRING      Pointer to property name.
  @retval     NULL            There is error.

**/
EFI_STRING
GetPropertyFromConfigureLang (
  IN EFI_STRING  ResourceUri,
  IN EFI_STRING  ConfigureLang
  )
{
  EFI_STATUS  Status;
  EFI_STRING  TempString;

  if ((ConfigureLang == NULL) || (ResourceUri == NULL)) {
    return NULL;
  }

  Status = IsRedpathArray (ConfigureLang, NULL, &TempString);
  if (!EFI_ERROR (Status)) {
    TempString += 2; // Advance two characters for '}' and '/'
    return TempString;
  }

  if (Status != EFI_NOT_FOUND) {
    return NULL;
  }

  //
  // The ConfigLang has no '{}'
  //
  if (GetNumberOfRedpathNodes (ConfigureLang) == 1) {
    return NULL;
  }

  if (GetRedpathNodeByIndex (ConfigureLang, 0, &TempString) == NULL) {
    return NULL;
  }

  //
  // Advance two characters to the starting
  // pointer of next node.
  //
  return TempString + 2;
}

/**

  Get the property value in string type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.

  @retval     CHAR8*        Pointer to the CHAR8 buffer.
  @retval     NULL          There is error.

**/
CHAR8 *
GetPropertyStringValue (
  IN CHAR8       *Schema,
  IN CHAR8       *Version,
  IN EFI_STRING  PropertyName,
  IN EFI_STRING  ConfigureLang
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  CHAR8                *AsciiStringValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName)) {
    return NULL;
  }

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_STRING) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  AsciiStringValue = AllocateCopyPool (AsciiStrSize (RedfishValue.Value.Buffer), RedfishValue.Value.Buffer);
  ASSERT (AsciiStringValue != NULL);

  return AsciiStringValue;
}

/**

  Get the property value in numeric type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.

  @retval     INT64*        Pointer to the INT64 value.
  @retval     NULL          There is error.

**/
INT64 *
GetPropertyNumericValue (
  IN CHAR8       *Schema,
  IN CHAR8       *Version,
  IN EFI_STRING  PropertyName,
  IN EFI_STRING  ConfigureLang
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  INT64                *ResultValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName)) {
    return NULL;
  }

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_INTEGER) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not numeric type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  ResultValue = AllocatePool (sizeof (INT64));
  ASSERT (ResultValue != NULL);
  if (ResultValue == NULL) {
    return NULL;
  }

  *ResultValue = RedfishValue.Value.Integer;

  return ResultValue;
}

/**

  Get the property value in Boolean type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.

  @retval     BOOLEAN       Boolean value returned by this property.

**/
BOOLEAN *
GetPropertyBooleanValue (
  IN CHAR8       *Schema,
  IN CHAR8       *Version,
  IN EFI_STRING  PropertyName,
  IN EFI_STRING  ConfigureLang
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  BOOLEAN              *ResultValue;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName)) {
    return NULL;
  }

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_BOOLEAN) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not boolean type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  ResultValue = AllocatePool (sizeof (BOOLEAN));
  ASSERT (ResultValue != NULL);
  if (ResultValue == NULL) {
    return NULL;
  }

  *ResultValue = RedfishValue.Value.Boolean;

  return ResultValue;
}

/**

  Return the last string of configure language. Any modification to returned
  string will change ConfigureLanguage.

  @param[in]  ConfigureLanguage Configure language string

  @retval     EFI_STRING        Attribute name is returned
  @retval     NULL              Error occurs

**/
EFI_STRING
GetAttributeNameFromConfigLanguage (
  IN  EFI_STRING  ConfigureLanguage
  )
{
  UINTN  StringLen;
  UINTN  Index;

  if (IS_EMPTY_STRING (ConfigureLanguage)) {
    return NULL;
  }

  StringLen = StrLen (ConfigureLanguage);
  for (Index = StringLen - 1; Index >= 0; Index--) {
    if (ConfigureLanguage[Index] == '/') {
      return &ConfigureLanguage[Index + 1];
    }
  }

  return NULL;
}

/**

  Get the property string value in array type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.
  @param[out] ArraySize     The size of returned array.

  @retval     CHAR8 **      Returned string array. NULL while error happens.

**/
CHAR8 **
GetPropertyStringArrayValue (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  PropertyName,
  IN  EFI_STRING  ConfigureLang,
  OUT UINTN       *ArraySize
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  CHAR8                **StringArray;
  UINTN                Index;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName) || (ArraySize == NULL)) {
    return NULL;
  }

  *ArraySize = 0;

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_STRING_ARRAY) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  StringArray = AllocatePool (sizeof (CHAR8 *) * RedfishValue.ArrayCount);
  if (StringArray == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  *ArraySize = RedfishValue.ArrayCount;
  for (Index = 0; Index < RedfishValue.ArrayCount; Index++) {
    StringArray[Index] = RedfishValue.Value.StringArray[Index];
  }

  return StringArray;
}

/**

  Get the property numeric value in array type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.
  @param[out] ArraySize     The size of returned array.

  @retval     INT64 *      Returned integer array. NULL while error happens.

**/
INT64 *
GetPropertyNumericArrayValue (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  PropertyName,
  IN  EFI_STRING  ConfigureLang,
  OUT UINTN       *ArraySize
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  INT64                *IntegerArray;
  UINTN                Index;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName) || (ArraySize == NULL)) {
    return NULL;
  }

  *ArraySize = 0;

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_INTEGER_ARRAY) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  IntegerArray = AllocatePool (sizeof (INT64) * RedfishValue.ArrayCount);
  if (IntegerArray == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  *ArraySize = RedfishValue.ArrayCount;
  for (Index = 0; Index < RedfishValue.ArrayCount; Index++) {
    IntegerArray[Index] = RedfishValue.Value.IntegerArray[Index];
  }

  return IntegerArray;
}

/**

  Get the property boolean value in array type.

  @param[in]  Schema        Schema of this property.
  @param[in]  Version       Schema version.
  @param[in]  PropertyName  Property name.
  @param[in]  ConfigureLang Configure Language of this property.
  @param[out] ArraySize     The size of returned array.

  @retval     BOOLEAN *      Returned boolean array. NULL while error happens.

**/
BOOLEAN *
GetPropertyBooleanArrayValue (
  IN  CHAR8       *Schema,
  IN  CHAR8       *Version,
  IN  EFI_STRING  PropertyName,
  IN  EFI_STRING  ConfigureLang,
  OUT UINTN       *ArraySize
  )
{
  EFI_STATUS           Status;
  EDKII_REDFISH_VALUE  RedfishValue;
  EFI_STRING           ConfigureLangBuffer;
  UINTN                BufferSize;
  BOOLEAN              *BooleanArray;
  UINTN                Index;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName) || (ArraySize == NULL)) {
    return NULL;
  }

  *ArraySize = 0;

  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);
  Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangBuffer, &RedfishValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    return NULL;
  }

  if (RedfishValue.Type != REDFISH_VALUE_TYPE_BOOLEAN_ARRAY) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a %s value is not string array type\n", __FUNCTION__, Schema, Version, ConfigureLang));
    return NULL;
  }

  BooleanArray = AllocatePool (sizeof (INT64) * RedfishValue.ArrayCount);
  if (BooleanArray == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: out of resource\n", __FUNCTION__));
    return NULL;
  }

  *ArraySize = RedfishValue.ArrayCount;
  for (Index = 0; Index < RedfishValue.ArrayCount; Index++) {
    BooleanArray[Index] = RedfishValue.Value.BooleanArray[Index];
  }

  return BooleanArray;
}

/**

  Free the list of empty property key values.

  @param[in]  EmptyPropKeyValueListHead  The head of RedfishCS_EmptyProp_KeyValue

**/
VOID
FreeEmptyPropKeyValueList (
  RedfishCS_EmptyProp_KeyValue  *EmptyPropKeyValueListHead
  )
{
  RedfishCS_EmptyProp_KeyValue  *NextEmptyPropKeyValueList;

  while (EmptyPropKeyValueListHead != NULL) {
    NextEmptyPropKeyValueList = EmptyPropKeyValueListHead->NextKeyValuePtr;
    if (EmptyPropKeyValueListHead->Value->DataValue.CharPtr != NULL) {
      FreePool (EmptyPropKeyValueListHead->Value->DataValue.CharPtr);
    }

    if (EmptyPropKeyValueListHead->Value != NULL) {
      FreePool (EmptyPropKeyValueListHead->Value);
    }

    if (EmptyPropKeyValueListHead->KeyNamePtr != NULL) {
      FreePool (EmptyPropKeyValueListHead->KeyNamePtr);
    }

    FreePool (EmptyPropKeyValueListHead);
    EmptyPropKeyValueListHead = NextEmptyPropKeyValueList;
  }
}

/**

  Create a new entry of RedfishCS_EmptyProp_KeyValue

  @param[in]  KeyName        The key name.
  @param[in]  RedfishValue   Redfish vale of this key.

* @retval     RedfishCS_EmptyProp_KeyValue   Return the new RedfishCS_EmptyProp_KeyValue.
*                                            NULL means no new entry is created.

**/
RedfishCS_EmptyProp_KeyValue *
NewEmptyPropKeyValueFromRedfishValue (
  IN  EFI_STRING           KeyName,
  IN  EDKII_REDFISH_VALUE  *RedfishValue
  )
{
  RedfishCS_EmptyProp_KeyValue  *EmptyPropKeyValue;
  RedfishCS_Vague               *VagueValue;
  RedfishCS_char                *KeyNameChar;
  VOID                          *Data;
  UINTN                         DataSize;
  INT32                         Bool32;

  KeyNameChar = StrUnicodeToAscii (KeyName);
  if (KeyNameChar == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to convert unicode to ASCII.\n", __FUNCTION__));
    return NULL;
  }

  EmptyPropKeyValue = (RedfishCS_EmptyProp_KeyValue *)AllocateZeroPool (sizeof (RedfishCS_EmptyProp_KeyValue));
  if (EmptyPropKeyValue == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for EmptyPropKeyValue\n", __FUNCTION__));
    return NULL;
  }

  VagueValue = (RedfishCS_Vague *)AllocateZeroPool (sizeof (RedfishCS_Vague));
  if (VagueValue == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for VagueValue\n", __FUNCTION__));
    FreePool (EmptyPropKeyValue);
    return NULL;
  }

  if (RedfishValue->Type == REDFISH_VALUE_TYPE_BOOLEAN) {
    VagueValue->DataType = RedfishCS_Vague_DataType_Bool;
    DataSize             = sizeof (BOOLEAN);
    //
    // Redfish JSON to C structure converter uses
    // "int" for the BOOLEAN.
    //
    Bool32 = (INT32)RedfishValue->Value.Boolean;
    Data   = (VOID *)&Bool32;
  } else if (RedfishValue->Type == REDFISH_VALUE_TYPE_INTEGER) {
    VagueValue->DataType = RedfishCS_Vague_DataType_Int64;
    DataSize             = sizeof (INT64);
    Data                 = (VOID *)&RedfishValue->Value.Integer;
  } else if (RedfishValue->Type == REDFISH_VALUE_TYPE_STRING) {
    VagueValue->DataType = RedfishCS_Vague_DataType_String;
    DataSize             = AsciiStrSize (RedfishValue->Value.Buffer);
    Data                 = (VOID *)RedfishValue->Value.Buffer;
  } else {
    DEBUG ((DEBUG_ERROR, "%a: wrong type of RedfishValue: %x\n", __FUNCTION__, RedfishValue->Type));
    FreePool (VagueValue);
    FreePool (EmptyPropKeyValue);
    return NULL;
  }

  VagueValue->DataValue.CharPtr = (RedfishCS_char *)AllocateCopyPool (DataSize, Data);
  EmptyPropKeyValue->Value      = VagueValue;
  EmptyPropKeyValue->KeyNamePtr = KeyNameChar;
  return EmptyPropKeyValue;
}

/**

  Get the property value in the vague type.

  @param[in]  Schema          Schema of this property.
  @param[in]  Version         Schema version.
  @param[in]  PropertyName    Property name.
  @param[in]  ConfigureLang   Configure Language of this property.
  @param[out] NumberOfValues  Return the number of vague type of values

  @retval     RedfishCS_EmptyProp_KeyValue   The pointer to the structure
                                             of vague type of values.

**/
RedfishCS_EmptyProp_KeyValue *
GetPropertyVagueValue (
  IN CHAR8       *Schema,
  IN CHAR8       *Version,
  IN EFI_STRING  PropertyName,
  IN EFI_STRING  ConfigureLang,
  OUT UINT32     *NumberOfValues
  )
{
  EFI_STATUS                    Status;
  RedfishCS_EmptyProp_KeyValue  *EmptyPropKeyValueList;
  RedfishCS_EmptyProp_KeyValue  *PreEmptyPropKeyValueList;
  RedfishCS_EmptyProp_KeyValue  *FirstEmptyPropKeyValueList;
  EDKII_REDFISH_VALUE           RedfishValue;
  EFI_STRING                    ConfigureLangBuffer;
  EFI_STRING                    KeyName;
  EFI_STRING                    *ConfigureLangList;
  EFI_STRING                    SearchPattern;
  UINTN                         BufferSize;
  UINTN                         ConfigListCount;
  UINTN                         ConfigListCountIndex;
  UINTN                         ListCount;

  if (IS_EMPTY_STRING (Schema) || IS_EMPTY_STRING (Version) || IS_EMPTY_STRING (ConfigureLang) || IS_EMPTY_STRING (PropertyName)) {
    return NULL;
  }

  ListCount                  = 0;
  FirstEmptyPropKeyValueList = 0;
  //
  // Configure Language buffer.
  //
  BufferSize          = sizeof (CHAR16) * MAX_CONF_LANG_LEN;
  ConfigureLangBuffer = AllocatePool (BufferSize);
  if (ConfigureLangBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for ConfigureLangBuffer\n", __FUNCTION__));
    return NULL;
  }

  UnicodeSPrint (ConfigureLangBuffer, BufferSize, L"%s/%s", ConfigureLang, PropertyName);

  //
  // Initial search pattern
  //
  BufferSize    = (StrLen (ConfigureLangBuffer) + StrLen (L"/.*") + 1) * sizeof (CHAR16); // Increase one for the NULL terminator.
  SearchPattern = AllocatePool (BufferSize);
  if (SearchPattern == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate memory for SearchPattern\n", __FUNCTION__));
    FreePool (ConfigureLangBuffer);
    return NULL;
  }

  BufferSize = BufferSize / sizeof (CHAR16);
  StrCpyS (SearchPattern, BufferSize, ConfigureLangBuffer);
  StrCatS (SearchPattern, BufferSize, L"/.*");
  Status = RedfishPlatformConfigGetConfigureLang (Schema, Version, SearchPattern, &ConfigureLangList, &ConfigListCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: %a.%a Get configure language of vague type values of %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangBuffer, Status));
    goto ErrorLeave;
  }

  //
  // Build up the list of RedfishCS_EmptyProp_KeyValue.
  //
  ListCount                  = 0;
  ConfigListCountIndex       = 0;
  PreEmptyPropKeyValueList   = NULL;
  FirstEmptyPropKeyValueList = NULL;
  while (ConfigListCountIndex < ConfigListCount) {
    Status = RedfishPlatformConfigGetValue (Schema, Version, ConfigureLangList[ConfigListCountIndex], &RedfishValue);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: %a.%a query current setting for %s failed: %r\n", __FUNCTION__, Schema, Version, ConfigureLangList[ConfigListCountIndex], Status));
      ConfigListCountIndex++;
      continue;
    }

    //
    // Get the key name.
    //
    KeyName = GetAttributeNameFromConfigLanguage (ConfigureLangList[ConfigListCountIndex]);
    //
    // Create an entry of RedfishCS_EmptyProp_KeyValue.
    //
    EmptyPropKeyValueList = NewEmptyPropKeyValueFromRedfishValue (KeyName, &RedfishValue);
    if (EmptyPropKeyValueList == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: Failed to create an entry of EmptyPropKeyValueList\n", __FUNCTION__));
      ConfigListCountIndex++;
      continue;
    }

    //
    // Link the RedfishCS_EmptyProp_KeyValue list.
    //
    if (PreEmptyPropKeyValueList != NULL) {
      PreEmptyPropKeyValueList->NextKeyValuePtr = EmptyPropKeyValueList;
    } else {
      FirstEmptyPropKeyValueList = EmptyPropKeyValueList;
    }

    PreEmptyPropKeyValueList = EmptyPropKeyValueList;
    ConfigListCountIndex++;
    ListCount++;
  }

  goto LeaveFunction;

ErrorLeave:;
  if (FirstEmptyPropKeyValueList != NULL) {
    FreeEmptyPropKeyValueList (FirstEmptyPropKeyValueList);
  }

  FirstEmptyPropKeyValueList = NULL;

LeaveFunction:
  if (SearchPattern != NULL) {
    FreePool (SearchPattern);
  }

  if (ConfigureLangBuffer != NULL) {
    FreePool (ConfigureLangBuffer);
  }

  FreePool (ConfigureLangList);

  *NumberOfValues = (UINT32)ListCount;
  return FirstEmptyPropKeyValueList;
}

/**

  Check and see if we need to do provisioning for this property.

  @param[in]  PropertyBuffer   Pointer to property instance.
  @param[in]  ProvisionMode    TRUE if we are in provision mode. FALSE otherwise.

  @retval     TRUE             Provision is required.
  @retval     FALSE            Provision is not required.

**/
BOOLEAN
PropertyChecker (
  IN VOID     *PropertyBuffer,
  IN BOOLEAN  ProvisionMode
  )
{
  if (ProvisionMode) {
    return TRUE;
  }

  if (!ProvisionMode && (PropertyBuffer != NULL)) {
    return TRUE;
  }

  return FALSE;
}

/**

  Check and see if ETAG is identical to what we keep in system.

  @param[in]  Uri           URI requested
  @param[in]  EtagInHeader  ETAG string returned from HTTP request.
  @param[in]  EtagInJson    ETAG string in JSON body.

  @retval     TRUE          ETAG is identical.
  @retval     FALSE         ETAG is changed.

**/
BOOLEAN
CheckEtag (
  IN EFI_STRING  Uri,
  IN CHAR8       *EtagInHeader,
  IN CHAR8       *EtagInJson
  )
{
  CHAR8  *EtagInDb;

  if (IS_EMPTY_STRING (Uri)) {
    return FALSE;
  }

  if (IS_EMPTY_STRING (EtagInHeader) && IS_EMPTY_STRING (EtagInJson)) {
    return FALSE;
  }

  //
  // Check ETAG to see if we need to consume it
  //
  EtagInDb = NULL;
  EtagInDb = GetEtagWithUri (Uri);
  if (EtagInDb == NULL) {
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: no ETAG record could be found for: %s\n", __FUNCTION__, Uri));
    return FALSE;
  }

  if (EtagInHeader != NULL) {
    if (AsciiStrCmp (EtagInDb, EtagInHeader) == 0) {
      FreePool (EtagInDb);
      return TRUE;
    }
  }

  if (EtagInJson != NULL) {
    if (AsciiStrCmp (EtagInDb, EtagInJson) == 0) {
      FreePool (EtagInDb);
      return TRUE;
    }
  }

  DEBUG ((ETAG_DEBUG_TRACE, "%a: Etag: %a Db: %a\n", __FUNCTION__, EtagInHeader, EtagInDb));

  FreePool (EtagInDb);

  return FALSE;
}

/**
  Check and see if given ObjectName can be found in JsonObj or not

  @param[in]  JsonObj       JSON object to search
  @param[in]  ObjectName    Object name

  @retval     EDKII_JSON_VALUE *  Pointer to Json object is found. NULL otherwise.

**/
EDKII_JSON_VALUE *
MatchJsonObject (
  IN EDKII_JSON_VALUE  *JsonObj,
  IN CHAR8             *ObjectName
  )
{
  EDKII_JSON_VALUE  N;
  CHAR8             *Key;
  EDKII_JSON_VALUE  Value;

  if ((JsonObj == NULL) || IS_EMPTY_STRING (ObjectName)) {
    return NULL;
  }

  if (!JsonValueIsObject (JsonObj)) {
    return NULL;
  }

  EDKII_JSON_OBJECT_FOREACH_SAFE (JsonObj, N, Key, Value) {
    if (AsciiStrCmp (Key, ObjectName) == 0) {
      return Value;
    }
  }

  return NULL;
}

/**

  Check and see if given property is in JSON context or not

  @param[in]  Property      Property name string
  @param[in]  Json          The JSON context to search.

  @retval     TRUE          Property is found in JSON context
  @retval     FALSE         Property is not in JSON context

**/
BOOLEAN
MatchPropertyWithJsonContext (
  IN  EFI_STRING  Property,
  IN  CHAR8       *Json
  )
{
  CHAR8             *AsciiProperty;
  CHAR8             *PropertyNode;
  UINTN             Index;
  EDKII_JSON_VALUE  *JsonObj;
  EDKII_JSON_VALUE  *MatchObj;
  EDKII_JSON_TYPE   JsonType;

  if (IS_EMPTY_STRING (Property) || IS_EMPTY_STRING (Json)) {
    return FALSE;
  }

  JsonObj = JsonLoadString (Json, 0, NULL);
  if ((JsonObj == NULL) || !JsonValueIsObject (JsonObj)) {
    return FALSE;
  }

  AsciiProperty = StrUnicodeToAscii (Property);
  if (AsciiProperty == NULL) {
    return FALSE;
  }

  Index        = 0;
  PropertyNode = AsciiProperty;
  MatchObj     = JsonObj;

  //
  // Walk through property and find corresponding object in JSON input
  //
  while (AsciiProperty[Index] != '\0') {
    if (AsciiProperty[Index] == '/') {
      AsciiProperty[Index] = '\0';
      MatchObj             = MatchJsonObject (MatchObj, PropertyNode);
      if (MatchObj == NULL) {
        PropertyNode = NULL;
        break;
      }

      PropertyNode = &AsciiProperty[Index + 1];
    }

    Index++;
  }

  if (PropertyNode != NULL) {
    MatchObj = MatchJsonObject (MatchObj, PropertyNode);
  }

  //
  // Value check
  //
  if (MatchObj != NULL) {
    //
    // If object has empty value, treat it as not matching
    //
    JsonType = JsonGetType (MatchObj);
    switch (JsonType) {
      case EdkiiJsonTypeObject:
        if (JsonValueIsNull (MatchObj)) {
          MatchObj = NULL;
        }

        break;
      case EdkiiJsonTypeArray:
        if (JsonArrayCount (MatchObj) == 0) {
          MatchObj = NULL;
        }

        break;
      case EdkiiJsonTypeNull:
        MatchObj = NULL;
        break;
      default:
        break;
    }
  }

  FreePool (AsciiProperty);

  return (MatchObj == NULL ? FALSE : TRUE);
}

/**

  Create string array and append to array node in Redfish JSON convert format.

  @param[in,out]  Head          The head of string array.
  @param[in]      StringArray   Input string array.
  @param[in]      ArraySize     The size of StringArray.

  @retval     EFI_SUCCESS       String array is created successfully.
  @retval     Others            Error happens

**/
EFI_STATUS
AddRedfishCharArray (
  IN OUT  RedfishCS_char_Array  **Head,
  IN      CHAR8                 **StringArray,
  IN      UINTN                 ArraySize
  )
{
  UINTN                 Index;
  RedfishCS_char_Array  *CharArrayBuffer;
  RedfishCS_char_Array  *PreArrayBuffer;

  if ((Head == NULL) || (StringArray == NULL) || (ArraySize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  PreArrayBuffer = NULL;
  for (Index = 0; Index < ArraySize; Index++) {
    CharArrayBuffer = AllocatePool (sizeof (RedfishCS_char_Array));
    if (CharArrayBuffer == NULL) {
      ASSERT (CharArrayBuffer != NULL);
      continue;
    }

    if (Index == 0) {
      *Head = CharArrayBuffer;
    }

    CharArrayBuffer->ArrayValue = StringArray[Index];
    CharArrayBuffer->Next       = NULL;
    if (PreArrayBuffer != NULL) {
      PreArrayBuffer->Next = CharArrayBuffer;
    }

    PreArrayBuffer = CharArrayBuffer;
  }

  return EFI_SUCCESS;
}

/**

  Create numeric array and append to array node in Redfish JSON convert format.

  @param[in,out]  Head           The head of string array.
  @param[in]      NumericArray   Input numeric array.
  @param[in]      ArraySize      The size of NumericArray.

  @retval     EFI_SUCCESS       String array is created successfully.
  @retval     Others            Error happens

**/
EFI_STATUS
AddRedfishNumericArray (
  IN OUT  RedfishCS_int64_Array  **Head,
  IN      INT64                  *NumericArray,
  IN      UINTN                  ArraySize
  )
{
  UINTN                  Index;
  RedfishCS_int64_Array  *NumericArrayBuffer;
  RedfishCS_int64_Array  *PreArrayBuffer;

  if ((Head == NULL) || (NumericArray == NULL) || (ArraySize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  PreArrayBuffer = NULL;
  for (Index = 0; Index < ArraySize; Index++) {
    NumericArrayBuffer = AllocatePool (sizeof (RedfishCS_int64_Array));
    if (NumericArrayBuffer == NULL) {
      ASSERT (NumericArrayBuffer != NULL);
      continue;
    }

    if (Index == 0) {
      *Head = NumericArrayBuffer;
    }

    NumericArrayBuffer->ArrayValue =  AllocatePool (sizeof (RedfishCS_int64));
    if (NumericArrayBuffer->ArrayValue == NULL) {
      ASSERT (NumericArrayBuffer->ArrayValue != NULL);
      continue;
    }

    *NumericArrayBuffer->ArrayValue = NumericArray[Index];
    NumericArrayBuffer->Next        = NULL;
    if (PreArrayBuffer != NULL) {
      PreArrayBuffer->Next = NumericArrayBuffer;
    }

    PreArrayBuffer = NumericArrayBuffer;
  }

  return EFI_SUCCESS;
}

/**

  Create boolean array and append to array node in Redfish JSON convert format.

  @param[in,out]  Head           The head of string array.
  @param[in]      BooleanArray   Input boolean array.
  @param[in]      ArraySize      The size of BooleanArray.

  @retval     EFI_SUCCESS       String array is created successfully.
  @retval     Others            Error happens

**/
EFI_STATUS
AddRedfishBooleanArray (
  IN OUT  RedfishCS_bool_Array  **Head,
  IN      BOOLEAN               *BooleanArray,
  IN      UINTN                 ArraySize
  )
{
  UINTN                 Index;
  RedfishCS_bool_Array  *BooleanArrayBuffer;
  RedfishCS_bool_Array  *PreArrayBuffer;

  if ((Head == NULL) || (BooleanArray == NULL) || (ArraySize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  PreArrayBuffer = NULL;
  for (Index = 0; Index < ArraySize; Index++) {
    BooleanArrayBuffer = AllocatePool (sizeof (RedfishCS_bool_Array));
    if (BooleanArrayBuffer == NULL) {
      ASSERT (BooleanArrayBuffer != NULL);
      continue;
    }

    if (Index == 0) {
      *Head = BooleanArrayBuffer;
    }

    BooleanArrayBuffer->ArrayValue =  AllocatePool (sizeof (RedfishCS_bool));
    if (BooleanArrayBuffer->ArrayValue == NULL) {
      ASSERT (BooleanArrayBuffer->ArrayValue != NULL);
      continue;
    }

    *BooleanArrayBuffer->ArrayValue = BooleanArray[Index];
    BooleanArrayBuffer->Next        = NULL;
    if (PreArrayBuffer != NULL) {
      PreArrayBuffer->Next = BooleanArrayBuffer;
    }

    PreArrayBuffer = BooleanArrayBuffer;
  }

  return EFI_SUCCESS;
}

/**

  Check and see if value in Redfish string array are all the same as the one
  from HII configuration.

  @param[in]  Head          The head of string array.
  @param[in]  StringArray   Input string array.
  @param[in]  ArraySize     The size of StringArray.

  @retval     TRUE          All string in Redfish array are as same as string
                            in HII configuration array.
              FALSE         These two array are not identical.

**/
BOOLEAN
CompareRedfishStringArrayValues (
  IN RedfishCS_char_Array  *Head,
  IN CHAR8                 **StringArray,
  IN UINTN                 ArraySize
  )
{
  UINTN                 Index;
  RedfishCS_char_Array  *CharArrayBuffer;

  if ((Head == NULL) || (StringArray == NULL) || (ArraySize == 0)) {
    return FALSE;
  }

  CharArrayBuffer = Head;
  Index           = 0;
  while (CharArrayBuffer != NULL && Index < ArraySize) {
    if (AsciiStrCmp (StringArray[Index], CharArrayBuffer->ArrayValue) != 0) {
      break;
    }

    Index++;
    CharArrayBuffer = CharArrayBuffer->Next;
  }

  if ((CharArrayBuffer != NULL) || (Index < ArraySize)) {
    return FALSE;
  }

  return TRUE;
}

/**

  Check and see if value in Redfish numeric array are all the same as the one
  from HII configuration.

  @param[in]  Head          The head of Redfish CS numeric array.
  @param[in]  NumericArray  Input numeric array.
  @param[in]  ArraySize     The size of NumericArray.

  @retval     TRUE          All string in Redfish array are as same as integer
                            in HII configuration array.
              FALSE         These two array are not identical.

**/
BOOLEAN
CompareRedfishNumericArrayValues (
  IN RedfishCS_int64_Array  *Head,
  IN INT64                  *NumericArray,
  IN UINTN                  ArraySize
  )
{
  UINTN                  Index;
  RedfishCS_int64_Array  *NumericArrayBuffer;

  if ((Head == NULL) || (NumericArray == NULL) || (ArraySize == 0)) {
    return FALSE;
  }

  NumericArrayBuffer = Head;
  Index              = 0;
  while (NumericArrayBuffer != NULL && Index < ArraySize) {
    if (NumericArray[Index] != *NumericArrayBuffer->ArrayValue) {
      break;
    }

    Index++;
    NumericArrayBuffer = NumericArrayBuffer->Next;
  }

  if ((NumericArrayBuffer != NULL) || (Index < ArraySize)) {
    return FALSE;
  }

  return TRUE;
}

/**

  Check and see if value in Redfish boolean array are all the same as the one
  from HII configuration.

  @param[in]  Head          The head of Redfish CS boolean array.
  @param[in]  BooleanArray  Input boolean array.
  @param[in]  ArraySize     The size of BooleanArray.

  @retval     TRUE          All string in Redfish array are as same as integer
                            in HII configuration array.
              FALSE         These two array are not identical.

**/
BOOLEAN
CompareRedfishBooleanArrayValues (
  IN RedfishCS_bool_Array  *Head,
  IN BOOLEAN               *BooleanArray,
  IN UINTN                 ArraySize
  )
{
  UINTN                 Index;
  RedfishCS_bool_Array  *BooleanArrayBuffer;

  if ((Head == NULL) || (BooleanArray == NULL) || (ArraySize == 0)) {
    return FALSE;
  }

  BooleanArrayBuffer = Head;
  Index              = 0;
  while (BooleanArrayBuffer != NULL && Index < ArraySize) {
    if (BooleanArray[Index] != *BooleanArrayBuffer->ArrayValue) {
      break;
    }

    Index++;
    BooleanArrayBuffer = BooleanArrayBuffer->Next;
  }

  if ((BooleanArrayBuffer != NULL) || (Index < ArraySize)) {
    return FALSE;
  }

  return TRUE;
}

/**

  Check and see if "@Redfish.Settings" exist in given Payload. If found, return the
  payload and URI to pending settings. Caller has to release "SettingPayload" and
  "SettingUri".

  @param[in]  Payload         Payload that may contain "@Redfish.Settings"
  @param[out] SettingPayload  Payload keeps pending settings. This is optional.
  @param[out] SettingUri      URI to pending settings.

  @retval     EFI_SUCCESS     Pending settings is found and returned.
  @retval     Others          Error happens

**/
EFI_STATUS
GetPendingSettings (
  IN  REDFISH_SERVICE RedfishService,
  IN  REDFISH_PAYLOAD Payload,
  OUT REDFISH_RESPONSE *SettingResponse, OPTIONAL
  OUT EFI_STRING        *SettingUri
  )
{
  CONST CHAR8       *RedfishSettingsUriKeys[] = { "@Redfish.Settings", "SettingsObject", "@odata.id" };
  EDKII_JSON_VALUE  JsonValue;
  UINTN             Index;
  EFI_STATUS        Status;

  if ((RedfishService == NULL) || (Payload == NULL) || (SettingUri == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *SettingUri = NULL;
  JsonValue   = RedfishJsonInPayload (Payload);

  //
  // Seeking RedfishSettings URI link.
  //
  for (Index = 0; Index < ARRAY_SIZE (RedfishSettingsUriKeys); Index++) {
    if (JsonValue == NULL) {
      break;
    }

    JsonValue = JsonObjectGetValue (JsonValueGetObject (JsonValue), RedfishSettingsUriKeys[Index]);
  }

  if (JsonValue != NULL) {
    //
    // Verify RedfishSettings URI link is valid to retrieve resource or not.
    //
    *SettingUri = JsonValueGetUnicodeString (JsonValue);
    if (*SettingUri == NULL) {
      return EFI_NOT_FOUND;
    }

    if (SettingResponse != NULL) {
      Status = GetResourceByUri (RedfishService, *SettingUri, SettingResponse);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: @Redfish.Settings exists, get resource from: %s failed: %r\n", __FUNCTION__, *SettingUri, Status));
        return Status;
      }
    }

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**

  Check and see if any difference between two vague value set.
  This is just a simple check.

  @param[in]  RedfishVagueKeyValuePtr     The vague key value sets on Redfish service.
  @param[in]  RedfishVagueKeyValueNumber  The number of vague key value sets
  @param[in]  ConfigVagueKeyValuePtr      The vague configuration on platform.
  @param[in]  ConfigVagueKeyValueNumber   The number of vague key value sets

  @retval     TRUE          All values are the same.
              FALSE         There is some difference.

**/
BOOLEAN
CompareRedfishPropertyVagueValues (
  IN RedfishCS_EmptyProp_KeyValue  *RedfishVagueKeyValuePtr,
  IN UINT32                        RedfishVagueKeyValueNumber,
  IN RedfishCS_EmptyProp_KeyValue  *ConfigVagueKeyValuePtr,
  IN UINT32                        ConfigVagueKeyValueNumber
  )
{
  RedfishCS_EmptyProp_KeyValue  *ThisConfigVagueKeyValuePtr;
  RedfishCS_EmptyProp_KeyValue  *ThisRedfishVagueKeyValuePtr;

  if (RedfishVagueKeyValueNumber != ConfigVagueKeyValueNumber) {
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: size is different: %d vs %d\n", __FUNCTION__, RedfishVagueKeyValueNumber, ConfigVagueKeyValueNumber));
    return FALSE;
  }

  ThisConfigVagueKeyValuePtr = ConfigVagueKeyValuePtr;
  //
  // Loop through all key/value on system.
  //
  while (ThisConfigVagueKeyValuePtr != NULL) {
    ThisRedfishVagueKeyValuePtr = RedfishVagueKeyValuePtr;
    //
    // Loop through all key/value on Redfish service..
    //
    while (ThisRedfishVagueKeyValuePtr != NULL) {
      if (AsciiStrCmp (ThisConfigVagueKeyValuePtr->KeyNamePtr, ThisRedfishVagueKeyValuePtr->KeyNamePtr) == 0) {
        //
        // Check the type of value.
        //
        if (ThisConfigVagueKeyValuePtr->Value->DataType != ThisRedfishVagueKeyValuePtr->Value->DataType) {
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a data type is different\n", __FUNCTION__, ThisConfigVagueKeyValuePtr->KeyNamePtr));
          return FALSE;
        }

        //
        // Check the value.
        //
        if (ThisConfigVagueKeyValuePtr->Value->DataType == RedfishCS_Vague_DataType_String) {
          //
          // Is the string identical?
          //
          if (AsciiStrCmp (
                ThisConfigVagueKeyValuePtr->Value->DataValue.CharPtr,
                ThisRedfishVagueKeyValuePtr->Value->DataValue.CharPtr
                ) == 0)
          {
            break;
          } else {
            DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a is updated\n", __FUNCTION__, ThisConfigVagueKeyValuePtr->KeyNamePtr));
            return FALSE;
          }
        } else if (ThisConfigVagueKeyValuePtr->Value->DataType == RedfishCS_Vague_DataType_Int64) {
          if (*ThisConfigVagueKeyValuePtr->Value->DataValue.Int64Ptr == *ThisRedfishVagueKeyValuePtr->Value->DataValue.Int64Ptr) {
            break;
          } else {
            DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a is updated\n", __FUNCTION__, ThisConfigVagueKeyValuePtr->KeyNamePtr));
            return FALSE;
          }
        } else if (ThisConfigVagueKeyValuePtr->Value->DataType == RedfishCS_Vague_DataType_Bool) {
          if ((UINT8)*ThisConfigVagueKeyValuePtr->Value->DataValue.BoolPtr == (UINT8)*ThisRedfishVagueKeyValuePtr->Value->DataValue.BoolPtr) {
            break;
          } else {
            DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a is updated\n", __FUNCTION__, ThisConfigVagueKeyValuePtr->KeyNamePtr));
            return FALSE;
          }
        } else {
          DEBUG ((REDFISH_DEBUG_TRACE, "%a: %a unsupported type: 0x%x\n", __FUNCTION__, ThisConfigVagueKeyValuePtr->KeyNamePtr, ThisConfigVagueKeyValuePtr->Value->DataType));
          return FALSE;
        }
      }

      ThisRedfishVagueKeyValuePtr = ThisRedfishVagueKeyValuePtr->NextKeyValuePtr;
    }

    if (ThisRedfishVagueKeyValuePtr == NULL) {
      //
      // No matched key name. Threat these two vague value set is different.
      //
      return FALSE;
    }

    ThisConfigVagueKeyValuePtr = ThisConfigVagueKeyValuePtr->NextKeyValuePtr;
  }

  return TRUE;
}

/**

  Install Boot Maintenance Manager Menu driver.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCCESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishFeatureUtilityLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}

/**
  Unloads the application and its installed protocol.

  @param[in] ImageHandle       Handle that identifies the image to be unloaded.
  @param[in] SystemTable      The system table.

  @retval EFI_SUCCESS      The image has been unloaded.

**/
EFI_STATUS
EFIAPI
RedfishFeatureUtilityLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EFI_SUCCESS;
}
