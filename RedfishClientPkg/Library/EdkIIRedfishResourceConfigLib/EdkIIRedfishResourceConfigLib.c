/** @file
  Redfish resource config library implementation

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "EdkIIRedfishResourceConfigInternal.h"

REDFISH_CONFIG_PROTOCOL_CACHE                    *mRedfishResourceConfigCache  = NULL;
REDFISH_CONFIG_PROTOCOL_CACHE                    *mRedfishResourceConfig2Cache = NULL;
EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL  mRedfishFeatureInterchangeData;

/**

  Get schema information by given protocol and service instance if JsonText
  is NULL or empty. When JsonText is provided by caller, this function read
  schema information from JsonText.

  @param[in]  RedfishService      Pointer to Redfish service instance.
  @param[in]  JsonStructProtocol  Json Structure protocol instance.
  @param[in]  Uri                 Target URI.
  @param[in]  JsonText            Redfish data in JSON format. This is optional.
  @param[out] SchemaInfo          Returned schema information.

  @retval     EFI_SUCCESS         Schema information is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
EFIAPI
GetRedfishSchemaInfo (
  IN  REDFISH_SERVICE                   *RedfishService,
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL  *JsonStructProtocol,
  IN  EFI_STRING                        Uri,
  IN  CHAR8                             *JsonText OPTIONAL,
  OUT REDFISH_SCHEMA_INFO               *SchemaInfo
  )
{
  EFI_STATUS                      Status;
  REDFISH_RESPONSE                Response;
  CHAR8                           *JsonData;
  EFI_REST_JSON_STRUCTURE_HEADER  *Header;

  if ((RedfishService == NULL) || (JsonStructProtocol == NULL) || IS_EMPTY_STRING (Uri) || (SchemaInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  JsonData = NULL;
  Header   = NULL;
  ZeroMem (&Response, sizeof (Response));
  if (IS_EMPTY_STRING (JsonText)) {
    Status = RedfishHttpGetResource (RedfishService, Uri, &Response, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to get resource from %s: %r", __func__, Uri, Status));
      return Status;
    }

    if (Response.Payload != NULL) {
      JsonData = JsonDumpString (RedfishJsonInPayload (Response.Payload), EDKII_JSON_COMPACT);
    }
  } else {
    JsonData = AllocateCopyPool (AsciiStrSize (JsonText), JsonText);
  }

  if (IS_EMPTY_STRING (JsonData)) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  //
  // Convert JSON text to C structure.
  //
  Status = JsonStructProtocol->ToStructure (
                                 JsonStructProtocol,
                                 NULL,
                                 JsonData,
                                 &Header
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __func__, Status));
    goto ON_RELEASE;
  }

  AsciiStrCpyS (SchemaInfo->Schema, REDFISH_SCHEMA_STRING_SIZE, Header->JsonRsrcIdentifier.NameSpace.ResourceTypeName);
  AsciiStrCpyS (SchemaInfo->Major, REDFISH_SCHEMA_VERSION_SIZE, Header->JsonRsrcIdentifier.NameSpace.MajorVersion);
  AsciiStrCpyS (SchemaInfo->Minor, REDFISH_SCHEMA_VERSION_SIZE, Header->JsonRsrcIdentifier.NameSpace.MinorVersion);
  AsciiStrCpyS (SchemaInfo->Errata, REDFISH_SCHEMA_VERSION_SIZE, Header->JsonRsrcIdentifier.NameSpace.ErrataVersion);

ON_RELEASE:
  //
  // Release resource.
  //
  if (Response.Payload != NULL) {
    RedfishHttpFreeResource (&Response);
  }

  if (JsonData != NULL) {
    FreePool (JsonData);
  }

  JsonStructProtocol->DestoryStructure (JsonStructProtocol, Header);

  return Status;
}

/**

  Get supported schema list by given specify schema name.

  @param[in]  Schema      Schema type name.
  @param[out] SchemaInfo  Returned schema information.

  @retval     EFI_SUCCESS         Schema information is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
EFIAPI
GetSupportedSchemaVersion (
  IN   CHAR8                *Schema,
  OUT  REDFISH_SCHEMA_INFO  *SchemaInfo
  )
{
  EFI_STATUS  Status;
  CHAR8       *SupportSchema;
  CHAR8       *SchemaName;
  UINTN       Index;
  UINTN       Index2;
  BOOLEAN     Found;

  if (IS_EMPTY_STRING (Schema) || (SchemaInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishPlatformConfigGetSupportedSchema (&SupportSchema);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "Supported schema: %a\n", SupportSchema));

  Index      = 0;
  Found      = FALSE;
  SchemaName = SupportSchema;
  while (TRUE) {
    if ((SupportSchema[Index] == ';') || (SupportSchema[Index] == '\0')) {
      if (AsciiStrnCmp (&SchemaName[SCHEMA_NAME_PREFIX_OFFSET], Schema, AsciiStrLen (Schema)) == 0) {
        Found                = TRUE;
        SupportSchema[Index] = '\0';
        break;
      }

      SchemaName = &SupportSchema[Index + 1];
    }

    if (SupportSchema[Index] == '\0') {
      break;
    }

    ++Index;
  }

  if (Found) {
    AsciiStrCpyS (SchemaInfo->Schema, REDFISH_SCHEMA_STRING_SIZE, Schema);

    //
    // forward to '.'
    //
    Index = 0;
    while (SchemaName[Index] != '\0' && SchemaName[Index] != '.') {
      ++Index;
    }

    ASSERT (SchemaName[Index] != '\0');

    //
    // Skip '.' and 'v'
    //
    Index += 2;

    //
    // forward to '_'
    //
    Index2 = Index;
    while (SchemaName[Index2] != '\0' && SchemaName[Index2] != '_') {
      ++Index2;
    }

    ASSERT (SchemaName[Index2] != '\0');

    AsciiStrnCpyS (SchemaInfo->Major, REDFISH_SCHEMA_VERSION_SIZE, &SchemaName[Index], (Index2 - Index));
    Index = Index2;

    //
    // Skip '_'
    //
    ++Index;

    //
    // forward to '_'
    //
    Index2 = Index;
    while (SchemaName[Index2] != '\0' && SchemaName[Index2] != '_') {
      ++Index2;
    }

    ASSERT (SchemaName[Index2] != '\0');

    AsciiStrnCpyS (SchemaInfo->Minor, REDFISH_SCHEMA_VERSION_SIZE, &SchemaName[Index], (Index2 - Index));
    Index = Index2;

    //
    // Skip '_'
    //
    ++Index;

    AsciiStrCpyS (SchemaInfo->Errata, REDFISH_SCHEMA_VERSION_SIZE, &SchemaName[Index]);
  }

  FreePool (SupportSchema);

  return (Found ? EFI_SUCCESS : EFI_NOT_FOUND);
}

/**

  Find Redfish Resource Config Protocol that supports given schema and version.

  @param[in]  Schema      Schema name.
  @param[out] Handle      Pointer to receive the handle that has EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL
                          installed on it.

  @retval     EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL *    Pointer to protocol
  @retval     NULL                                        No protocol found.

**/
EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *
GetRedfishResourceConfigProtocol (
  IN  REDFISH_SCHEMA_INFO  *Schema,
  OUT EFI_HANDLE           *Handle OPTIONAL
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              *HandleBuffer;
  UINTN                                   NumberOfHandles;
  UINTN                                   Index;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL  *Protocol;
  REDFISH_SCHEMA_INFO                     SchemaInfo;
  BOOLEAN                                 Found;

  if (IS_EMPTY_STRING (Schema->Schema) ||
      IS_EMPTY_STRING (Schema->Major) ||
      IS_EMPTY_STRING (Schema->Minor) ||
      IS_EMPTY_STRING (Schema->Errata)
      )
  {
    return NULL;
  }

  if ((mRedfishResourceConfigCache != NULL) && (mRedfishResourceConfigCache->RedfishResourceConfig.ConfigProtocol != NULL)) {
    if ((AsciiStrCmp (Schema->Schema, mRedfishResourceConfigCache->SchemaInfoCache.Schema) == 0) &&
        (AsciiStrCmp (Schema->Major, mRedfishResourceConfigCache->SchemaInfoCache.Major) == 0) &&
        (AsciiStrCmp (Schema->Minor, mRedfishResourceConfigCache->SchemaInfoCache.Minor) == 0) &&
        (AsciiStrCmp (Schema->Errata, mRedfishResourceConfigCache->SchemaInfoCache.Errata) == 0))
    {
      if (Handle != NULL) {
        *Handle = mRedfishResourceConfigCache->CachedHandle;
      }

      return mRedfishResourceConfigCache->RedfishResourceConfig.ConfigProtocol;
    }
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEdkIIRedfishResourceConfigProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Found = FALSE;

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEdkIIRedfishResourceConfigProtocolGuid,
                    (VOID **)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->GetInfo (Protocol, &SchemaInfo);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if ((AsciiStrCmp (Schema->Schema, SchemaInfo.Schema) == 0) &&
        (AsciiStrCmp (Schema->Major, SchemaInfo.Major) == 0) &&
        (AsciiStrCmp (Schema->Minor, SchemaInfo.Minor) == 0) &&
        (AsciiStrCmp (Schema->Errata, SchemaInfo.Errata) == 0))
    {
      Found = TRUE;
      break;
    }
  }

  if (Found) {
    if (mRedfishResourceConfigCache != NULL) {
      mRedfishResourceConfigCache->CachedHandle                         = HandleBuffer[Index];
      mRedfishResourceConfigCache->RedfishResourceConfig.ConfigProtocol = Protocol;
      CopyMem (&mRedfishResourceConfigCache->SchemaInfoCache, Schema, sizeof (REDFISH_SCHEMA_INFO));
    }

    if (Handle != NULL) {
      *Handle = HandleBuffer[Index];
    }
  }

  FreePool (HandleBuffer);

  return (Found ? Protocol : NULL);
}

