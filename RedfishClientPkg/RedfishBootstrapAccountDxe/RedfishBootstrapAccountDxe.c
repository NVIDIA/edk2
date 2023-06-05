/** @file
  This driver deletes bootstrap account in BMC after BIOS Redfish finished
  all jobs.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishBootstrapAccountDxe.h"

REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE  *mBootstrapPrivate = NULL;

/**
  Callback function executed when the AfterProvisioning event group is signaled.

  @param[in]   Event    Event whose notification function is being invoked.
  @param[out]  Context  Pointer to the Context buffer

**/
VOID
EFIAPI
RedfishBootstrapAccountOnRedfishAfterProvisioning (
  IN  EFI_EVENT  Event,
  OUT VOID       *Context
  )
{
  EFI_STATUS                         Status;
  REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE  *Private;
  EDKII_REDFISH_CREDENTIAL_PROTOCOL  *credentialProtocol;
  EDKII_REDFISH_AUTH_METHOD          AuthMethod;
  CHAR8                              *AccountName;
  CHAR8                              *AccountCredential;
  CHAR16                             TargetUri[REDFISH_URI_LENGTH];
  CHAR16                             *RedfishVersion;
  REDFISH_RESPONSE                   RedfishResponse;

  RedfishVersion = NULL;

  Private = (REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE *)Context;
  if ((Private == NULL) || (Private->RedfishService == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Redfish service is not available\n", __func__));
    return;
  }

  //
  // Locate Redfish Credential Protocol to get credential for
  // accessing to Redfish service.
  //
  Status = gBS->LocateProtocol (
                  &gEdkIIRedfishCredentialProtocolGuid,
                  NULL,
                  (VOID **)&credentialProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((REDFISH_BOOTSTRAP_ACCOUNT_DEBUG, "%a: No Redfish Credential Protocol is installed on system.", __func__));
    return;
  }

  Status = credentialProtocol->GetAuthInfo (
                                 credentialProtocol,
                                 &AuthMethod,
                                 &AccountName,
                                 &AccountCredential
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not get bootstrap account information: %r\n", __func__, Status));
    return;
  }

  //
  // Carving the URI
  //
  RedfishVersion = RedfishGetVersion (Private->RedfishService);
  if (RedfishVersion == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: can not get Redfish version\n", __func__));
    return;
  }

  UnicodeSPrint (TargetUri, (sizeof (CHAR16) * REDFISH_URI_LENGTH), L"%s%s/%a", RedfishVersion, REDFISH_MANAGER_ACCOUNT_COLLECTION_URI, AccountName);

  DEBUG ((REDFISH_BOOTSTRAP_ACCOUNT_DEBUG, "%a: bootstrap account:    %a\n", __func__, AccountName));
  DEBUG ((REDFISH_BOOTSTRAP_ACCOUNT_DEBUG, "%a: bootstrap credential: %a\n", __func__, AccountCredential));
  DEBUG ((REDFISH_BOOTSTRAP_ACCOUNT_DEBUG, "%a: bootstrap URI:        %s\n", __func__, TargetUri));

  //
  // Remove bootstrap account at /redfish/v1/AccountService/Account
  //
  ZeroMem (&RedfishResponse, sizeof (REDFISH_RESPONSE));
  Status = DeleteResourceByUri (
             Private->RedfishService,
             TargetUri,
             &RedfishResponse
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not remove bootstrap account at BMC: %r", __func__, Status));
    DumpRedfishResponse (__func__, DEBUG_ERROR, &RedfishResponse);
    if (RedfishResponse.Payload != NULL) {
      RedfishFreeResponse (
        RedfishResponse.StatusCode,
        0,
        NULL,
        RedfishResponse.Payload
        );
    }
  } else {
    DEBUG ((REDFISH_BOOTSTRAP_ACCOUNT_DEBUG, "%a: bootstrap account: %a is removed from: %s\n", __func__, AccountName, REDFISH_MANAGER_ACCOUNT_COLLECTION_URI));
  }

  //
  // Clean credential
  //
  ZeroMem (AccountName, AsciiStrSize (AccountName));
  ZeroMem (AccountCredential, AsciiStrSize (AccountCredential));

  return;
}

/**
  Initialize a Redfish configure handler.

  This function will be called by the Redfish config driver to initialize each Redfish configure
  handler.

  @param[in]   This                     Pointer to EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL instance.
  @param[in]   RedfishConfigServiceInfo Redfish service informaiton.

  @retval EFI_SUCCESS                  The handler has been initialized successfully.
  @retval EFI_DEVICE_ERROR             Failed to create or configure the REST EX protocol instance.
  @retval EFI_ALREADY_STARTED          This handler has already been initialized.
  @retval Other                        Error happens during the initialization.

**/
EFI_STATUS
EFIAPI
RedfishBootstrapAccountInit (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This,
  IN  REDFISH_CONFIG_SERVICE_INFORMATION     *RedfishConfigServiceInfo
  )
{
  REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE  *Private;

  Private = REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE_FROM_PROTOCOL (This);

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
RedfishBootstrapAccountStop (
  IN  EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  *This
  )
{
  REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE  *Private;

  Private = REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE_FROM_PROTOCOL (This);

  if (Private->RedfishService != NULL) {
    RedfishCleanupService (Private->RedfishService);
    Private->RedfishService = NULL;
  }

  return EFI_SUCCESS;
}

EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL  mRedfishConfigHandler = {
  RedfishBootstrapAccountInit,
  RedfishBootstrapAccountStop
};

/**
  Unloads an image.

  @param[in]  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
RedfishBootstrapAccountUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;

  if (mBootstrapPrivate == NULL) {
    return EFI_SUCCESS;
  }

  if (mBootstrapPrivate->RedfishEvent != NULL) {
    gBS->CloseEvent (mBootstrapPrivate->RedfishEvent);
  }

  Status = gBS->UninstallProtocolInterface (
                  mBootstrapPrivate->ImageHandle,
                  &gEdkIIRedfishETagProtocolGuid,
                  (VOID *)&mBootstrapPrivate->Protocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not uninstall Redfish config handler protocol: %r\n", __func__, Status));
  }

  FreePool (mBootstrapPrivate);
  mBootstrapPrivate = NULL;

  return EFI_SUCCESS;
}

/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
RedfishBootstrapAccountEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  if (mBootstrapPrivate != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mBootstrapPrivate = AllocateZeroPool (sizeof (REDFISH_BOOTSTRAP_ACCOUNT_PRIVATE));
  if (mBootstrapPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (&mBootstrapPrivate->Protocol, &mRedfishConfigHandler, sizeof (EDKII_REDFISH_CONFIG_HANDLER_PROTOCOL));
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEdkIIRedfishConfigHandlerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mBootstrapPrivate->Protocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: can not install Redfish config handler protocol: %r\n", __func__, Status));
    goto ON_ERROR;
  }

  //
  // Register after provisioning event to remove bootstrap account.
  //
  Status = CreateAfterProvisioningEvent (
             RedfishBootstrapAccountOnRedfishAfterProvisioning,
             (VOID *)mBootstrapPrivate,
             &mBootstrapPrivate->RedfishEvent
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to register after-provisioning event: %r\n", __func__, Status));
    goto ON_ERROR;
  }

  return EFI_SUCCESS;

ON_ERROR:

  RedfishBootstrapAccountUnload (ImageHandle);

  return Status;
}
