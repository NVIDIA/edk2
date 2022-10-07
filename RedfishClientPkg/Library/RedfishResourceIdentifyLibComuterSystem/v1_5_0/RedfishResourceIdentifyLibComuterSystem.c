/** @file
  Redfish resource identify library implementation for computer system version 1.5.0

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <RedfishBase.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/NetLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RedfishResourceIdentifyLib.h>

#include <Protocol/RestJsonStructure.h>

#include <RedfishJsonStructure/ComputerSystem/v1_5_0/EfiComputerSystemV1_5_0.h>

EFI_REST_JSON_STRUCTURE_PROTOCOL  *mJsonStructProtocol = NULL;

/**
  Identify resource from given URI and context in JSON format

  @param[in]   Uri    URI of given Redfish resource
  @param[in]   Json   Context in JSON format of give Redfish resource

  @retval TRUE        This is the Redfish resource that we have to handle.
  @retval FALSE       We don't handle this Redfish resource.

**/
BOOLEAN
RedfishIdentifyResource (
  IN     EFI_STRING Uri,
  IN     CHAR8      *Json
  )
{
  EFI_STATUS                            Status;
  EFI_REDFISH_COMPUTERSYSTEM_V1_5_0     *ComputerSystem;
  EFI_REDFISH_COMPUTERSYSTEM_V1_5_0_CS  *ComputerSystemCs;
  EFI_GUID                              SystemUuid;
  EFI_GUID                              ResourceUuid;

  if (IS_EMPTY_STRING (Uri) || IS_EMPTY_STRING (Json)) {
    return FALSE;
  }

  if (mJsonStructProtocol == NULL) {
    return FALSE;
  }

  ComputerSystem = NULL;
  ComputerSystemCs = NULL;

  Status = mJsonStructProtocol->ToStructure (
                                  mJsonStructProtocol,
                                  NULL,
                                  Json,
                                  (EFI_REST_JSON_STRUCTURE_HEADER **)&ComputerSystem
                                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, ToStructure() failed: %r\n", __FUNCTION__, Status));
    return FALSE;
  }

  ComputerSystemCs = ComputerSystem->ComputerSystem;

  if (IS_EMPTY_STRING (ComputerSystemCs->UUID)) {
    return FALSE;
  }

  Status = AsciiStrToGuid (ComputerSystemCs->UUID, &ResourceUuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, fail to get resource UUID: %r\n", __FUNCTION__, Status));
    return FALSE;
  }

  Status = NetLibGetSystemGuid (&SystemUuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, fail to get system UUID from SMBIOS: %r\n", __FUNCTION__, Status));
    return FALSE;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a, Identify: System: %g Resource: %g\n", __FUNCTION__, &SystemUuid, &ResourceUuid));
  if (CompareGuid (&ResourceUuid, &SystemUuid)) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  }

  mJsonStructProtocol->DestoryStructure (
                        mJsonStructProtocol,
                        (EFI_REST_JSON_STRUCTURE_HEADER *)ComputerSystem
                        );

  return (Status == EFI_SUCCESS ? TRUE : FALSE);
}

/**
  Callback function when gEfiRestJsonStructureProtocolGuid is installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
RestJasonStructureProtocolIsReady
 (
  IN  EFI_EVENT                             Event,
  IN  VOID                                  *Context
  )
{
  EFI_STATUS  Status;

  if (mJsonStructProtocol != NULL) {
    return;
  }

  Status = gBS->LocateProtocol (
                 &gEfiRestJsonStructureProtocolGuid,
                 NULL,
                 (VOID **)&mJsonStructProtocol
                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, failed to locate gEfiRestJsonStructureProtocolGuid: %r\n", __FUNCTION__, Status));
  }

  gBS->CloseEvent (Event);
}

/**

  Install JSON protocol notification

  @param[in] ImageHandle     The image handle.
  @param[in] SystemTable     The system table.

  @retval  EFI_SUCEESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
RedfishResourceIdentifyComuterSystemConstructor (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  VOID  *Registration;

  EfiCreateProtocolNotifyEvent (
    &gEfiRestJsonStructureProtocolGuid,
    TPL_CALLBACK,
    RestJasonStructureProtocolIsReady,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}
