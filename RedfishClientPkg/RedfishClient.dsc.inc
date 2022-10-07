## @file
# Redfish Client DSC include file for Platform DSC
#
# This file includes all required information to enable Redfish features.
# It can be included to a platform DSC file by using "!include RedfishClientPkg/RedfishClient.dsc.inc".
#
# (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  !include RedfishClientPkg/RedfishClientDefines.dsc.inc

[LibraryClasses]
  !include RedfishClientPkg/RedfishClientLibs.dsc.inc

[Components]
  !include RedfishClientPkg/RedfishClientComponents.dsc.inc

