/** @file
  This file defines the Redfish version Library interface.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_VERSION_LIB_H_
#define REDFISH_VERSION_LIB_H_

/**
  Query HTTP request to BMC with given redfish service and return redfish
  version information. If there is troulbe to get Redfish version on BMC,
  The value of PcdDefaultRedfishVersion is returned.

  It's call responsibility to release returned buffer.

  @param[in]   Service  Redfish service instance

  @retval EFI_STRING  Redfish version string. NULL while error occurs.

**/
EFI_STRING
RedfishGetVersion (
  IN REDFISH_SERVICE  *Service
  );

#endif
