/** @file
  Redfish feature driver implementation - common functions

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "BootOptionCommon.h"

CHAR8  BootOptionEmptyJson[] = "{\"@odata.id\": \"\", \"@odata.type\": \"#BootOption.v1_0_4.BootOption\", \"Id\": \"\", \"Name\": \"\", \"BootOptionEnabled\": false, \"BootOptionReference\": \"\", \"Description\":\"\", \"DisplayName\": \"\", \"UefiDevicePath\":\"\"}";

REDFISH_RESOURCE_COMMON_PRIVATE  *mRedfishResourcePrivate = NULL;

/**
  Get boot option variable name from BootOptionReference attribute in Boot Option resource.
  It's caller's responsibility to release BootOptionName by calling FreePool().

  @param[in]   BootOptionCs   Pointer to boot option structure.
  @param[in]   BootOptionName Boot option name read from boot option resource.

  @retval EFI_SUCCESS         Boot option name is read successfully.
  @retval Others              Some error happened.

**/
EFI_STATUS
GetBootOptionNameFromRedfish (
  IN EFI_REDFISH_BOOTOPTION_V1_0_4_CS  *BootOptionCs,
  OUT EFI_STRING                       *BootOptionName
  )
{
  if ((BootOptionCs == NULL) || (BootOptionName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *BootOptionName = NULL;

  if (BootOptionCs->BootOptionReference == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Missing BootOptionReference attribute which is mandatory attribute, ignore this!\n", __func__));
    return EFI_PROTOCOL_ERROR;
  }

  *BootOptionName = StrAsciiToUnicode (BootOptionCs->BootOptionReference);
  if (*BootOptionName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Consume resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.
  @param[in]   HeaderEtag          The Etag string returned in HTTP header.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishConsumeResourceCommon (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN  CHAR8                            *Json,
  IN  CHAR8                            *HeaderEtag OPTIONAL
  )
{
  EFI_STATUS                        Status;
  EFI_REDFISH_BOOTOPTION_V1_0_4     *BootOption;
  EFI_REDFISH_BOOTOPTION_V1_0_4_CS  *BootOptionCs;
  EFI_BOOT_MANAGER_LOAD_OPTION      LoadOption;
  BOOLEAN                           LoadOptionEnabled;
  EFI_STRING                        BootOptionName;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  BootOption     = NULL;
  BootOptionCs   = NULL;
  BootOptionName = NULL;

  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Json,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&BootOption
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __func__, Status));
    return Status;
  }

  BootOptionCs = BootOption->BootOption;

  //
  // Check ETAG to see if we need to consume it
  //
  if (CheckEtag (Private->Uri, HeaderEtag, NULL)) {
    //
    // No change
    //
    DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: ETAG: %s has no change, ignore consume action\n", __func__, Private->Uri));
    Status = EFI_SUCCESS;
    goto ON_RELEASE;
  }

  //
  // Get boot option variable name from Redfish.
  //
  Status = GetBootOptionNameFromRedfish (BootOptionCs, &BootOptionName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not get boot option name: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  //
  // Find corresponding Boot#### boot option.
  //
  Status = EfiBootManagerVariableToLoadOption (BootOptionName, &LoadOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not load boot option: %s\n", __func__, BootOptionName));
    goto ON_RELEASE;
  }

  //
  // BootOptionEnabled is the only attribute that is not read-only.
  //
  if (BootOptionCs->BootOptionEnabled != NULL) {
    LoadOptionEnabled = ((LoadOption.Attributes & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE);

    if (*BootOptionCs->BootOptionEnabled != LoadOptionEnabled) {
      if (*BootOptionCs->BootOptionEnabled) {
        LoadOption.Attributes |= LOAD_OPTION_ACTIVE;
      } else {
        LoadOption.Attributes &= ~LOAD_OPTION_ACTIVE;
      }

      //
      // Save to variable.
      //
      Status = EfiBootManagerLoadOptionToVariable (&LoadOption);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: can not %a boot option: %s: %r", __func__, (*BootOptionCs->BootOptionEnabled ? "enable" : "disable"), BootOptionName, Status));
      }
    }
  }

  EfiBootManagerFreeLoadOption (&LoadOption);

ON_RELEASE:

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (
                                 Private->JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)BootOption
                                 );

  if (BootOptionName != NULL) {
    FreePool (BootOptionName);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningBootOptionProperties (
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL  *JsonStructProtocol,
  IN  CHAR8                             *InputJson,
  IN  CHAR8                             *ResourceId OPTIONAL,
  IN  EFI_STRING                        ConfigureLang,
  IN  BOOLEAN                           ProvisionMode,
  OUT CHAR8                             **ResultJson
  )
{
  EFI_REDFISH_BOOTOPTION_V1_0_4     *BootOption;
  EFI_REDFISH_BOOTOPTION_V1_0_4_CS  *BootOptionCs;
  EFI_STATUS                        Status;
  BOOLEAN                           PropertyChanged;
  EFI_BOOT_MANAGER_LOAD_OPTION      LoadOption;
  EFI_STRING                        DevicePathString;
  BOOLEAN                           LoadOptionEnabled;
  CHAR8                             *AsciiStringValue;

  if ((JsonStructProtocol == NULL) || (ResultJson == NULL) || IS_EMPTY_STRING (InputJson) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: provision for %s with: %s\n", __func__, ConfigureLang, (ProvisionMode ? L"Provision resource" : L"Update resource")));

  *ResultJson      = NULL;
  PropertyChanged  = FALSE;
  DevicePathString = NULL;
  AsciiStringValue = NULL;
  BootOption       = NULL;

  Status = JsonStructProtocol->ToStructure (
                                 JsonStructProtocol,
                                 NULL,
                                 InputJson,
                                 (EFI_REST_JSON_STRUCTURE_HEADER **)&BootOption
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure failure: %r\n", __func__, Status));
    return Status;
  }

  BootOptionCs = BootOption->BootOption;

  //
  // Find corresponding Boot#### boot option.
  //
  Status = EfiBootManagerVariableToLoadOption (ConfigureLang, &LoadOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not load boot option: %s\n", __func__, ConfigureLang));
    goto ON_RELEASE;
  }

  //
  // ID
  //
  if (BootOptionCs->Id != NULL) {
    BootOptionCs->Id = NULL;
  }

  //
  // Name
  //
  if (BootOptionCs->Name != NULL) {
    BootOptionCs->Name = NULL;
  }

  //
  // BootOptionEnabled
  //
  if (PropertyChecker (BootOptionCs->BootOptionEnabled, ProvisionMode)) {
    LoadOptionEnabled = ((LoadOption.Attributes & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE);
    if (*BootOptionCs->BootOptionEnabled != LoadOptionEnabled) {
      *BootOptionCs->BootOptionEnabled = LoadOptionEnabled;
      PropertyChanged                  = TRUE;
    }
  }

  //
  // BootOptionReference
  //
  if (PropertyChecker (BootOptionCs->BootOptionReference, ProvisionMode)) {
    AsciiStringValue = StrUnicodeToAscii (ConfigureLang);
    if (AsciiStringValue != NULL) {
      if ((BootOptionCs->BootOptionReference == NULL) || (AsciiStrCmp (AsciiStringValue, BootOptionCs->BootOptionReference) != 0)) {
        BootOptionCs->BootOptionReference = AsciiStringValue;
        PropertyChanged                   = TRUE;
      } else {
        FreePool (AsciiStringValue);
        AsciiStringValue                  = NULL;
        BootOptionCs->BootOptionReference = NULL;
      }
    }
  }

  //
  // Description
  //
  if (PropertyChecker (BootOptionCs->Description, ProvisionMode)) {
    AsciiStringValue = StrUnicodeToAscii (LoadOption.Description);
    if (AsciiStringValue != NULL) {
      if ((BootOptionCs->Description == NULL) || (AsciiStrCmp (AsciiStringValue, BootOptionCs->Description) != 0)) {
        BootOptionCs->Description = AsciiStringValue;
        PropertyChanged           = TRUE;
      } else {
        FreePool (AsciiStringValue);
        AsciiStringValue          = NULL;
        BootOptionCs->Description = NULL;
      }
    }
  }

  //
  // DisplayName
  //
  if (PropertyChecker (BootOptionCs->DisplayName, ProvisionMode)) {
    AsciiStringValue = StrUnicodeToAscii (LoadOption.Description);
    if (AsciiStringValue != NULL) {
      if ((BootOptionCs->DisplayName == NULL) || (AsciiStrCmp (AsciiStringValue, BootOptionCs->DisplayName) != 0)) {
        BootOptionCs->DisplayName = AsciiStringValue;
        PropertyChanged           = TRUE;
      } else {
        FreePool (AsciiStringValue);
        AsciiStringValue          = NULL;
        BootOptionCs->DisplayName = NULL;
      }
    }
  }

  //
  // UefiDevicePath
  //
  if (PropertyChecker (BootOptionCs->UefiDevicePath, ProvisionMode)) {
    DevicePathString = ConvertDevicePathToText (LoadOption.FilePath, TRUE, FALSE);
    if (DevicePathString != NULL) {
      AsciiStringValue = StrUnicodeToAscii (DevicePathString);
      if (AsciiStringValue != NULL) {
        if ((BootOptionCs->UefiDevicePath == NULL) || (AsciiStrCmp (AsciiStringValue, BootOptionCs->UefiDevicePath) != 0)) {
          BootOptionCs->UefiDevicePath = AsciiStringValue;
          PropertyChanged              = TRUE;
        } else {
          FreePool (AsciiStringValue);
          AsciiStringValue             = NULL;
          BootOptionCs->UefiDevicePath = NULL;
        }
      }

      FreePool (DevicePathString);
    }
  }

  EfiBootManagerFreeLoadOption (&LoadOption);

ON_RELEASE:

  //
  // Convert C structure back to JSON text.
  //
  Status = JsonStructProtocol->ToJson (
                                 JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)BootOption,
                                 ResultJson
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToJson() failed: %r\n", __func__, Status));
    return Status;
  }

  //
  // Release resource.
  //
  JsonStructProtocol->DestoryStructure (
                        JsonStructProtocol,
                        (EFI_REST_JSON_STRUCTURE_HEADER *)BootOption
                        );

  return (PropertyChanged ? EFI_SUCCESS : EFI_NOT_FOUND);
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
  CHAR8             *Json;
  CHAR8             *JsonWithAddendum;
  EFI_STATUS        Status;
  EFI_STRING        NewResourceLocation;
  EFI_STRING        BootOptionName;
  REDFISH_RESPONSE  Response;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Json                = NULL;
  JsonWithAddendum    = NULL;
  NewResourceLocation = NULL;
  BootOptionName      = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));

  if (ResourceExist) {
    DEBUG ((DEBUG_ERROR, "%a: dose not support the exist boot option resource\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Get boot option name from URI. The name is in the query parameter: "?name="
  //
  BootOptionName = StrStr (Private->Uri, REDFISH_BOOT_OPTION_PARAMETER);
  if (BootOptionName == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: can not get boot option name in: %s\n", __func__, Private->Uri));
    return EFI_DEVICE_ERROR;
  }

  BootOptionName[0] = '\0';

  BootOptionName += StrLen (REDFISH_BOOT_OPTION_PARAMETER);
  if (BootOptionName[0] == '\0') {
    DEBUG ((DEBUG_ERROR, "%a: empty boot option name in: %s\n", __func__, Private->Uri));
    return EFI_DEVICE_ERROR;
  }

  Status = ProvisioningBootOptionProperties (
             Private->JsonStructProtocol,
             BootOptionEmptyJson,
             NULL,
             BootOptionName,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: provisioning resource for %s failed: %r\n", __func__, BootOptionName, Status));
    return Status;
  }

  //
  // Check and see if platform has OEM data or not
  //
  Status = RedfishGetOemData (
             Private->Uri,
             RESOURCE_SCHEMA,
             RESOURCE_SCHEMA_VERSION,
             Json,
             &JsonWithAddendum
             );
  if (!EFI_ERROR (Status) && (JsonWithAddendum != NULL)) {
    FreePool (Json);
    Json             = JsonWithAddendum;
    JsonWithAddendum = NULL;
  }

  //
  // Check and see if platform has addendum data or not
  //
  Status = RedfishGetAddendumData (
             Private->Uri,
             RESOURCE_SCHEMA,
             RESOURCE_SCHEMA_VERSION,
             Json,
             &JsonWithAddendum
             );
  if (!EFI_ERROR (Status) && (JsonWithAddendum != NULL)) {
    FreePool (Json);
    Json             = JsonWithAddendum;
    JsonWithAddendum = NULL;
  }

  Status = RedfishHttpPostResource (Private->RedfishService, Private->Uri, Json, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: post BootOption resource for %s failed: %r\n", __func__, BootOptionName, Status));
    goto RELEASE_RESOURCE;
  }

  //
  // per Redfish spec. the URL of new resource will be returned in "Location" header.
  //
  Status = GetEtagAndLocation (&Response, NULL, &NewResourceLocation);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: cannot find new location: %r\n", __FUNCTION__, Status));
    goto RELEASE_RESOURCE;
  }

  //
  // Keep location of new resource.
  //
  if (NewResourceLocation != NULL) {
    DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: Location: %s\n", __func__, NewResourceLocation));
    RedfishSetRedfishUri (BootOptionName, NewResourceLocation);
  }

RELEASE_RESOURCE:

  if (NewResourceLocation != NULL) {
    FreePool (NewResourceLocation);
  }

  if (Json != NULL) {
    FreePool (Json);
  }

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  Check resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.
  @param[in]   HeaderEtag          The Etag string returned in HTTP header.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishCheckResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json,
  IN     CHAR8                            *HeaderEtag OPTIONAL
  )
{
  EFI_STATUS                        Status;
  EFI_REDFISH_BOOTOPTION_V1_0_4     *BootOption;
  EFI_REDFISH_BOOTOPTION_V1_0_4_CS  *BootOptionCs;
  EFI_STRING                        BootOptionName;
  EFI_BOOT_MANAGER_LOAD_OPTION      LoadOption;
  REDFISH_RESPONSE                  Response;
  BOOLEAN                           DeleteResourceRequired;
  EFI_STRING                        DevicePathString;
  CHAR8                             *DevicePathAsciiString;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));
  DevicePathString       = NULL;
  DevicePathAsciiString  = NULL;
  DeleteResourceRequired = FALSE;
  BootOptionName         = NULL;
  BootOption             = NULL;
  Response.Payload       = NULL;
  Status                 = Private->JsonStructProtocol->ToStructure (
                                                          Private->JsonStructProtocol,
                                                          NULL,
                                                          Json,
                                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&BootOption
                                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __func__, Status));
    return Status;
  }

  BootOptionCs = BootOption->BootOption;

  //
  // Get boot option variable name from Redfish.
  //
  Status = GetBootOptionNameFromRedfish (BootOptionCs, &BootOptionName);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not get boot option name: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  //
  // Find corresponding Boot#### boot option.
  //
  Status = EfiBootManagerVariableToLoadOption (BootOptionName, &LoadOption);
  if (!EFI_ERROR (Status)) {
    //
    // Check the UefiDevicePath
    //
    if (BootOptionCs->UefiDevicePath != NULL) {
      DevicePathString = ConvertDevicePathToText (LoadOption.FilePath, TRUE, FALSE);
      if (DevicePathString != NULL) {
        DevicePathAsciiString = StrUnicodeToAscii (DevicePathString);
        if (DevicePathAsciiString != NULL) {
          if (AsciiStrCmp (DevicePathAsciiString, BootOptionCs->UefiDevicePath) != 0) {
            //
            // The device path of this boot option is not the one in system.
            //
            DeleteResourceRequired = TRUE;
          }

          FreePool (DevicePathAsciiString);
        }

        FreePool (DevicePathString);
      } else {
        DeleteResourceRequired = TRUE;
      }
    }

    EfiBootManagerFreeLoadOption (&LoadOption);
  } else {
    DeleteResourceRequired = TRUE;
  }

  //
  // This boot option is deleted in system. Remove it from BMC too.
  //
  if (DeleteResourceRequired) {
    DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: boot option %s is deleted in system. Delete %s\n", __func__, BootOptionName, Private->Uri));
    Status = RedfishHttpDeleteResource (Private->RedfishService, Private->Uri, &Response);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: can not delete %s: %r\n", __func__, Private->Uri, Status));
    }

    //
    // We delete this resource. So this is not the resource that we supported.
    // Return EFI_UNSUPPORTED and caller will ignore this resource.
    //
    Status = EFI_UNSUPPORTED;
  } else {
    //
    // This is the boot option in system and we need further processing to it.
    //
    Status = EFI_SUCCESS;

    //
    // If configure language is missing due to default settings,
    // create the record because this is the boot option that we
    // handle.
    //
    if (RedfishGetConfigLanguage (Private->Uri) == NULL) {
      RedfishSetRedfishUri (BootOptionName, Private->Uri);
    }
  }

