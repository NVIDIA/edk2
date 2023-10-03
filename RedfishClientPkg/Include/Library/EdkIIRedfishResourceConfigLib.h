/** @file
  This file defines the EDKII resource config Library interface.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2022-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_RESOURCE_CONFIG_LIB_H_
#define EDKII_REDFISH_RESOURCE_CONFIG_LIB_H_

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/RedfishLib.h>
#include <Protocol/RestJsonStructure.h>
#include <Protocol/EdkIIRedfishResourceConfigProtocol.h>
#include <Protocol/EdkIIRedfishResourceConfig2Protocol.h>
#include <Protocol/EdkIIRedfishInterchangeData.h>

/**
  Provision redfish resource by given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 Target URI to create resource.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.
  @param[in]   HttpPostMode        TRUE if resource does not exist, HTTP POST method is used.
                                   FALSE if the resource exist but some of properties are missing,
                                   HTTP PUT method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigProvisioning (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     CHAR8                          *JsonText OPTIONAL,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchange,
  IN     BOOLEAN                        HttpPostMode
  );

/**
  Consume resource from given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigConsume (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  );

/**
  Update resource to given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigUpdate (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  );

/**
  Check resource on given URI.

  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigCheck (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri,
  IN     CHAR8                *JsonText OPTIONAL
  );

/**
  Identify resource on given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   JsonText            The JSON data in ASCII string format. This is optional.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.

  @retval EFI_SUCCESS              This is target resource which we want to handle.
  @retval EFI_UNSUPPORTED          This is not the target resource.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceConfigIdentify (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     CHAR8                          *JsonText OPTIONAL,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchange
  );

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ImageHandle     Pointer to image handle.
  @param[in]   ConfigLangList  Pointer to REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceSetConfigureLang (
  IN EFI_HANDLE                                   ImageHandle,
  IN REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *ConfigLangList
  );

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ImageHandle         Pointer to image handle.
  @param[in]   ConfigLangString    Configure language string.
  @param[in]   Index               Index value of configure language string.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
EdkIIRedfishResourceSetConfigureLangString (
  IN EFI_HANDLE  ImageHandle,
  IN EFI_STRING  ConfigLangString,
  IN UINTN       Index
  );

/**

  Get schema information by given protocol and service instance if JsonText
  is NULL or empty. When JsonText is provided by caller, this function read
  schema information from JsonText.

  @param[in]  RedfishService      Pointer to Redfish service instance.
  @param[in]  JsonStructProtocol  Json Structure protocol instance.
  @param[in]  Uri                 Target URI.
  @param[in]  JsonText            Redfish data in JSON format. This is optional.
  @param[out] SchemaInfo          Returned schema information.

  @retval     EFI_SUCCESS         Schema information is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
EFIAPI
GetRedfishSchemaInfo (
  IN  REDFISH_SERVICE                   *RedfishService,
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL  *JsonStructProtocol,
  IN  EFI_STRING                        Uri,
  IN  CHAR8                             *JsonText OPTIONAL,
  OUT REDFISH_SCHEMA_INFO               *SchemaInfo
  );

/**

  Get supported schema list by given specify schema name.

  @param[in]  Schema      Schema type name.
  @param[out] SchemaInfo  Returned schema information.

  @retval     EFI_SUCCESS         Schema information is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
EFIAPI
GetSupportedSchemaVersion (
  IN   CHAR8                *Schema,
  OUT  REDFISH_SCHEMA_INFO  *SchemaInfo
  );

#endif
