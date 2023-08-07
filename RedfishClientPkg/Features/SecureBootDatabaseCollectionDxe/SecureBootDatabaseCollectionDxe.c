/** @file

  Redfish feature driver implementation - SecureBootDatabaseCollection

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SecureBootDatabaseCollectionDxe.h"

REDFISH_COLLECTION_PRIVATE  *mRedfishCollectionPrivate = NULL;

/**
  This function handle the member instance in collection member array.

  @param[in]     Private           Pointer to private instance
  @param[in]     Uri               The secure boot database URI.

  @retval EFI_SUCCESS              Secure boot database resource is processed.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
HandleResource (
  IN  REDFISH_COLLECTION_PRIVATE  *Private,
  IN  EFI_STRING                  Uri
  )
{
  EFI_STATUS           Status;
  REDFISH_SCHEMA_INFO  SchemaInfo;

  if ((Private == NULL) || IS_EMPTY_STRING (Uri)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: process resource for: %s\n", __func__, Uri));

  Status = GetRedfishSchemaInfo (Private->RedfishService, Private->JsonStructProtocol, Uri, &SchemaInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to get schema information from: %s %r\n", __func__, Uri, Status));
    return Status;
  }

  //
  // The target property does not exist, do the provision to create property.
  //
  DEBUG ((REDFISH_DEBUG_TRACE, "%a provision for %s\n", __func__, Uri));
  Status = EdkIIRedfishResourceConfigProvisionging (&SchemaInfo, Uri, Private->InformationExchange, FALSE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to provision with GET mode: %r\n", __func__, Status));
  }

  return Status;
}

/**
  This function handle secure boot databases collection and find each invidual
  secure boot database instance.

  @param[in]     Private           Pointer to private instance

  @retval EFI_SUCCESS              Secure boot databases are processed.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
HandleCollectionResource (
  IN  REDFISH_COLLECTION_PRIVATE  *Private
  )
{
  EFI_STATUS                                   Status;
  EFI_REDFISH_SECUREBOOTDATABASECOLLECTION     *Collection;
  EFI_REDFISH_SECUREBOOTDATABASECOLLECTION_CS  *CollectionCs;
  RedfishCS_Link                               *List;
  RedfishCS_Header                             *Header;
  RedfishCS_Type_Uri_Data                      *UriData;
  EFI_STRING                                   MemberUri;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Private->JsonStructProtocol == NULL) || (Private->CollectionJson == NULL)) {
    return EFI_NOT_READY;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: process collection for: %s\n", __func__, Private->CollectionUri));

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

  CollectionCs = Collection->SecureBootDatabaseCollection;

  if (*CollectionCs->Membersodata_count == 0) {
    return EFI_NOT_FOUND;
  }

  if (IsLinkEmpty (&CollectionCs->Members)) {
    return EFI_NOT_FOUND;
  }

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
          DEBUG ((DEBUG_ERROR, "%a: process resource: %a failed: %r\n", __func__, UriData->Uri, Status));
        }

        FreePool (MemberUri);
      }
    }

    if (IsLinkAtEnd (&CollectionCs->Members, List)) {
      break;
    }

    List = GetNextLink (&CollectionCs->Members, List);
  }

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (Private->JsonStructProtocol, (EFI_REST_JSON_STRUCTURE_HEADER *)Collection);

  return EFI_SUCCESS;
}

/**
  This function release collection resource.

  @param[in]     Private           Pointer to private instance

  @retval EFI_SUCCESS              Collection resource is released.
  @retval Others                   Some errors happened.

**/
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
    RedfishFreeResponse (
      Private->Response.StatusCode,
      Private->Response.HeaderCount,
      Private->Response.Headers,
      Private->Response.Payload
      );
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

/**
  The callback handler to handle Redfish collection.

  @param[in]     Private             Pointer to private instance

  @retval EFI_SUCCESS              Collection is processed.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
CollectionHandler (
  IN REDFISH_COLLECTION_PRIVATE  *Private
  )
{
  EFI_STATUS  Status;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: collection handler for %s\n", __func__, Private->CollectionUri));

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
    DEBUG ((DEBUG_ERROR, "%a: handle collection resource failed: %r\n", __func__, Private->CollectionUri, Status));
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

  Private        = (REDFISH_COLLECTION_PRIVATE *)Context;
  ResourceUri    = NULL;
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
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_RELEASE;
  }

  Status = CollectionHandler (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CollectionHandler failure: %r\n", __func__, Status));
  }

ON_RELEASE:

  if (ResourceUri != NULL) {
    FreePool (ResourceUri);
  }

  if (Private->CollectionUri != NULL) {
    FreePool (Private->CollectionUri);
  }

  return Status;
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
    return;
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
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to install redfish config handler protocol: %r\n", __func__, Status));
  }

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
