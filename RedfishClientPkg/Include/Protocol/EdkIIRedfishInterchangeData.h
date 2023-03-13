/** @file
  This file defines the EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL interface.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_H_
#define EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_H_

//
// Definition of REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG
//
typedef struct {
  UINTN         Index;
  EFI_STRING    ConfigureLang;
} REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG;

//
// Definition of REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST
//
typedef struct {
  UINTN                                     Count;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG    *List;
} REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST;

typedef struct _EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL;

#define EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL_GUID \
    {  \
      0x4B8FF71C, 0x4A7B, 0x9478, { 0xB7, 0x81, 0x35, 0x9B, 0x0A, 0xF2, 0x00, 0x91 }  \
    }

typedef enum {
  InformationTypeNone = 0,                       ///< Invalid information.
  InformationTypeCollectionMemberUri,            ///< URI to the new created collection member.
  InformationTypeCollectionMemberConfigLanguage, ///< URI to the new created collection member.
  InformationTypeMax
} RESOURCE_INFORMATION_EXCHANGE_TYPE;

typedef struct {
  RESOURCE_INFORMATION_EXCHANGE_TYPE    Type;
  EFI_STRING                            ParentUri;    ///< The parent URI (in configure language) of the resource to process.
  EFI_STRING                            PropertyName; ///< The property name of the resource to process.
  EFI_STRING                            FullUri;      ///< The full URI (in configure language) of the resource to process.
} RESOURCE_INFORMATION_SEND;

typedef struct {
  RESOURCE_INFORMATION_EXCHANGE_TYPE             Type;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST    ConfigureLanguageList;
} RESOURCE_INFORMATION_RETURNED;

typedef struct {
  RESOURCE_INFORMATION_SEND        SendInformation;
  RESOURCE_INFORMATION_RETURNED    ReturnedInformation;
} RESOURCE_INFORMATION_EXCHANGE;

struct _EDKII_REDFISH_FEATURE_INTERCHANGE_DATA_PROTOCOL {
  RESOURCE_INFORMATION_EXCHANGE    *ResourceInformationExchage;
};

extern EFI_GUID  gEdkIIRedfishFeatureInterchangeDataProtocolGuid;

#endif
