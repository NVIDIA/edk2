/** @file
  This file defines the EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL interface.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_PLATFORM_CONFIG_H_
#define EDKII_REDFISH_PLATFORM_CONFIG_H_

typedef struct _EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL;

/**
  Definition of EDKII_REDFISH_TYPE_VALUE
 **/
typedef union {
  INT64      Integer;
  BOOLEAN    Boolean;
  CHAR8      *Buffer;
  CHAR8      **StringArray;
  INT64      *IntegerArray;
  BOOLEAN    *BooleanArray;
} EDKII_REDFISH_TYPE_VALUE;

/**
  Definition of EDKII_REDFISH_VALUE_TYPES
 **/
typedef enum {
  REDFISH_VALUE_TYPE_UNKNOWN = 0,
  REDFISH_VALUE_TYPE_INTEGER,
  REDFISH_VALUE_TYPE_BOOLEAN,
  REDFISH_VALUE_TYPE_STRING,
  REDFISH_VALUE_TYPE_STRING_ARRAY,
  REDFISH_VALUE_TYPE_INTEGER_ARRAY,
  REDFISH_VALUE_TYPE_BOOLEAN_ARRAY,
  REDFISH_VALUE_TYPE_MAX
} EDKII_REDFISH_VALUE_TYPES;

/**
  Definition of EDKII_REDFISH_VALUE
 **/
typedef struct {
  EDKII_REDFISH_VALUE_TYPES    Type;
  EDKII_REDFISH_TYPE_VALUE     Value;
  UINTN                        ArrayCount;
} EDKII_REDFISH_VALUE;

/**
  Get Redfish value with the given Schema and Configure Language.

  @param[in]   This                Pointer to EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL instance.
  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   ConfigureLang       The target value which match this configure Language.
  @param[out]  Value               The returned value.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_PLATFORM_CONFIG_GET_VALUE)(
  IN     EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL *This,
  IN     CHAR8                                  *Schema,
  IN     CHAR8                                  *Version,
  IN     EFI_STRING                             ConfigureLang,
  OUT    EDKII_REDFISH_VALUE                    *Value
  );

/**
  Set Redfish value with the given Schema and Configure Language.

  @param[in]   This                Pointer to EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL instance.
  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   ConfigureLang       The target value which match this configure Language.
  @param[in]   Value               The value to set.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_PLATFORM_CONFIG_SET_VALUE)(
  IN     EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL *This,
  IN     CHAR8                                  *Schema,
  IN     CHAR8                                  *Version,
  IN     EFI_STRING                             ConfigureLang,
  IN     EDKII_REDFISH_VALUE                    Value
  );

/**
  Get the list of Configure Language from platform configuration by the given Schema and RegexPattern.

  @param[in]   This                Pointer to EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL instance.
  @param[in]   Schema              The Redfish schema to query.
  @param[in]   Version             The Redfish version to query.
  @param[in]   RegexPattern        The target Configure Language pattern. This is used for regular expression matching.
  @param[out]  ConfigureLangList   The list of Configure Language.
  @param[out]  Count               The number of Configure Language in ConfigureLangList.

  @retval EFI_SUCCESS              ConfigureLangList is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_PLATFORM_CONFIG_GET_CONFIG_LANG)(
  IN     EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL *This,
  IN     CHAR8                                  *Schema,
  IN     CHAR8                                  *Version,
  IN     EFI_STRING                             RegexPattern,
  OUT    EFI_STRING                             **ConfigureLangList,
  OUT    UINTN                                  *Count
  );

/**
  Get the list of supported Redfish schema from platform configuration on the give HII handle.

  @param[in]   This                Pointer to EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL instance.
  @param[in]   HiiHandle           The target handle to search. If handle is NULL,
                                   this function returns all schema from HII database.
  @param[out]  SupportedSchema     The supported schema list which is separated by ';'.
                                   For example: "x-uefi-redfish-Memory.v1_7_1;x-uefi-redfish-Boot.v1_0_1"
                                   The SupportedSchema is allocated by the callee. It's caller's
                                   responsibility to free this buffer using FreePool().

  @retval EFI_SUCCESS              Schema is returned successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_PLATFORM_CONFIG_GET_SUPPORTED_SCHEMA)(
  IN     EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL    *This,
  IN     EFI_HII_HANDLE                            HiiHandle, OPTIONAL
  OUT    CHAR8                                     **SupportedSchema
  );

struct _EDKII_REDFISH_PLATFORM_CONFIG_PROTOCOL {
  EDKII_REDFISH_PLATFORM_CONFIG_GET_VALUE               GetValue;
  EDKII_REDFISH_PLATFORM_CONFIG_SET_VALUE               SetValue;
  EDKII_REDFISH_PLATFORM_CONFIG_GET_CONFIG_LANG         GetConfigureLang;
  EDKII_REDFISH_PLATFORM_CONFIG_GET_SUPPORTED_SCHEMA    GetSupportedSchema;
};

extern EFI_GUID  gEdkIIRedfishPlatformConfigProtocolGuid;

#endif