/**

  Find Redfish Resource Config2 Protocol that supports given schema and version.

  @param[in]  Schema      Schema name.
  @param[out] Handle      Pointer to receive the handle that has EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL
                          installed on it.

  @retval     EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL *    Pointer to protocol
  @retval     NULL                                         No protocol found.

**/
EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *
GetRedfishResourceConfig2Protocol (
  IN  REDFISH_SCHEMA_INFO  *Schema,
  OUT EFI_HANDLE           *Handle OPTIONAL
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               *HandleBuffer;
  UINTN                                    NumberOfHandles;
  UINTN                                    Index;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Protocol;
  REDFISH_SCHEMA_INFO                      SchemaInfo;
  BOOLEAN                                  Found;

  if (IS_EMPTY_STRING (Schema->Schema) ||
      IS_EMPTY_STRING (Schema->Major) ||
      IS_EMPTY_STRING (Schema->Minor) ||
      IS_EMPTY_STRING (Schema->Errata)
      )
  {
    return NULL;
  }

  if ((mRedfishResourceConfig2Cache != NULL) && (mRedfishResourceConfig2Cache->RedfishResourceConfig.Config2Protocol != NULL)) {
    if ((AsciiStrCmp (Schema->Schema, mRedfishResourceConfig2Cache->SchemaInfoCache.Schema) == 0) &&
        (AsciiStrCmp (Schema->Major, mRedfishResourceConfig2Cache->SchemaInfoCache.Major) == 0) &&
        (AsciiStrCmp (Schema->Minor, mRedfishResourceConfig2Cache->SchemaInfoCache.Minor) == 0) &&
        (AsciiStrCmp (Schema->Errata, mRedfishResourceConfig2Cache->SchemaInfoCache.Errata) == 0))
    {
      if (Handle != NULL) {
        *Handle = mRedfishResourceConfig2Cache->CachedHandle;
      }

      return mRedfishResourceConfig2Cache->RedfishResourceConfig.Config2Protocol;
    }
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEdkIIRedfishResourceConfig2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Found = FALSE;

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEdkIIRedfishResourceConfig2ProtocolGuid,
                    (VOID **)&Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Protocol->GetInfo (Protocol, &SchemaInfo);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if ((AsciiStrCmp (Schema->Schema, SchemaInfo.Schema) == 0) &&
        (AsciiStrCmp (Schema->Major, SchemaInfo.Major) == 0) &&
        (AsciiStrCmp (Schema->Minor, SchemaInfo.Minor) == 0) &&
        (AsciiStrCmp (Schema->Errata, SchemaInfo.Errata) == 0))
    {
      Found = TRUE;
      break;
    }
  }

  if (Found) {
    if (mRedfishResourceConfig2Cache != NULL) {
      mRedfishResourceConfig2Cache->CachedHandle                          = HandleBuffer[Index];
      mRedfishResourceConfig2Cache->RedfishResourceConfig.Config2Protocol = Protocol;
      CopyMem (&mRedfishResourceConfig2Cache->SchemaInfoCache, Schema, sizeof (REDFISH_SCHEMA_INFO));
    }

    if (Handle != NULL) {
      *Handle = HandleBuffer[Index];
    }
  }

  FreePool (HandleBuffer);

  return (Found ? Protocol : NULL);
}

