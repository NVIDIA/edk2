//
// Auto-generated file by Redfish Schema C Structure Generator.
// https://github.com/DMTF/Redfish-Schema-C-Struct-Generator.
//
//  (C) Copyright 2019-2021 Hewlett Packard Enterprise Development LP<BR>
//  SPDX-License-Identifier: BSD-2-Clause-Patent
//
// Auto-generated file by Redfish Schema C Structure Generator.
// https://github.com/DMTF/Redfish-Schema-C-Struct-Generator.
// Copyright Notice:
// Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
// License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
//

#ifndef EFI_REDFISHATTRIBUTEREGISTRY_V1_1_10_CSTRUCT_H_
#define EFI_REDFISHATTRIBUTEREGISTRY_V1_1_10_CSTRUCT_H_

#include "../../../include/RedfishDataTypeDef.h"
#include "../../../include/Redfish_AttributeRegistry_v1_1_10_CS.h"

typedef RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS EFI_REDFISH_ATTRIBUTEREGISTRY_V1_1_10_CS;

RedfishCS_status
Json_AttributeRegistry_V1_1_10_To_CS (RedfishCS_char *JsonRawText, EFI_REDFISH_ATTRIBUTEREGISTRY_V1_1_10_CS **ReturnedCs);

RedfishCS_status
CS_To_AttributeRegistry_V1_1_10_JSON (EFI_REDFISH_ATTRIBUTEREGISTRY_V1_1_10_CS *CSPtr, RedfishCS_char **JsonText);

RedfishCS_status
DestroyAttributeRegistry_V1_1_10_CS (EFI_REDFISH_ATTRIBUTEREGISTRY_V1_1_10_CS *CSPtr);

RedfishCS_status
DestroyAttributeRegistry_V1_1_10_Json (RedfishCS_char *JsonText);

#endif