ON_RELEASE:

  //
  // Release resource
  //
  RedfishHttpFreeResource (&Response);

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (
                                 Private->JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)BootOption
                                 );

  if (BootOptionName != NULL) {
    FreePool (BootOptionName);
  }

  return Status;
}

/**
  Update resource to given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishUpdateResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *InputJson
  )
{
  EFI_STATUS        Status;
  CHAR8             *Json;
  CHAR8             *JsonWithAddendum;
  EFI_STRING        ConfigureLang;
  REDFISH_RESPONSE  Response;

  if ((Private == NULL) || IS_EMPTY_STRING (InputJson)) {
    return EFI_INVALID_PARAMETER;
  }

  Json          = NULL;
  ConfigureLang = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningBootOptionProperties (
             Private->JsonStructProtocol,
             InputJson,
             NULL,
             ConfigureLang,
             FALSE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: update resource for %s ignored. Nothing changed\n", __func__, ConfigureLang));
      Status = EFI_SUCCESS;
    } else {
      DEBUG ((DEBUG_ERROR, "%a: update resource for %s failed: %r\n", __func__, ConfigureLang, Status));
    }

    goto ON_RELEASE;
  }

  //
  // Check and see if platform has OEM data or not
  //
  Status = RedfishGetOemData (
             Private->Uri,
             RESOURCE_SCHEMA,
             RESOURCE_SCHEMA_VERSION,
             Json,
             &JsonWithAddendum
             );
  if (!EFI_ERROR (Status) && (JsonWithAddendum != NULL)) {
    FreePool (Json);
    Json             = JsonWithAddendum;
    JsonWithAddendum = NULL;
  }

  //
  // Check and see if platform has addendum data or not
  //
  Status = RedfishGetAddendumData (
             Private->Uri,
             RESOURCE_SCHEMA,
             RESOURCE_SCHEMA_VERSION,
             Json,
             &JsonWithAddendum
             );
  if (!EFI_ERROR (Status) && (JsonWithAddendum != NULL)) {
    FreePool (Json);
    Json             = JsonWithAddendum;
    JsonWithAddendum = NULL;
  }

  DEBUG ((REDFISH_BOOT_OPTION_DEBUG_TRACE, "%a: update resource for %s\n", __func__, ConfigureLang));

  //
  // PATCH back to instance
  //
  Status = RedfishHttpPatchResource (Private->RedfishService, Private->Uri, Json, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: patch resource for %s failed: %r\n", __func__, ConfigureLang, Status));
  }

ON_RELEASE:

  if (Json != NULL) {
    FreePool (Json);
  }

  if (ConfigureLang != NULL) {
    FreePool (ConfigureLang);
  }

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  Identify resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishIdentifyResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json
  )
{
  BOOLEAN  Supported;

  Supported = RedfishIdentifyResource (Private->Uri, Private->Json);
  if (Supported) {
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}
