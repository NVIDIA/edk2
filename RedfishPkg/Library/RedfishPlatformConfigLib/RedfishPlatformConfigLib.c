/** @file
  Wrapper function to support Redfish Platform Config protocol.

  (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishPlatformConfigInternal.h"

REDFISH_PLATFORM_CONFIG_LIB_PRIVATE  mRedfishPlatformConfigLibPrivate;

/**
  Get Redfish value with the given Schema and Configure Language.

  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   ConfigureLang       The target value which match this configure Language.
  @param[out]  Value               The returned value.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval EFI_NOT_READY            Redfish Platform Config protocol is not ready.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishPlatformConfigGetValue (
  IN     CHAR8                *Schema,
  IN     CHAR8                *Version,
  IN     EFI_STRING           ConfigureLang,
  OUT    EDKII_REDFISH_VALUE  *Value
  )
{
  if (mRedfishPlatformConfigLibPrivate.Protocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishPlatformConfigLibPrivate.Protocol->GetValue (
                                                      mRedfishPlatformConfigLibPrivate.Protocol,
                                                      Schema,
                                                      Version,
                                                      ConfigureLang,
                                                      Value
                                                      );
}

/**
  Set Redfish value with the given Schema and Configure Language.

  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   ConfigureLang       The target value which match this configure Language.
  @param[in]   Value               The value to set.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval EFI_NOT_READY            Redfish Platform Config protocol is not ready.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishPlatformConfigSetValue (
  IN     CHAR8                *Schema,
  IN     CHAR8                *Version,
  IN     EFI_STRING           ConfigureLang,
  IN     EDKII_REDFISH_VALUE  Value
  )
{
  if (mRedfishPlatformConfigLibPrivate.Protocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishPlatformConfigLibPrivate.Protocol->SetValue (
                                                      mRedfishPlatformConfigLibPrivate.Protocol,
                                                      Schema,
                                                      Version,
                                                      ConfigureLang,
                                                      Value
                                                      );
}

/**
  Get the list of Configure Language from platform configuration by the given Schema and Pattern.

  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   Pattern             The target Configure Language pattern.
  @param[out]  RedpathList         The list of Configure Language.
  @param[out]  Count               The number of Configure Language in ConfigureLangList.

  @retval EFI_SUCCESS              ConfigureLangList is returned successfully.
  @retval EFI_NOT_READY            Redfish Platform Config protocol is not ready.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishPlatformConfigGetConfigureLang (
  IN     CHAR8       *Schema,
  IN     CHAR8       *Version,
  IN     EFI_STRING  Pattern,
  OUT    EFI_STRING  **ConfigureLangList,
  OUT    UINTN       *Count
  )
{
  if (mRedfishPlatformConfigLibPrivate.Protocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishPlatformConfigLibPrivate.Protocol->GetConfigureLang (
                                                      mRedfishPlatformConfigLibPrivate.Protocol,
                                                      Schema,
                                                      Version,
                                                      Pattern,
                                                      ConfigureLangList,
                                                      Count
                                                      );
}

/**
  Get the list of supported Redfish schema from paltform configuration on give HII handle.

  @param[in]   HiiHandle           The target handle to search. If handle is NULL,
                                   this function return all schema from HII database.
  @param[out]  SupportedSchema     The supported schema list which is separated by ';'.
                                   The SupportedSchema is allocated by the callee. It's caller's
                                   responsibility to free this buffer using FreePool().

  @retval EFI_SUCCESS              Schema is returned successfully.
  @retval EFI_NOT_READY            Redfish Platform Config protocol is not ready.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishPlatformConfigGetSupportedSchema (
  IN     EFI_HII_HANDLE  HiiHandle     OPTIONAL,
  OUT    CHAR8           **SupportedSchema
  )
{
  if (mRedfishPlatformConfigLibPrivate.Protocol == NULL) {
    return EFI_NOT_READY;
  }

  return mRedfishPlatformConfigLibPrivate.Protocol->GetSupportedSchema (
                                                      mRedfishPlatformConfigLibPrivate.Protocol,
                                                      HiiHandle,
                                                      SupportedSchema
                                                      );
}

/**
  This is a EFI_REDFISH_PLATFORM_CONFIG_PROTOCOL notification event handler.

  Install HII package notification.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
RedfishPlatformConfigProtocolInstalled (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS  Status;

  //
  // Locate EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL.
  //
  Status = gBS->LocateProtocol (
                  &gEdkIIRedfishPlatformConfigProtocolGuid,
                  NULL,
                  (VOID **)&mRedfishPlatformConfigLibPrivate.Protocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, locate EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL failure: %r\n", __FUNCTION__, Status));
    return;
  }

  gBS->CloseEvent (Event);
  mRedfishPlatformConfigLibPrivate.ProtocolEvent = NULL;
}

/**

  Create prottocol listener and wait for Redfish Platform Config protocol.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  Protocol listener is registered successfully.

**/
EFI_STATUS
EFIAPI
RedfishPlatformConfigLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  ZeroMem (&mRedfishPlatformConfigLibPrivate, sizeof (REDFISH_PLATFORM_CONFIG_LIB_PRIVATE));
  mRedfishPlatformConfigLibPrivate.ProtocolEvent = EfiCreateProtocolNotifyEvent (
                                                     &gEdkIIRedfishPlatformConfigProtocolGuid,
                                                     TPL_CALLBACK,
                                                     RedfishPlatformConfigProtocolInstalled,
                                                     NULL,
                                                     &mRedfishPlatformConfigLibPrivate.Registration
                                                     );
  if (mRedfishPlatformConfigLibPrivate.ProtocolEvent == NULL) {
    DEBUG ((DEBUG_ERROR, "%a, failed to create protocol notify event\n", __FUNCTION__));
  }

  return EFI_SUCCESS;
}

/**
  Unloads the application and its installed protocol.

  @param ImageHandle       Handle that identifies the image to be unloaded.
  @param  SystemTable      The system table.

  @retval EFI_SUCCESS      The image has been unloaded.

**/
EFI_STATUS
EFIAPI
RedfishPlatformConfigLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mRedfishPlatformConfigLibPrivate.ProtocolEvent != NULL) {
    gBS->CloseEvent (mRedfishPlatformConfigLibPrivate.ProtocolEvent);
    mRedfishPlatformConfigLibPrivate.ProtocolEvent = NULL;
  }

  mRedfishPlatformConfigLibPrivate.Protocol = NULL;

  return EFI_SUCCESS;
}