/**
  Install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL
  on child feature driver handle.

  @param[in]   Handle  Handle to install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
InstallInterchangeDataProtocol (
  IN  EFI_HANDLE                     Handle,
  IN  RESOURCE_INFORMATION_EXCHANGE  *InformationExchange
  )
{
  EFI_STATUS                                       Status;
  EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL  *Interface;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEdkIIRedfishFeatureInterchangeDataProtocolGuid,
                  (VOID **)&Interface
                  );
  if (!EFI_ERROR (Status)) {
    Interface->ResourceInformationExchage = InformationExchange;
    return EFI_SUCCESS;
  }

  if (Status == EFI_UNSUPPORTED) {
    mRedfishFeatureInterchangeData.ResourceInformationExchage = InformationExchange;
    Status                                                    = gBS->InstallProtocolInterface (
                                                                       &Handle,
                                                                       &gEdkIIRedfishFeatureInterchangeDataProtocolGuid,
                                                                       EFI_NATIVE_INTERFACE,
                                                                       (VOID *)&mRedfishFeatureInterchangeData
                                                                       );
  }

  return Status;
}

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ImageHandle     Pointer to image handle.
  @param[in]   ConfigLangList  Pointer to REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceSetConfigureLang (
  IN EFI_HANDLE                                   ImageHandle,
  IN REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *ConfigLangList
  )
{
  EFI_STATUS                                       Status;
  UINTN                                            Index;
  EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL  *Interface;

  if ((ImageHandle == NULL) || (ConfigLangList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((ConfigLangList->Count == 0) || (ConfigLangList->List == NULL)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEdkIIRedfishFeatureInterchangeDataProtocolGuid,
                  (VOID **)&Interface
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL is not installed on %p: %r\n", __func__, ImageHandle, Status));
    return Status;
  }

  Interface->ResourceInformationExchage->ReturnedInformation.Type                        = InformationTypeCollectionMemberConfigLanguage;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.Count = ConfigLangList->Count;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List  =
    AllocateZeroPool (sizeof (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG) * ConfigLangList->Count);
  if (Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG.\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < ConfigLangList->Count; Index++) {
    Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List[Index].Index         = ConfigLangList->List[Index].Index;
    Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List[Index].ConfigureLang =
      (EFI_STRING)AllocateCopyPool (StrSize (ConfigLangList->List[Index].ConfigureLang), (VOID *)ConfigLangList->List[Index].ConfigureLang);
  }

  return EFI_SUCCESS;
}

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ImageHandle         Pointer to image handle.
  @param[in]   ConfigLangString    Configure language string.
  @param[in]   Index               Index value of configure language string.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceSetConfigureLangString (
  IN EFI_HANDLE  ImageHandle,
  IN EFI_STRING  ConfigLangString,
  IN UINTN       Index
  )
{
  EFI_STATUS                                       Status;
  EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL  *Interface;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG           *ConfigLang;

  if ((ImageHandle == NULL) || IS_EMPTY_STRING (ConfigLangString)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEdkIIRedfishFeatureInterchangeDataProtocolGuid,
                  (VOID **)&Interface
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL is not installed on %p: %r\n", __func__, ImageHandle, Status));
    return Status;
  }

  ConfigLang = AllocateZeroPool (sizeof (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG));
  if (ConfigLang == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG.\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  Interface->ResourceInformationExchage->ReturnedInformation.Type                                        = InformationTypeCollectionMemberConfigLanguage;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.Count                 = 1;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List                  = ConfigLang;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List[0].Index         = Index;
  Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List[0].ConfigureLang = (EFI_STRING)AllocateCopyPool (StrSize (ConfigLangString), (VOID *)ConfigLangString);
  if (Interface->ResourceInformationExchage->ReturnedInformation.ConfigureLanguageList.List[0].ConfigureLang == NULL) {
    FreePool (ConfigLang);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Provision redfish resource by given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 Target URI to create resource.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.
  @param[in]   HttpPostMode        TRUE if resource does not exist, HTTP POST method is used.
                                   FALSE if the resource exist but some of properties are missing,
                                   HTTP PUT method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigProvisioning (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     CHAR8                          *JsonText OPTIONAL,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchange,
  IN     BOOLEAN                        HttpPostMode
  )
{
  EFI_HANDLE                               Handle;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL   *ConfigProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Config2Protocol;

  //
  // Initialization.
  //
  Handle          = NULL;
  ConfigProtocol  = NULL;
  Config2Protocol = NULL;

  //
  // Try to use config2 protocol first.
  //
  Config2Protocol = GetRedfishResourceConfig2Protocol (Schema, &Handle);
  if ((Config2Protocol != NULL) && (Handle != NULL)) {
    //
    // Install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL on the child
    // feature driver handle.
    //
    InstallInterchangeDataProtocol (Handle, InformationExchange);
    return Config2Protocol->Provisioning (Config2Protocol, Uri, JsonText, HttpPostMode);
  }

  ConfigProtocol = GetRedfishResourceConfigProtocol (Schema, &Handle);
  if ((ConfigProtocol == NULL) || (Handle == NULL)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL on the child
  // feature driver handle.
  //
  InstallInterchangeDataProtocol (Handle, InformationExchange);
  return ConfigProtocol->Provisioning (ConfigProtocol, Uri, HttpPostMode);
}

/**
  Consume resource from given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigConsume (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  )
{
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL   *ConfigProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Config2Protocol;

  //
  // Initialization.
  //
  ConfigProtocol  = NULL;
  Config2Protocol = NULL;

  //
  // Try to use config2 protocol first.
  //
  Config2Protocol = GetRedfishResourceConfig2Protocol (Schema, NULL);
  if (Config2Protocol != NULL) {
    return Config2Protocol->Consume (Config2Protocol, Uri, JsonText);
  }

  ConfigProtocol = GetRedfishResourceConfigProtocol (Schema, NULL);
  if (ConfigProtocol == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return ConfigProtocol->Consume (ConfigProtocol, Uri);
}

/**
  Update resource to given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigUpdate (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  )
{
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL   *ConfigProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Config2Protocol;

  //
  // Initialization.
  //
  ConfigProtocol  = NULL;
  Config2Protocol = NULL;

  //
  // Try to use config2 protocol first.
  //
  Config2Protocol = GetRedfishResourceConfig2Protocol (Schema, NULL);
  if (Config2Protocol != NULL) {
    return Config2Protocol->Update (Config2Protocol, Uri, JsonText);
  }

  ConfigProtocol = GetRedfishResourceConfigProtocol (Schema, NULL);
  if (ConfigProtocol == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return ConfigProtocol->Update (ConfigProtocol, Uri);
}

/**
  Check resource on given URI.

  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigCheck (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  )
{
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL   *ConfigProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Config2Protocol;

  //
  // Initialization.
  //
  ConfigProtocol  = NULL;
  Config2Protocol = NULL;

  //
  // Try to use config2 protocol first.
  //
  Config2Protocol = GetRedfishResourceConfig2Protocol (Schema, NULL);
  if (Config2Protocol != NULL) {
    return Config2Protocol->Check (Config2Protocol, Uri, JsonText);
  }

  ConfigProtocol = GetRedfishResourceConfigProtocol (Schema, NULL);
  if (ConfigProtocol == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return ConfigProtocol->Check (ConfigProtocol, Uri);
}

/**
  Identify resource on given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.

  @retval EFI_SUCCESS              This is target resource which we want to handle.
  @retval EFI_UNSUPPORTED          This is not the target resource.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigIdentify (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     CHAR8                          *JsonText OPTIONAL,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchange
  )
{
  EFI_HANDLE                               Handle;
  EDKII_REDFISH_RESOURCE_CONFIG_PROTOCOL   *ConfigProtocol;
  EDKII_REDFISH_RESOURCE_CONFIG2_PROTOCOL  *Config2Protocol;

  //
  // Initialization.
  //
  Handle          = NULL;
  ConfigProtocol  = NULL;
  Config2Protocol = NULL;

  //
  // Try to use config2 protocol first.
  //
  Config2Protocol = GetRedfishResourceConfig2Protocol (Schema, &Handle);
  if ((Config2Protocol != NULL) && (Handle != NULL)) {
    //
    // Install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL on the child
    // feature driver handle.
    //
    InstallInterchangeDataProtocol (Handle, InformationExchange);
    return Config2Protocol->Identify (Config2Protocol, Uri, JsonText);
  }

  ConfigProtocol = GetRedfishResourceConfigProtocol (Schema, &Handle);
  if (ConfigProtocol == NULL) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Install EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL on the child
  // feature driver handle.
  //
  InstallInterchangeDataProtocol (Handle, InformationExchange);
  return ConfigProtocol->Identify (ConfigProtocol, Uri);
}

/**

  Initial resource config library instance.

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCCESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishResourceConfigConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  mRedfishResourceConfigCache = AllocateZeroPool (sizeof (REDFISH_CONFIG_PROTOCOL_CACHE));
  if (mRedfishResourceConfigCache == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mRedfishResourceConfig2Cache = AllocateZeroPool (sizeof (REDFISH_CONFIG_PROTOCOL_CACHE));
  if (mRedfishResourceConfig2Cache == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&mRedfishFeatureInterchangeData, sizeof (mRedfishFeatureInterchangeData));

  return EFI_SUCCESS;
}

/**
  Release allocated resource.

  @param[in] ImageHandle       Handle that identifies the image to be unloaded.
  @param[in] SystemTable      The system table.

  @retval EFI_SUCCESS      The image has been unloaded.

**/
EFI_STATUS
EFIAPI
RedfishResourceConfigDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mRedfishResourceConfigCache != NULL) {
    FreePool (mRedfishResourceConfigCache);
    mRedfishResourceConfigCache = NULL;
  }

  if (mRedfishResourceConfig2Cache != NULL) {
    FreePool (mRedfishResourceConfig2Cache);
    mRedfishResourceConfig2Cache = NULL;
  }

  return EFI_SUCCESS;
}
