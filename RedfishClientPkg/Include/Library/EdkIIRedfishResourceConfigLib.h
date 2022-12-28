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
#include <Protocol/EdkIIRedfishInterchangeData.h>

/**
  Provising redfish resource by given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 Target URI to create resource.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.
  @param[in]   HttpPostMode        TRUE if resource does not exist, HTTP POST method is used.
                                   FALSE if the resource exist but some of properties are missing,
                                   HTTP PUT method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceConfigProvisionging (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchange,
  IN     BOOLEAN                        HttpPostMode
  );

/**
  Consume resource from given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceConfigConsume (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri
  );

/**
  Update resource to given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceConfigUpdate (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri
  );

/**
  Check resource on given URI.

  @param[in]   Uri                 The target URI to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceConfigCheck (
  IN     REDFISH_SCHEMA_INFO  *Schema,
  IN     EFI_STRING           Uri
  );

/**
  Identify resource on given URI.

  @param[in]   Schema              Redfish schema information.
  @param[in]   Uri                 The target URI to consume.
  @param[in]   InformationExchange Pointer to RESOURCE_INFORMATION_EXCHANGE.

  @retval EFI_SUCCESS              This is target resource which we want to handle.
  @retval EFI_UNSUPPORTED          This is not the target resource.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceConfigIdentify (
  IN     REDFISH_SCHEMA_INFO            *Schema,
  IN     EFI_STRING                     Uri,
  IN     RESOURCE_INFORMATION_EXCHANGE  *InformationExchangeUri
  );

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ConfigLangList  Pointer to REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceSetConfigureLang (
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  *ConfigLangList
  );

/**
  Set Configure language of this resource in the
  RESOURCE_INFORMATION_EXCHANGE structure.

  @param[in]   ConfigLangString    Configure language string.
  @param[in]   Index               Index value of configure language string.

  @retval EFI_SUCCESS              Configure language is set.
  @retval EFI_UNSUPPORTED          EdkIIRedfishFeatureInterchangeDataProtocol is not found.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EdkIIRedfishResourceSetConfigureLangString (
  IN EFI_STRING  ConfigLangString,
  IN UINTN       Index
  );

/**

  Get schema information by given protocol and service instance.

  @param[in]  RedfishService      Pointer to Redfish service instance.
  @param[in]  JsonStructProtocol  Json Structure protocol instance.
  @param[in]  Uri                 Target URI.
  @param[out] SchemaInfo          Returned schema information.

  @retval     EFI_SUCCESS         Schema information is returned successfully.
  @retval     Others              Errors occur.

**/
EFI_STATUS
GetRedfishSchemaInfo (
  IN  REDFISH_SERVICE                   *RedfishService,
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL  *JsonStructProtocol,
  IN  EFI_STRING                        Uri,
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
GetSupportedSchemaVersion (
  IN   CHAR8                *Schema,
  OUT  REDFISH_SCHEMA_INFO  *SchemaInfo
  );

#endif
