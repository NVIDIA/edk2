## @file
# RedfishClientPkg DSC include file for [Defines] section of all Architectures.
#
# This file can be included to the [Defines] section of a platform DSC file by
# using "!include RedfishClientPkg/RedfishClientDefines.dsc.inc" to set value of
# flags.
#
# These flags can be defined before the !include line, or changed on the command
# line to enable or disable related feature support.
#   -D FLAG=VALUE
#
# The default value of these flags are:
#   DEFINE REDFISH_CLIENT = TRUE
#
# (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>
#
#    SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

!ifndef REDFISH_CLIENT
  #
  # This flag is to enable or disable the EDK2 Redfish client support.
  #
  DEFINE REDFISH_CLIENT = TRUE
!endif

!ifndef REDFISH_CLIENT_ALL_AUTOGENED
  #
  # This flag is to enable or disable the EDK2 Redfish client support.
  #
  DEFINE REDFISH_CLIENT_ALL_AUTOGENED = FALSE
!endif

