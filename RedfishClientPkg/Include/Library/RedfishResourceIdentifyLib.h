/** @file
  This file defines the Redfish resource identify Library interface.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_RESOURCE_IDENTIFY_LIB_H_
#define REDFISH_RESOURCE_IDENTIFY_LIB_H_

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
  );

#endif
