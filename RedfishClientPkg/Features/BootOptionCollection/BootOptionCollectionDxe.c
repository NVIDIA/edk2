/** @file

  Redfish feature driver implementation - BootOptionCollection

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "BootOptionCollectionDxe.h"

REDFISH_COLLECTION_PRIVATE  *mRedfishCollectionPrivate = NULL;

/**
  Process specific member in collection.

  @param[in]  Private      Pointer to private instance.
  @param[in]  Uri          URI to this member.

  @retval     EFI_SUCCESS         collection member is handled successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
HandleResource (
  IN  REDFISH_COLLECTION_PRIVATE  *Private,
  IN  EFI_STRING                  Uri
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

  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: process resource for: %s\n", __func__, Uri));

  Status = GetRedfishSchemaInfo (Private->RedfishService, Private->JsonStructProtocol, Uri, &SchemaInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get schema information from: %s %r\n", __func__, Uri, Status));
    return Status;
  }

  //
  // Check and see if this is target resource that we want to handle.
  // Some resource is handled by other provider so we have to make sure this first.
  //
  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: Identify for %s\n", __func__, Uri));
  SystemRestDetected = FALSE;
  ConfigLang         = RedfishGetConfigLanguage (Uri);
  if (ConfigLang == NULL) {
    Status = EdkIIRedfishResourceConfigIdentify (&SchemaInfo, Uri, Private->InformationExchange);
    if (EFI_ERROR (Status)) {
      if (Status == EFI_UNSUPPORTED) {
        DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: \"%s\" is not handled by us\n", __func__, Uri));
        return EFI_SUCCESS;
      } else if (Status == EFI_NOT_FOUND) {
        DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: \"%s\" has nothing to handle\n", __func__, Uri));
        return EFI_SUCCESS;
      }

      DEBUG ((DEBUG_ERROR, "%a: fail to identify resource: \"%s\": %r\n", __func__, Uri, Status));
      return Status;
    }

    //
    // When there is no history record in UEFI variable, this is first boot or
    // system is reset by defaulting command. The pending setting on BMC may be
    // a stale value so we will ignore pending settings in BMC.
    //
    SystemRestDetected = TRUE;
  } else {
    DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: history record found: %s\n", __func__, ConfigLang));
    FreePool (ConfigLang);
  }

  //
  // Check and see if target property exist or not even when collection member exists.
  // If not, we sill do provision.
  //
  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a Check for %s\n", __func__, Uri));
  Status = EdkIIRedfishResourceConfigCheck (&SchemaInfo, Uri);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_UNSUPPORTED) {
      DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: \"%s\" is not handled by us\n", __func__, Uri));
      return EFI_ABORTED;
    }

    //
    // The target property does not exist, do the provision to create property.
    //
    DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a provision for %s\n", __func__, Uri));
    Status = EdkIIRedfishResourceConfigProvisionging (&SchemaInfo, Uri, Private->InformationExchange, FALSE);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to provision with GET mode: %r\n", __func__, Status));
    }

    return Status;
  }

  //
  // Consume first.
  //
  if (SystemRestDetected) {
    DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a system has been reset to default setting. ignore pending settings because they may be stale values\n", __func__));
  } else {
    DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a consume for %s\n", __func__, Uri));
    Status = EdkIIRedfishResourceConfigConsume (&SchemaInfo, Uri);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: failed to consume resource for: %s: %r\n", __func__, Uri, Status));
    }
  }

  //
  // Patch.
  //
  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a update for %s\n", __func__, Uri));
  Status = EdkIIRedfishResourceConfigUpdate (&SchemaInfo, Uri);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to update resource for: %s: %r\n", __func__, Uri, Status));
  }

  return Status;
}

/**
  This function get boot option number from given Uri and mark the boot option
  in BootOptions array as processed.

  @param[in]  RedfishService      Pointer Redfish service.
  @param[in]  BootOptions         Boot option array.
  @param[in]  BootOptionCount     Number of boot option in BootOptions array.
  @param[in]  Uri                 The URI to get boot option information.

  @retval     EFI_SUCCESS         Boot option is marked successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
MarkBootOptionProcessed (
  IN REDFISH_SERVICE               RedfishService,
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions,
  IN UINTN                         BootOptionCount,
  IN EFI_STRING                    Uri
  )
{
  UINTN             Index;
  REDFISH_RESPONSE  Response;
  EDKII_JSON_VALUE  JsonValue;
  EDKII_JSON_VALUE  BootOptionReference;
  EFI_STATUS        Status;
  CONST CHAR8       *BootReferenceString;
  UINTN             OptionNumber;

  if ((BootOptions == NULL) || (BootOptionCount == 0) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get boot option reference attribute
  //
  ZeroMem (&Response, sizeof (REDFISH_RESPONSE));
  Status = RedfishHttpGetResource (RedfishService, Uri, &Response, TRUE);
  if (EFI_ERROR (Status) || (Response.Payload == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get resource from %s: %r", __func__, Uri, Status));
    return Status;
  }

  JsonValue = RedfishJsonInPayload (Response.Payload);
  if (!JsonValueIsObject (JsonValue)) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  BootOptionReference = JsonObjectGetValue (JsonValueGetObject (JsonValue), REDFISH_BOOT_OPTION_REFERENCE_NAME);
  if (!JsonValueIsString (BootOptionReference)) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  BootReferenceString = JsonValueGetAsciiString (BootOptionReference);
  if (BootReferenceString == NULL) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  if (AsciiStrnCmp (BootReferenceString, REDFISH_BOOT_OPTION_PREFIX, REDFISH_BOOT_OPTION_PREFIX_LEN) != 0) {
    Status = EFI_NOT_FOUND;
    goto ON_RELEASE;
  }

  OptionNumber = AsciiStrHexToUintn (&BootReferenceString[4]);
  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: boot option number: 0x%x in %s\n", __func__, OptionNumber, Uri));

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < BootOptionCount; Index++) {
    if (BootOptions[Index].OptionNumber == OptionNumber) {
      BootOptions[Index].Status = EFI_ALREADY_STARTED;
      Status                    = EFI_SUCCESS;
      break;
    }
  }

ON_RELEASE:

  RedfishHttpFreeResource (&Response);

  return Status;
}

/**
  Create new member to collection resource.

  @param[in]  Private      Pointer to private instance.
  @param[in]  BootOption   The boot option which will be updated to BMC.

  @retval     EFI_SUCCESS         New collection member is created successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
CreateCollectionResource (
  IN  REDFISH_COLLECTION_PRIVATE    *Private,
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS           Status;
  REDFISH_SCHEMA_INFO  SchemaInfo;
  EFI_STRING           BootOptionUri;
  UINTN                UriSize;

  if ((Private == NULL) || (BootOption == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  UriSize       = (StrLen (Private->CollectionUri) + REDFISH_BOOT_OPTION_PREFIX_LEN + REDFISH_BOOT_OPTION_NUM_LEN + sizeof (REDFISH_BOOT_OPTION_PARAMETER) + 1) * sizeof (CHAR16);
  BootOptionUri = AllocateZeroPool (UriSize);
  if (BootOptionUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Build URI with query parameter to deliver the boot option name
  //
  UnicodeSPrint (BootOptionUri, UriSize, L"%s%a%a%04X", Private->CollectionUri, REDFISH_BOOT_OPTION_PARAMETER, REDFISH_BOOT_OPTION_PREFIX, BootOption->OptionNumber);

  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: create resource for collection for: %s\n", __func__, BootOptionUri));

  AsciiStrCpyS (SchemaInfo.Schema, REDFISH_SCHEMA_STRING_SIZE, REDFISH_SCHEMA_NAME);
  AsciiStrCpyS (SchemaInfo.Major, REDFISH_SCHEMA_VERSION_SIZE, REDFISH_SCHEMA_MAJOR);
  AsciiStrCpyS (SchemaInfo.Minor, REDFISH_SCHEMA_VERSION_SIZE, REDFISH_SCHEMA_MINOR);
  AsciiStrCpyS (SchemaInfo.Errata, REDFISH_SCHEMA_VERSION_SIZE, REDFISH_SCHEMA_ERRATA);

  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: supported schema: %a %a.%a.%a\n", __func__, SchemaInfo.Schema, SchemaInfo.Major, SchemaInfo.Minor, SchemaInfo.Errata));

  Status = EdkIIRedfishResourceConfigProvisionging (&SchemaInfo, BootOptionUri, Private->InformationExchange, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to create resource for: %s: %r\n", __func__, BootOptionUri, Status));
  }

  return Status;
}

/**
  Process members in collection resource.

  @param[in]  Private      Pointer to private instance.

  @retval     EFI_SUCCESS         collection members are handled successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
HandleCollectionResource (
  IN  REDFISH_COLLECTION_PRIVATE  *Private
  )
{
  EFI_STATUS                           Status;
  EFI_REDFISH_BOOTOPTIONCOLLECTION     *Collection;
  EFI_REDFISH_BOOTOPTIONCOLLECTION_CS  *CollectionCs;
  RedfishCS_Link                       *List;
  RedfishCS_Header                     *Header;
  RedfishCS_Type_Uri_Data              *UriData;
  EFI_STRING                           MemberUri;
  EFI_BOOT_MANAGER_LOAD_OPTION         *BootOptions;
  UINTN                                BootOptionCount;
  UINTN                                Index;
  BOOLEAN                              IsLegacyOption;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Private->JsonStructProtocol == NULL) || (Private->CollectionJson == NULL)) {
    return EFI_NOT_READY;
  }

  //
  // Load boot options in system.
  //
  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: process collection for: %s\n", __func__, Private->CollectionUri));

  //
  // Convert JSON text to C structure.
  //
  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Private->CollectionJson,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&Collection
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __func__, Status));
    return Status;
  }

  CollectionCs = Collection->BootOptionCollection;

  //
  // Handle existing resources.
  //
  if ((*CollectionCs->Membersodata_count > 0) && !IsLinkEmpty (&CollectionCs->Members)) {
    List = GetFirstLink (&CollectionCs->Members);
    while (TRUE) {
      Header = (RedfishCS_Header *)List;

      if (Header->ResourceType == RedfishCS_Type_Uri) {
        UriData   = (RedfishCS_Type_Uri_Data *)Header;
        MemberUri = NULL;
        MemberUri = StrAsciiToUnicode (UriData->Uri);

        ASSERT (MemberUri != NULL);

        if (MemberUri != NULL) {
          Status = HandleResource (Private, MemberUri);
          if (EFI_ERROR (Status)) {
            if (Status != EFI_ABORTED) {
              DEBUG ((DEBUG_ERROR, "%a: process BootOptionCollection resource: %a failed: %r\n", __func__, UriData->Uri, Status));
            }
          }

          //
          // This boot option is processed.
          //
          if (Status != EFI_ABORTED) {
            Status = MarkBootOptionProcessed (Private->RedfishService, BootOptions, BootOptionCount, MemberUri);
            if (EFI_ERROR (Status)) {
              DEBUG ((DEBUG_ERROR, "%a: mark boot option: %a failed: %r\n", __func__, UriData->Uri, Status));
            }
          }

          FreePool (MemberUri);
        }
      }

      if (IsLinkAtEnd (&CollectionCs->Members, List)) {
        break;
      }

      List = GetNextLink (&CollectionCs->Members, List);
    }
  }

  //
  // Handle new resources.
  //
  for (Index = 0; Index < BootOptionCount; Index++) {
    //
    // This boot option is handled already.
    //
    if (BootOptions[Index].Status == EFI_ALREADY_STARTED) {
      continue;
    }

    //
    // Don't display hidden boot options, but retain inactive ones.
    //
    if ((BootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) {
      continue;
    }

    //
    // Group the legacy boot option in the sub title created dynamically
    //
    IsLegacyOption = (BOOLEAN)((DevicePathType (BootOptions[Index].FilePath) == BBS_DEVICE_PATH) &&
                               (DevicePathSubType (BootOptions[Index].FilePath) == BBS_BBS_DP)
                               );

    //
    // Don't display legacy boot options
    //
    if (IsLegacyOption) {
      continue;
    }

    Status = CreateCollectionResource (Private, &BootOptions[Index]);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: can not create resource for: %s\n", __func__, BootOptions[Index].Description));
    }
  }

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (Private->JsonStructProtocol, (EFI_REST_JSON_STRUCTURE_HEADER *)Collection);
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);

  return EFI_SUCCESS;
}

EFI_STATUS
ReleaseCollectionResource (
  IN REDFISH_COLLECTION_PRIVATE  *Private
  )
{
  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Release resource
  //
  if (Private->Response.Payload != NULL) {
    RedfishHttpFreeResource (&Private->Response);
    Private->Response.StatusCode  = NULL;
    Private->Response.HeaderCount = 0;
    Private->Response.Headers     = NULL;
    Private->Response.Payload     = NULL;
  }

  if (Private->CollectionJson != NULL) {
    FreePool (Private->CollectionJson);
    Private->CollectionJson = NULL;
  }

  if (Private->RedfishVersion != NULL) {
    FreePool (Private->RedfishVersion);
    Private->RedfishVersion = NULL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CollectionHandler (
  IN REDFISH_COLLECTION_PRIVATE  *Private
  )
{
  EFI_STATUS  Status;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_BOOT_OPTION_COLLECTION_DEBUG_TRACE, "%a: collection handler for %s\n", __func__, Private->CollectionUri));

  //
  // Query collection from Redfish service.
  //
  Status = RedfishHttpGetResource (Private->RedfishService, Private->CollectionUri, &Private->Response, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to get resource from: %s :%r\n", __func__, Private->CollectionUri, Status));
    goto ON_RELEASE;
  }

  Private->CollectionPayload = Private->Response.Payload;
  ASSERT (Private->CollectionPayload != NULL);

  Private->CollectionJson = JsonDumpString (RedfishJsonInPayload (Private->CollectionPayload), EDKII_JSON_COMPACT);
  ASSERT (Private->CollectionJson != NULL);

  Status = HandleCollectionResource (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: unable to handle collection resource at: %s :%r\n", __func__, Private->CollectionUri, Status));
  }

ON_RELEASE:

  ReleaseCollectionResource (Private);

  return Status;
}

/**
  The callback function provided by Redfish Feature driver.

  @param[in]     This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]     FeatureAction       The action Redfish feature driver should take.
  @param[in]     Uri                 The collection URI.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] InformationExchange The pointer to RESOURCE_INFORMATION_EXCHANGE

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
EFIAPI
RedfishCollectionFeatureCallback (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN     FEATURE_CALLBACK_ACTION         FeatureAction,
  IN     VOID                            *Context,
  IN OUT RESOURCE_INFORMATION_EXCHANGE   *InformationExchange
  )
{
  EFI_STATUS                  Status;
  REDFISH_SERVICE             RedfishService;
  REDFISH_COLLECTION_PRIVATE  *Private;
  EFI_STRING                  ResourceUri;

  if (FeatureAction != CallbackActionStartOperation) {
    return EFI_UNSUPPORTED;
  }

  Private = (REDFISH_COLLECTION_PRIVATE *)Context;

  RedfishService = Private->RedfishService;
  if (RedfishService == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: no Redfish service configured\n", __func__));
    return EFI_NOT_READY;
  }

  //
  // Save in private structure.
  //
  Private->InformationExchange = InformationExchange;

  //
  // Find Redfish version on BMC
  //
  Private->RedfishVersion = RedfishGetVersion (RedfishService);

  //
  // Create the full URI from Redfish service root.
  //
  ResourceUri = (EFI_STRING)AllocateZeroPool (MAX_URI_LENGTH * sizeof (CHAR16));
  if (ResourceUri == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for full URI.\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  StrCatS (ResourceUri, MAX_URI_LENGTH, Private->RedfishVersion);
  StrCatS (ResourceUri, MAX_URI_LENGTH, InformationExchange->SendInformation.FullUri);

  //
  // Initialize collection path
  //
  Private->CollectionUri = RedfishGetUri (ResourceUri);
  if (Private->CollectionUri == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CollectionHandler (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CollectionHandler failure: %r\n", __func__, Status));
  }

  return EFI_SUCCESS;
}

/**
  Initialize a Redfish configure handler.

  This function will be called by the Redfish config driver to initialize each Redfish configure
  handler.

  @param[in]   This                     Pointer to EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL instance.
  @param[in]   RedfishConfigServiceInfo Redfish service information.

  @retval EFI_SUCCESS                  The handler has been initialized successfully.
  @retval EFI_DEVICE_ERROR             Failed to create or configure the REST EX protocol instance.
  @retval EFI_ALREADY_STARTED          This handler has already been initialized.
  @retval Other                        Error happens during the initialization.

**/
EFI_STATUS
EFIAPI
RedfishCollectionInit (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This,
  IN  REDFISH_CONFIG_SERVICE_INFORMATION     *RedfishConfigServiceInfo
  )
{
  REDFISH_COLLECTION_PRIVATE  *Private;

  Private = REDFISH_COLLECTION_PRIVATE_DATA_FROM_PROTOCOL (This);

  Private->RedfishService = RedfishCreateService (RedfishConfigServiceInfo);
  if (Private->RedfishService == NULL) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Stop a Redfish configure handler.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL instance.

  @retval EFI_SUCCESS              This handler has been stoped successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishCollectionStop (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This
  )
{
  REDFISH_COLLECTION_PRIVATE  *Private;

  Private = REDFISH_COLLECTION_PRIVATE_DATA_FROM_PROTOCOL (This);

  if (Private->RedfishService != NULL) {
    RedfishCleanupService (Private->RedfishService);
    Private->RedfishService = NULL;
  }

  ReleaseCollectionResource (Private);

  if (Private->FeatureProtocol != NULL) {
    Private->FeatureProtocol->Unregister (
                                Private->FeatureProtocol,
                                REDFISH_MANAGED_URI,
                                NULL
                                );
  }

  return EFI_SUCCESS;
}

/**
  Callback function when gEfiRestJsonStructureProtocolGuid is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
EfiRestJasonStructureProtocolIsReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS  Status;

  if (mRedfishCollectionPrivate == NULL) {
    return;
  }

  if (mRedfishCollectionPrivate->JsonStructProtocol != NULL) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiRestJsonStructureProtocolGuid,
                  NULL,
                  (VOID **)&mRedfishCollectionPrivate->JsonStructProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate gEfiRestJsonStructureProtocolGuid: %r\n", __func__, Status));
  }

  gBS->CloseEvent (Event);
}

/**
  Callback function when gEdkIIRedfishFeatureProtocolGuid is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
EdkIIRedfishFeatureProtocolIsReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                      Status;
  EDKII_REDFISH_FEATURE_PROTOCOL  *FeatureProtocol;

  if (mRedfishCollectionPrivate == NULL) {
    return;
  }

  if (mRedfishCollectionPrivate->FeatureProtocol != NULL) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEdkIIRedfishFeatureProtocolGuid,
                  NULL,
                  (VOID **)&FeatureProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate gEdkIIRedfishFeatureProtocolGuid: %r\n", __func__, Status));
    gBS->CloseEvent (Event);
    return;
  }

  Status = FeatureProtocol->Register (
                              FeatureProtocol,
                              REDFISH_MANAGED_URI,
                              RedfishCollectionFeatureCallback,
                              (VOID *)mRedfishCollectionPrivate
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to register %s: %r\n", __func__, REDFISH_MANAGED_URI, Status));
  }

  mRedfishCollectionPrivate->FeatureProtocol = FeatureProtocol;

  gBS->CloseEvent (Event);
}

/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishCollectionUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS                             Status;
  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *ConfigHandler;

  if (mRedfishCollectionPrivate == NULL) {
    return EFI_NOT_READY;
  }

  ConfigHandler = NULL;

  //
  // Firstly, find ConfigHandler Protocol interface in this ImageHandle.
  //
  Status = gBS->OpenProtocol (
                  ImageHandle,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  (VOID **)&ConfigHandler,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                  );
  if (EFI_ERROR (Status) || (ConfigHandler == NULL)) {
    return Status;
  }

  ConfigHandler->Stop (ConfigHandler);

  //
  // Last, uninstall ConfigHandler Protocol.
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  ConfigHandler,
                  NULL
                  );

  FreePool (mRedfishCollectionPrivate);
  mRedfishCollectionPrivate = NULL;

  return Status;
}

EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  mRedfishConfigHandler = {
  RedfishCollectionInit,
  RedfishCollectionStop
};

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers. It initialize the global variables and
  publish the driver binding protocol.

  @param[in]   ImageHandle      The firmware allocated handle for the UEFI image.
  @param[in]   SystemTable      A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_ACCESS_DENIED     EFI_ISCSI_INITIATOR_NAME_PROTOCOL was installed unexpectedly.
  @retval Others                Other errors as indicated.
**/
EFI_STATUS
EFIAPI
RedfishCollectionEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;

  if (mRedfishCollectionPrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mRedfishCollectionPrivate = AllocateZeroPool (sizeof (REDFISH_COLLECTION_PRIVATE));
  CopyMem (&mRedfishCollectionPrivate->ConfigHandler, &mRedfishConfigHandler, sizeof (EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL));

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mRedfishCollectionPrivate->ConfigHandler
                  );

  EfiCreateProtocolNotifyEvent (
    &gEfiRestJsonStructureProtocolGuid,
    TPL_CALLBACK,
    EfiRestJasonStructureProtocolIsReady,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gEdkIIRedfishFeatureProtocolGuid,
    TPL_CALLBACK,
    EdkIIRedfishFeatureProtocolIsReady,
    NULL,
    &Registration
    );

  return Status;
}
