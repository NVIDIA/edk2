/** @file
  Redfish resource identify NULL library implementation

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <RedfishBase.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/RedfishResourceIdentifyLib.h>

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
  if (!IS_EMPTY_STRING (Uri)) {
    DEBUG ((DEBUG_VERBOSE, "%a, accept resource: %s\n", __FUNCTION__, Uri));
  }

  return TRUE;
}
