/** @file
  Definitinos of RedfishPlatformConfigLib

  (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_PLATFORM_CONFIG_LIB_H_
#define REDFISH_PLATFORM_CONFIG_LIB_H_

#include <Protocol/EdkIIRedfishPlatformConfig.h>

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
  );

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
  );

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
  );

/**
  Get the list of supported Redfish schema from platform configuration on the give HII handle.

  @param[in]   HiiHandle           The target handle to search. If handle is NULL,
                                   this function return all schema from HII database.
  @param[out]  SupportedSchema     The supported schema list which is separated by ';'.
                                   For example: "x-uefi-redfish-Memory.v1_7_1;x-uefi-redfish-Boot.v1_0_1"
                                   The SupportedSchema is allocated by the callee. It's caller's
                                   responsibility to free this buffer using FreePool().

  @retval EFI_SUCCESS              Schema is returned successfully.
  @retval EFI_NOT_READY            Redfish Platform Config protocol is not ready.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishPlatformConfigGetSupportedSchema (
  IN     EFI_HII_HANDLE HiiHandle, OPTIONAL
  OUT    CHAR8          **SupportedSchema
  );

#endif
