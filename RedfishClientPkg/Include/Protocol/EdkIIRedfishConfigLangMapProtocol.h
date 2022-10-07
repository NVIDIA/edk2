/** @file
  This file defines the EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL interface.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_H_
#define EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_H_

typedef struct _EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL;

/**
 Definition of REDFISH_CONFIG_LANG_MAP_GET_TYPE
 **/
typedef enum {
  RedfishGetTypeUri = 0,
  RedfishGetTypeConfigLang,
  RedfishGetTypeMax
} REDFISH_CONFIG_LANG_MAP_GET_TYPE;

/**
  Get string in database by given query string.

  @param[in]   This                    Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.
  @param[in]   QueryStringType         The type of given QueryString.
  @param[in]   QueryString             Query string.
  @param[out]  ResultString            Returned string mapping to give query string.

  @retval EFI_SUCCESS                  The result is found successfully.
  @retval EFI_INVALID_PARAMETER        Invalid parameter is given.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_GET) (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *This,
  IN  REDFISH_CONFIG_LANG_MAP_GET_TYPE        QueryStringType,
  IN  EFI_STRING                              QueryString,
  OUT EFI_STRING                              *ResultString
  );

/**
  Save URI string which maps to given ConfigLang.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.
  @param[in]   ConfigLang          Config language to set
  @param[in]   Uri                 Uri which is mapping to give ConfigLang. If Uri is NULL,
                                   the record will be removed.

  @retval EFI_SUCCESS              Uri is saved successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_SET) (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL  *This,
  IN  EFI_STRING                              ConfigLang,
  IN  EFI_STRING                              Uri        OPTIONAL
  );

/**
  Refresh the resource map database and save database to variable.

  @param[in]   This                Pointer to EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL instance.

  @retval EFI_SUCCESS              database is saved successfully.
  @retval Others                   Some error happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_FLUSH) (
  IN  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL    *This
  );

struct _EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL {
  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_GET   Get;
  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_SET   Set;
  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL_FLUSH Flush;
};

extern EFI_GUID gEdkIIRedfishConfigLangMapProtocolGuid;

#endif
