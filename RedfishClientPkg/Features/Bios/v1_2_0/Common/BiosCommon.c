/** @file
  Redfish feature driver implementation - common functions

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "BiosCommon.h"

CHAR8  BiosEmptyJson[] = "{\"@odata.id\": \"\", \"@odata.type\": \"#Bios.v1_2_0.Bios\", \"Id\": \"\", \"Name\": \"\", \"Attributes\":{}}";

REDFISH_RESOURCE_COMMON_PRIVATE  *mRedfishResourcePrivate             = NULL;
EFI_HANDLE                       mRedfishResourceConfigProtocolHandle = NULL;

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
  EFI_REDFISH_BIOS_V1_2_0           *Bios;
  EFI_REDFISH_BIOS_V1_2_0_CS        *BiosCs;
  EFI_STRING                        ConfigureLang;
  RedfishCS_Type_EmptyProp_CS_Data  *EmptyPropCs;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  Bios          = NULL;
  BiosCs        = NULL;
  ConfigureLang = NULL;

  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Json,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&Bios
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  BiosCs = Bios->Bios;

  //
  // Check ETAG to see if we need to consume it
  //
  if (CheckEtag (Private->Uri, HeaderEtag, BiosCs->odata_etag)) {
    //
    // No change
    //
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: ETAG: %s has no change, ignore consume action\n", __FUNCTION__, Private->Uri));
    Status = EFI_SUCCESS;
    goto ON_RELEASE;
  }

  //
  // Handle ATTRIBUTEREGISTRY
  //
  if (BiosCs->AttributeRegistry != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (BiosCs->odata_id, "AttributeRegistry");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, BiosCs->AttributeRegistry);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ATTRIBUTES->EmptyProperty
  //
  if (BiosCs->Attributes != NULL) {
    //
    // Validate empty property.
    //
    if (BiosCs->Attributes->Prop.ForwardLink == &BiosCs->Attributes->Prop) {
      goto ON_RELEASE;
    }

    EmptyPropCs = (RedfishCS_Type_EmptyProp_CS_Data *)BiosCs->Attributes->Prop.ForwardLink;
    if (EmptyPropCs->Header.ResourceType == RedfishCS_Type_JSON) {
      DEBUG ((DEBUG_ERROR, "%a: Empty property with RedfishCS_Type_JSON type resource is not supported yet. (%s)\n", __FUNCTION__, Private->Uri));
      goto ON_RELEASE;
    }

    if (EmptyPropCs->NunmOfProperties > 0) {
      //
      // Find corresponding configure language for collection resource.
      //
      ConfigureLang = GetConfigureLang (BiosCs->odata_id, "Attributes");
      if (ConfigureLang != NULL) {
        Status = ApplyFeatureSettingsVagueType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, EmptyPropCs->KeyValuePtr, EmptyPropCs->NunmOfProperties);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
        }

        FreePool (ConfigureLang);
      } else {
        DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
      }
    }
  }

ON_RELEASE:

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (
                                 Private->JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)Bios
                                 );

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningBiosProperties (
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL *JsonStructProtocol,
  IN  CHAR8 *InputJson,
  IN  CHAR8 *ResourceId, OPTIONAL
  IN  EFI_STRING                        ConfigureLang,
  IN  BOOLEAN                           ProvisionMode,
  OUT CHAR8                             **ResultJson
  )
{
  EFI_REDFISH_BIOS_V1_2_0       *Bios;
  EFI_REDFISH_BIOS_V1_2_0_CS    *BiosCs;
  EFI_STATUS                    Status;
  BOOLEAN                       PropertyChanged;
  CHAR8                         *AsciiStringValue;
  RedfishCS_EmptyProp_KeyValue  *PropertyVagueValues;
  UINT32                        VagueValueNumber;

  if ((JsonStructProtocol == NULL) || (ResultJson == NULL) || IS_EMPTY_STRING (InputJson) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a provision for %s with: %s\n", __FUNCTION__, ConfigureLang, (ProvisionMode ? L"Provision resource" : L"Update resource")));

  *ResultJson     = NULL;
  PropertyChanged = FALSE;

  Bios   = NULL;
  Status = JsonStructProtocol->ToStructure (
                                 JsonStructProtocol,
                                 NULL,
                                 InputJson,
                                 (EFI_REST_JSON_STRUCTURE_HEADER **)&Bios
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure failure: %r\n", __FUNCTION__, Status));
    return Status;
  }

  BiosCs = Bios->Bios;

  //
  // Starting from BIOS V1_2_0, when Bios.RestBios is called, ResetBiosToDefaultsPending
  // is set to TRUE.
  //
  if (!ProvisionMode && (BiosCs->ResetBiosToDefaultsPending != NULL) && (*(BiosCs->ResetBiosToDefaultsPending) == TRUE)) {
    ProvisionMode = TRUE;
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: ResetBiosToDefaultsPending is set. Force to provisioning mode\n"));
  }

  //
  // ID
  //
  if ((BiosCs->Id == NULL) && !IS_EMPTY_STRING (ResourceId)) {
    BiosCs->Id = AllocateCopyPool (AsciiStrSize (ResourceId), ResourceId);
  }

  //
  // Handle ATTRIBUTEREGISTRY
  //
  if (PropertyChecker (BiosCs->AttributeRegistry, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"AttributeRegistry", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (BiosCs->AttributeRegistry, AsciiStringValue) != 0)) {
        BiosCs->AttributeRegistry = AsciiStringValue;
        PropertyChanged           = TRUE;
      }
    }
  }

  //
  // Handle ATTRIBUTES
  //
  if (PropertyChecker (BiosCs->Attributes, ProvisionMode)) {
    PropertyVagueValues = GetPropertyVagueValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Attributes", ConfigureLang, &VagueValueNumber);
    if (PropertyVagueValues != NULL) {
      if (ProvisionMode || !CompareRedfishPropertyVagueValues (
                              ((RedfishCS_Type_EmptyProp_CS_Data *)BiosCs->Attributes->Prop.ForwardLink)->KeyValuePtr,
                              ((RedfishCS_Type_EmptyProp_CS_Data *)BiosCs->Attributes->Prop.ForwardLink)->NunmOfProperties,
                              PropertyVagueValues,
                              VagueValueNumber
                              ))
      {
        DEBUG ((REDFISH_DEBUG_TRACE, "%a: PropertyVagueValues: %p VagueValueNumber: %d\n", __FUNCTION__, PropertyVagueValues, VagueValueNumber));
        //
        // Use the properties on system to replace the one on Redfish service.
        //
        ((RedfishCS_Type_EmptyProp_CS_Data *)BiosCs->Attributes->Prop.ForwardLink)->KeyValuePtr      = PropertyVagueValues;
        ((RedfishCS_Type_EmptyProp_CS_Data *)BiosCs->Attributes->Prop.ForwardLink)->NunmOfProperties = VagueValueNumber;
        PropertyChanged                                                                              = TRUE;
      }
    }
  }

  if (PropertyChanged) {
    //
    // Convert C structure back to JSON text.
    //
    Status = JsonStructProtocol->ToJson (
                                   JsonStructProtocol,
                                   (EFI_REST_JSON_STRUCTURE_HEADER *)Bios,
                                   ResultJson
                                   );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: ToJson() failed: %r\n", __FUNCTION__, Status));
      //
      // Release resource.
      //
      JsonStructProtocol->DestoryStructure (
                            JsonStructProtocol,
                            (EFI_REST_JSON_STRUCTURE_HEADER *)Bios
                            );
      return Status;
    }
  }

  //
  // Release resource.
  //
  JsonStructProtocol->DestoryStructure (
                        JsonStructProtocol,
                        (EFI_REST_JSON_STRUCTURE_HEADER *)Bios
                        );

  return (PropertyChanged ? EFI_SUCCESS : EFI_NOT_FOUND);
}

EFI_STATUS
ProvisioningBiosResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN  UINTN                            Index,
  IN  EFI_STRING                       ConfigureLang
  )
{
  CHAR8             *Json;
  CHAR8             *JsonWithAddendum;
  EFI_STATUS        Status;
  EFI_STRING        NewResourceLocation;
  CHAR8             ResourceId[16];
  REDFISH_RESPONSE  Response;

  if (IS_EMPTY_STRING (ConfigureLang) || (Private == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));
  NewResourceLocation = NULL;
  AsciiSPrint (ResourceId, sizeof (ResourceId), "%d", Index);

  Status = ProvisioningBiosProperties (
             Private->JsonStructProtocol,
             BiosEmptyJson,
             ResourceId,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: provisioning resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
    DEBUG ((DEBUG_ERROR, "%a: post Bios resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
    RedfishSetRedfishUri (ConfigureLang, NewResourceLocation);
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

EFI_STATUS
ProvisioningBiosResources (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private
  )
{
  UINTN                                        Index;
  EFI_STATUS                                   Status;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  UnifiedConfigureLangList;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishFeatureGetUnifiedArrayTypeConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, CONFIG_LANG_ARRAY_PATTERN, &UnifiedConfigureLangList);
  if (EFI_ERROR (Status) || (UnifiedConfigureLangList.Count == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: No HII question found with configure language: %s: %r\n", __FUNCTION__, CONFIG_LANG_ARRAY_PATTERN, Status));
    return EFI_NOT_FOUND;
  }

  //
  // Set the configuration language in the RESOURCE_INFORMATION_EXCHANGE.
  // This information is sent back to the parent resource (e.g. the collection driver).
  //
  EdkIIRedfishResourceSetConfigureLang (mRedfishResourceConfigProtocolHandle, &UnifiedConfigureLangList);

  for (Index = 0; Index < UnifiedConfigureLangList.Count; Index++) {
    DEBUG ((REDFISH_DEBUG_TRACE, "[%d] create Bios resource from: %s\n", UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang));
    ProvisioningBiosResource (Private, UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang);
    FreePool (UnifiedConfigureLangList.List[Index].ConfigureLang);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningBiosExistResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private
  )
{
  EFI_STATUS        Status;
  EFI_STRING        ConfigureLang;
  CHAR8             *Json;
  CHAR8             *JsonWithAddendum;
  REDFISH_RESPONSE  Response;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Json          = NULL;
  ConfigureLang = NULL;
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningBiosProperties (
             Private->JsonStructProtocol,
             BiosEmptyJson,
             NULL,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: provisioning existing resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
      Status = EFI_SUCCESS;
    } else {
      DEBUG ((DEBUG_ERROR, "%a: provisioning existing resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: provisioning existing resource for %s\n", __FUNCTION__, ConfigureLang));

  //
  // PUT back to instance
  //
  Status = RedfishHttpPutResource (Private->RedfishService, Private->Uri, Json, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: put resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return (ResourceExist ? ProvisioningBiosExistResource (Private) : ProvisioningBiosResources (Private));
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
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_STRING  *ConfigureLangList;
  UINTN       Count;
  EFI_STRING  Property;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check ETAG to see if we need to check it
  //
  if (CheckEtag (Private->Uri, HeaderEtag, NULL)) {
    //
    // No change
    //
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: ETAG: %s has no change, ignore check action\n", __FUNCTION__, Private->Uri));
    return EFI_SUCCESS;
  }

  Status = RedfishPlatformConfigGetConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, CONFIG_LANG_ARRAY_PATTERN, &ConfigureLangList, &Count);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: BiosConfigToRedfishGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (Count == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < Count; Index++) {
    Property = GetPropertyFromConfigureLang (Private->Uri, ConfigureLangList[Index]);
    if (Property == NULL) {
      continue;
    }

    DEBUG ((REDFISH_DEBUG_TRACE, "%a: [%d] check attribute for: %s\n", __FUNCTION__, Index, Property));
    if (!MatchPropertyWithJsonContext (Property, Json)) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: property is missing: %s\n", __FUNCTION__, Property));
    } else {
      //
      // When there is any attribute found in /Bios/Attributes, it means that
      // the provisioning was performed before. Any missing attribute will
      // be handled by update operation.
      // When all attributes are missing in /Bios/Attributes, provisioning is
      // performed.
      //
      Status = EFI_SUCCESS;
    }
  }

  FreePool (ConfigureLangList);

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

  Status = ProvisioningBiosProperties (
             Private->JsonStructProtocol,
             InputJson,
             NULL,
             ConfigureLang,
             FALSE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: update resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
      Status = EFI_SUCCESS;
    } else {
      DEBUG ((DEBUG_ERROR, "%a: update resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: update resource for %s\n", __FUNCTION__, ConfigureLang));
  //
  // PUT back to instance
  //
  Status = RedfishHttpPutResource (Private->RedfishService, Private->Uri, Json, &Response);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: put resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
  BOOLEAN                                      Supported;
  EFI_STATUS                                   Status;
  EFI_STRING                                   EndOfChar;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  ConfigLangList;

  Supported = RedfishIdentifyResource (Private->Uri, Private->Json);
  if (Supported) {
    Status = RedfishFeatureGetUnifiedArrayTypeConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, CONFIG_LANG_ARRAY_PATTERN, &ConfigLangList);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: BiosConfigToRedfishGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
      return Status;
    }

    if (ConfigLangList.Count == 0) {
      return EFI_SUCCESS;
    }

    // EndOfChar = StrStr (ConfigLangList.List[0].ConfigureLang, L"}");
    Status = IsRedpathArray (ConfigLangList.List[0].ConfigureLang, NULL, &EndOfChar);
    if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
      ASSERT (FALSE);
      return EFI_DEVICE_ERROR;
    }

    if (Status != EFI_SUCCESS) {
      //
      // This is not the collection config language.
      //
      GetRedpathNodeByIndex (ConfigLangList.List[0].ConfigureLang, 0, &EndOfChar);
    }

    *(++EndOfChar) = '\0';
    //
    // Keep URI and ConfigLang mapping
    //
    RedfishSetRedfishUri (ConfigLangList.List[0].ConfigureLang, Private->Uri);
    //
    // Set the configuration language in the RESOURCE_INFORMATION_EXCHANGE.
    // This information is sent back to the parent resource (e.g. the collection driver).
    //
    EdkIIRedfishResourceSetConfigureLang (mRedfishResourceConfigProtocolHandle, &ConfigLangList);
    DestroyConfiglanguageList (&ConfigLangList);
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
HandleResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN  EFI_STRING                       Uri
  )
{
  EFI_STATUS           Status;
  REDFISH_SCHEMA_INFO  SchemaInfo;
  EFI_STRING           ConfigLang;
  BOOLEAN              SystemRestDetected;

  if ((Private == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Resource match
  //

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: process resource for: %s\n", __FUNCTION__, Uri));

  Status = GetRedfishSchemaInfo (Private->RedfishService, Private->JsonStructProtocol, Uri, NULL, &SchemaInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get schema information from: %s %r\n", __FUNCTION__, Uri, Status));
    return Status;
  }

  //
  // Check and see if this is target resource that we want to handle.
  // Some resource is handled by other provider so we have to make sure this first.
  //
  DEBUG ((REDFISH_DEBUG_TRACE, "%s Identify for %s\n", __FUNCTION__, Uri));
  SystemRestDetected = FALSE;
  ConfigLang         = RedfishGetConfigLanguage (Uri);
  if (ConfigLang == NULL) {
    Status = EdkIIRedfishResourceConfigIdentify (&SchemaInfo, Uri, NULL, Private->InformationExchange);
    if (EFI_ERROR (Status)) {
      if (Status == EFI_UNSUPPORTED) {
        DEBUG ((REDFISH_DEBUG_TRACE, "%a: \"%s\" is not handled by us\n", __FUNCTION__, Uri));
        return EFI_SUCCESS;
      } else if (Status == EFI_NOT_FOUND) {
        DEBUG ((REDFISH_DEBUG_TRACE, "%a: \"%s\" has nothing to handle\n", __FUNCTION__, Uri));
        return EFI_SUCCESS;
      }

      DEBUG ((DEBUG_ERROR, "%a: fail to identify resource: \"%s\": %r\n", __FUNCTION__, Uri, Status));
      return Status;
    }

    //
    // When there is no history record in UEFI variable, this is first boot or
    // system is reset by defaulting command. The pending setting on BMC may be
    // a stale value so we will ignore pending settings in BMC.
    //
    SystemRestDetected = TRUE;
  } else {
    DEBUG ((REDFISH_DEBUG_TRACE, "%a: history record found: %s\n", __FUNCTION__, ConfigLang));
    FreePool (ConfigLang);
  }

  //
  // Check and see if target property exist or not even when collection member exists.
  // If not, we sill do provision.
  //
  DEBUG ((REDFISH_DEBUG_TRACE, "%a Check for %s\n", __FUNCTION__, Uri));
  Status = EdkIIRedfishResourceConfigCheck (&SchemaInfo, Uri, NULL);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_UNSUPPORTED) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: \"%s\" is not handled by us\n", __FUNCTION__, Uri));
      return EFI_SUCCESS;
    }

    //
    // The target property does not exist, do the provision to create property.
    //
    DEBUG ((REDFISH_DEBUG_TRACE, "%a provision for %s\n", __FUNCTION__, Uri));
    Status = EdkIIRedfishResourceConfigProvisioning (&SchemaInfo, Uri, NULL, Private->InformationExchange, FALSE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to provision with GET mode: %r\n", __FUNCTION__, Status));
    }

    DEBUG ((REDFISH_DEBUG_TRACE, "%a: process resource for: %s finished\n", __FUNCTION__, Uri));

    return Status;
  }

  //
  // Consume first.
  //
  if (SystemRestDetected) {
    DEBUG ((REDFISH_DEBUG_TRACE, "%a system has been reset to default setting. ignore pending settings because they may be stale values\n", __FUNCTION__));
  } else {
    DEBUG ((REDFISH_DEBUG_TRACE, "%a consume for %s\n", __FUNCTION__, Uri));
    Status = EdkIIRedfishResourceConfigConsume (&SchemaInfo, Uri, NULL);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to consume resource for: %s: %r\n", __FUNCTION__, Uri, Status));
    }
  }

  //
  // Patch.
  //
  DEBUG ((REDFISH_DEBUG_TRACE, "%a update for %s\n", __FUNCTION__, Uri));
  Status = EdkIIRedfishResourceConfigUpdate (&SchemaInfo, Uri, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to update resource for: %s: %r\n", __FUNCTION__, Uri, Status));
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: process resource for: %s finished\n", __FUNCTION__, Uri));

  return Status;
}
