//
// Auto-generated file by Redfish Schema C Structure Generator.
// https://github.com/DMTF/Redfish-Schema-C-Struct-Generator.
//
//  (C) Copyright 2019-2022 Hewlett Packard Enterprise Development LP<BR>
//
// Copyright Notice:
// Copyright 2019-2022 Distributed Management Task Force, Inc. All rights reserved.
// License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
//

#include"Redfish_AttributeRegistry_v1_3_6_CS.h"
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

RedfishCS_bool SupportedRedfishResource (RedfishCS_char *Odata_Type, RedfishCS_char *NameSpace, RedfishCS_char *Version, RedfishCS_char *DataType);
RedfishCS_status CreateCsUriByNode (RedfishCS_void *Cs, json_t *JsonOj, RedfishCS_char *NodeName, RedfishCS_char *ParentUri, RedfishCS_Type_Uri_Data **CsTypeUriData);
RedfishCS_status CreateCsJsonByNode (RedfishCS_void *Cs, json_t *JsonOj, RedfishCS_char *NodeName, RedfishCS_char *ParentUri, RedfishCS_Type_JSON_Data **CsTypeJsonData);
RedfishCS_status CreateCsUriOrJsonByNode (RedfishCS_void *Cs, json_t *JsonObj, RedfishCS_char *NodeName, RedfishCS_char *ParentUri, RedfishCS_Link *LinkHead);
RedfishCS_status CreateCsUriOrJsonByNodeArray (RedfishCS_void *Cs, json_t *JsonObj, RedfishCS_char *NodeName, RedfishCS_char *ParentUri, RedfishCS_Link *LinkHead);
RedfishCS_status CreateJsonPayloadAndCs (char *JsonRawText, char *ResourceType, char *ResourceVersion, char *TypeName, json_t **JsonObjReturned, void **Cs, int size);
RedfishCS_status GetRedfishPropertyStr (RedfishCS_void *Cs, json_t *JsonObj, char *Key, RedfishCS_char **DstBuffer);
RedfishCS_status GetRedfishPropertyBoolean (RedfishCS_void *Cs, json_t *JsonObj, char *Key, RedfishCS_bool **DstBuffer);
RedfishCS_status GetRedfishPropertyVague (RedfishCS_void *Cs, json_t *JsonObj, char *Key, RedfishCS_Vague **DstBuffer);
RedfishCS_status DestoryCsMemory (RedfishCS_void *rootCs);
RedfishCS_status GetRedfishPropertyInt64 (RedfishCS_void *Cs, json_t *JsonObj, char *Key, RedfishCS_int64 **Dst);
RedfishCS_status InsertJsonStringObj (json_t *JsonObj, char *Key, char *StringValue);
RedfishCS_status InsertJsonLinkObj (json_t *JsonObj, char *Key, RedfishCS_Link *Link);
RedfishCS_status InsertJsonInt64Obj (json_t *ParentJsonObj, char *Key, RedfishCS_int64 *Int64Value);
RedfishCS_status InsertJsonBoolObj (json_t *ParentJsonObj, char *Key, RedfishCS_bool *BoolValue);
RedfishCS_status InsertJsonStringArrayObj (json_t *JsonObj, char *Key, RedfishCS_char_Array *StringValueArray);
RedfishCS_status InsertJsonLinkArrayObj (json_t *JsonObj, char *Key, RedfishCS_Link *LinkArray);
RedfishCS_status InsertJsonInt64ArrayObj (json_t *ParentJsonObj, char *Key, RedfishCS_int64_Array *Int64ValueArray);
RedfishCS_status InsertJsonBoolArrayObj (json_t *ParentJsonObj, char *Key, RedfishCS_bool_Array *BoolValueArray);
RedfishCS_status InsertJsonVagueObj (json_t *ParentJsonObj, char *Key, RedfishCS_Vague *VagueValue);
RedfishCS_bool CheckEmptyPropJsonObject(json_t *JsonObj, RedfishCS_uint32 *NumOfProperty);
RedfishCS_status CreateEmptyPropCsJson(RedfishCS_void *Cs, json_t *JsonOj, RedfishCS_char *NodeName, RedfishCS_char *ParentUri, RedfishCS_Type_EmptyProp_CS_Data **CsTypeEmptyPropCS, RedfishCS_uint32 NunmOfProperties);
RedfishCS_status CsEmptyPropLinkToJson(json_t *CsJson, char *Key, RedfishCS_Link *Link);

static RedfishCS_status GenOemCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishResource_Oem_CS **Dst);
//
//Generate C structure for Oem
//
static RedfishCS_status GenOemActionsCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_OemActions_CS **Dst)
{
  RedfishCS_status Status;
  RedfishCS_Type_JSON_Data *CsTypeJson;
  RedfishCS_Type_EmptyProp_CS_Data *CsTypeEmptyPropCS;
  RedfishCS_uint32 NunmOfEmptyPropProperties;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_OemActions_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  InitializeLinkHead (&(*Dst)->Prop);

  //
  // Try to create C structure if the property is
  // declared as empty property in schema. The supported property type
  // is string, integer, real, number and boolean.
  //
  if (CheckEmptyPropJsonObject(TempJsonObj, &NunmOfEmptyPropProperties)) {
    Status = CreateEmptyPropCsJson(Cs, JsonObj, Key, Cs->Header.ThisUri, &CsTypeEmptyPropCS, NunmOfEmptyPropProperties);
    if (Status != RedfishCS_status_success) {
      goto Error;
    }
    InsertTailLink(&(*Dst)->Prop, &CsTypeEmptyPropCS->Header.LinkEntry);
  } else {
    Status = CreateCsJsonByNode (Cs, JsonObj, Key, Cs->Header.ThisUri, &CsTypeJson);
    if (Status != RedfishCS_status_success) {
      goto Error;
    }
    InsertTailLink(&(*Dst)->Prop, &CsTypeJson->Header.LinkEntry);
  }
Error:;
  return Status;
}
//
//Generate C structure for Actions
//
static RedfishCS_status GenActionsCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_Actions_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_Actions_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // Oem
  Status = GenOemActionsCs (Cs, TempJsonObj, "Oem", &(*Dst)->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
//
//Generate C structure for Oem
//
static RedfishCS_status GenOemCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishResource_Oem_CS **Dst)
{
  RedfishCS_status Status;
  RedfishCS_Type_JSON_Data *CsTypeJson;
  RedfishCS_Type_EmptyProp_CS_Data *CsTypeEmptyPropCS;
  RedfishCS_uint32 NunmOfEmptyPropProperties;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishResource_Oem_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  InitializeLinkHead (&(*Dst)->Prop);

  //
  // Try to create C structure if the property is
  // declared as empty property in schema. The supported property type
  // is string, integer, real, number and boolean.
  //
  if (CheckEmptyPropJsonObject(TempJsonObj, &NunmOfEmptyPropProperties)) {
    Status = CreateEmptyPropCsJson(Cs, JsonObj, Key, Cs->Header.ThisUri, &CsTypeEmptyPropCS, NunmOfEmptyPropProperties);
    if (Status != RedfishCS_status_success) {
      goto Error;
    }
    InsertTailLink(&(*Dst)->Prop, &CsTypeEmptyPropCS->Header.LinkEntry);
  } else {
    Status = CreateCsJsonByNode (Cs, JsonObj, Key, Cs->Header.ThisUri, &CsTypeJson);
    if (Status != RedfishCS_status_success) {
      goto Error;
    }
    InsertTailLink(&(*Dst)->Prop, &CsTypeJson->Header.LinkEntry);
  }
Error:;
  return Status;
}
static RedfishCS_status GenAttributeValue_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_AttributeValue_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_AttributeValue_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // ValueDisplayName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "ValueDisplayName", &(*Dst)->ValueDisplayName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ValueName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "ValueName", &(*Dst)->ValueName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS
//
static RedfishCS_status GenAttributeValue_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenAttributeValue_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
static RedfishCS_status GenAttributes_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_Attributes_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_Attributes_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // AttributeName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "AttributeName", &(*Dst)->AttributeName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // CurrentValue
  Status = GetRedfishPropertyVague (Cs, TempJsonObj, "CurrentValue", &(*Dst)->CurrentValue);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DefaultValue
  Status = GetRedfishPropertyVague (Cs, TempJsonObj, "DefaultValue", &(*Dst)->DefaultValue);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DisplayName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "DisplayName", &(*Dst)->DisplayName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DisplayOrder
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "DisplayOrder", &(*Dst)->DisplayOrder);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // GrayOut
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "GrayOut", &(*Dst)->GrayOut);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // HelpText
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "HelpText", &(*Dst)->HelpText);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Hidden
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "Hidden", &(*Dst)->Hidden);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Immutable
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "Immutable", &(*Dst)->Immutable);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // IsSystemUniqueProperty
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "IsSystemUniqueProperty", &(*Dst)->IsSystemUniqueProperty);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // LowerBound
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "LowerBound", &(*Dst)->LowerBound);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MaxLength
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "MaxLength", &(*Dst)->MaxLength);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MenuPath
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MenuPath", &(*Dst)->MenuPath);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MinLength
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "MinLength", &(*Dst)->MinLength);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Oem
  Status = GenOemCs (Cs, TempJsonObj, "Oem", &(*Dst)->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ReadOnly
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "ReadOnly", &(*Dst)->ReadOnly);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ResetRequired
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "ResetRequired", &(*Dst)->ResetRequired);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ScalarIncrement
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "ScalarIncrement", &(*Dst)->ScalarIncrement);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Type
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "Type", &(*Dst)->Type);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UefiDevicePath
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "UefiDevicePath", &(*Dst)->UefiDevicePath);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UefiKeywordName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "UefiKeywordName", &(*Dst)->UefiKeywordName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UefiNamespaceId
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "UefiNamespaceId", &(*Dst)->UefiNamespaceId);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UpperBound
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "UpperBound", &(*Dst)->UpperBound);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Value
  Status = GenAttributeValue_ArrayCs (Cs, TempJsonObj, "Value", &(*Dst)->Value);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ValueExpression
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "ValueExpression", &(*Dst)->ValueExpression);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // WarningText
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "WarningText", &(*Dst)->WarningText);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // WriteOnly
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "WriteOnly", &(*Dst)->WriteOnly);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS
//
static RedfishCS_status GenAttributes_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenAttributes_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
static RedfishCS_status GenMapFrom_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_MapFrom_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_MapFrom_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // MapFromAttribute
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapFromAttribute", &(*Dst)->MapFromAttribute);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapFromCondition
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapFromCondition", &(*Dst)->MapFromCondition);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapFromProperty
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapFromProperty", &(*Dst)->MapFromProperty);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapFromValue
  Status = GetRedfishPropertyVague (Cs, TempJsonObj, "MapFromValue", &(*Dst)->MapFromValue);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapTerms
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapTerms", &(*Dst)->MapTerms);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS
//
static RedfishCS_status GenMapFrom_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenMapFrom_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
//
//Generate C structure for Dependency
//
static RedfishCS_status GenDependencyCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_Dependency_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_Dependency_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // MapFrom
  Status = GenMapFrom_ArrayCs (Cs, TempJsonObj, "MapFrom", &(*Dst)->MapFrom);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapToAttribute
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapToAttribute", &(*Dst)->MapToAttribute);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapToProperty
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MapToProperty", &(*Dst)->MapToProperty);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MapToValue
  Status = GetRedfishPropertyVague (Cs, TempJsonObj, "MapToValue", &(*Dst)->MapToValue);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
static RedfishCS_status GenDependencies_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_Dependencies_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_Dependencies_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // Dependency
  Status = GenDependencyCs (Cs, TempJsonObj, "Dependency", &(*Dst)->Dependency);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DependencyFor
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "DependencyFor", &(*Dst)->DependencyFor);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Type
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "Type", &(*Dst)->Type);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS
//
static RedfishCS_status GenDependencies_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenDependencies_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
static RedfishCS_status GenMenus_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_Menus_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_Menus_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // DisplayName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "DisplayName", &(*Dst)->DisplayName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DisplayOrder
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "DisplayOrder", &(*Dst)->DisplayOrder);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // GrayOut
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "GrayOut", &(*Dst)->GrayOut);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Hidden
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "Hidden", &(*Dst)->Hidden);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MenuName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MenuName", &(*Dst)->MenuName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MenuPath
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "MenuPath", &(*Dst)->MenuPath);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Oem
  Status = GenOemCs (Cs, TempJsonObj, "Oem", &(*Dst)->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ReadOnly
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "ReadOnly", &(*Dst)->ReadOnly);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_Menus_Array_CS
//
static RedfishCS_status GenMenus_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_Menus_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_Menus_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_Menus_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenMenus_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
//
//Generate C structure for RegistryEntries
//
static RedfishCS_status GenRegistryEntriesCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_RegistryEntries_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_RegistryEntries_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // Attributes
  Status = GenAttributes_ArrayCs (Cs, TempJsonObj, "Attributes", &(*Dst)->Attributes);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Dependencies
  Status = GenDependencies_ArrayCs (Cs, TempJsonObj, "Dependencies", &(*Dst)->Dependencies);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Menus
  Status = GenMenus_ArrayCs (Cs, TempJsonObj, "Menus", &(*Dst)->Menus);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
static RedfishCS_status GenSupportedSystems_Array_ElementCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  RedfishAttributeRegistry_V1_3_6_SupportedSystems_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(RedfishAttributeRegistry_V1_3_6_SupportedSystems_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // FirmwareVersion
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "FirmwareVersion", &(*Dst)->FirmwareVersion);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ProductName
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "ProductName", &(*Dst)->ProductName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // SystemId
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "SystemId", &(*Dst)->SystemId);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS
//
static RedfishCS_status GenSupportedSystems_ArrayCs(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs, json_t *JsonObj, char *Key, RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenSupportedSystems_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
static RedfishCS_status CS_To_JSON_ActionsOem(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_OemActions_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Check if this is RedfishCS_Type_CS_EmptyProp.
  CsEmptyPropLinkToJson(CsJson, Key, &CSPtr->Prop);
  // No JSON property for this structure.
  return RedfishCS_status_success;
}
static RedfishCS_status CS_To_JSON_Actions(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_Actions_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Oem
  if (CS_To_JSON_ActionsOem(CsJson, "Oem", CSPtr->Oem) != RedfishCS_status_success) {goto Error;}

  // Set to parent JSON object.
  if (json_object_set_new (CsParentJson, Key, CsJson) == -1) {goto Error;}

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_Oem(json_t *CsJson, char *Key, RedfishResource_Oem_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Check if this is RedfishCS_Type_CS_EmptyProp.
  CsEmptyPropLinkToJson(CsJson, Key, &CSPtr->Prop);
  // No JSON property for this structure.
  return RedfishCS_status_success;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesAttributesOem(json_t *CsJson, char *Key, RedfishResource_Oem_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Check if this is RedfishCS_Type_CS_EmptyProp.
  CsEmptyPropLinkToJson(CsJson, Key, &CSPtr->Prop);
  // No JSON property for this structure.
  return RedfishCS_status_success;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesAttributesValue(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_AttributeValue_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_AttributeValue_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // ValueDisplayName 
    if (InsertJsonStringObj (ArrayMember, "ValueDisplayName", NextArrayItem->ValueDisplayName) != RedfishCS_status_success) {goto Error;}

    // ValueName 
    if (InsertJsonStringObj (ArrayMember, "ValueName", NextArrayItem->ValueName) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesAttributes(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_Attributes_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_Attributes_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // AttributeName 
    if (InsertJsonStringObj (ArrayMember, "AttributeName", NextArrayItem->AttributeName) != RedfishCS_status_success) {goto Error;}

    // CurrentValue 
    if (InsertJsonVagueObj (ArrayMember, "CurrentValue", NextArrayItem->CurrentValue) != RedfishCS_status_success) {goto Error;}

    // DefaultValue 
    if (InsertJsonVagueObj (ArrayMember, "DefaultValue", NextArrayItem->DefaultValue) != RedfishCS_status_success) {goto Error;}

    // DisplayName 
    if (InsertJsonStringObj (ArrayMember, "DisplayName", NextArrayItem->DisplayName) != RedfishCS_status_success) {goto Error;}

    // DisplayOrder 
    if (InsertJsonInt64Obj (ArrayMember, "DisplayOrder", NextArrayItem->DisplayOrder) != RedfishCS_status_success) {goto Error;}

    // GrayOut 
    if (InsertJsonBoolObj (ArrayMember, "GrayOut", NextArrayItem->GrayOut) != RedfishCS_status_success) {goto Error;}

    // HelpText 
    if (InsertJsonStringObj (ArrayMember, "HelpText", NextArrayItem->HelpText) != RedfishCS_status_success) {goto Error;}

    // Hidden 
    if (InsertJsonBoolObj (ArrayMember, "Hidden", NextArrayItem->Hidden) != RedfishCS_status_success) {goto Error;}

    // Immutable 
    if (InsertJsonBoolObj (ArrayMember, "Immutable", NextArrayItem->Immutable) != RedfishCS_status_success) {goto Error;}

    // IsSystemUniqueProperty 
    if (InsertJsonBoolObj (ArrayMember, "IsSystemUniqueProperty", NextArrayItem->IsSystemUniqueProperty) != RedfishCS_status_success) {goto Error;}

    // LowerBound 
    if (InsertJsonInt64Obj (ArrayMember, "LowerBound", NextArrayItem->LowerBound) != RedfishCS_status_success) {goto Error;}

    // MaxLength 
    if (InsertJsonInt64Obj (ArrayMember, "MaxLength", NextArrayItem->MaxLength) != RedfishCS_status_success) {goto Error;}

    // MenuPath 
    if (InsertJsonStringObj (ArrayMember, "MenuPath", NextArrayItem->MenuPath) != RedfishCS_status_success) {goto Error;}

    // MinLength 
    if (InsertJsonInt64Obj (ArrayMember, "MinLength", NextArrayItem->MinLength) != RedfishCS_status_success) {goto Error;}

    // Oem
    if (CS_To_JSON_RegistryEntriesAttributesOem(ArrayMember, "Oem", NextArrayItem->Oem) != RedfishCS_status_success) {goto Error;}

    // ReadOnly 
    if (InsertJsonBoolObj (ArrayMember, "ReadOnly", NextArrayItem->ReadOnly) != RedfishCS_status_success) {goto Error;}

    // ResetRequired 
    if (InsertJsonBoolObj (ArrayMember, "ResetRequired", NextArrayItem->ResetRequired) != RedfishCS_status_success) {goto Error;}

    // ScalarIncrement 
    if (InsertJsonInt64Obj (ArrayMember, "ScalarIncrement", NextArrayItem->ScalarIncrement) != RedfishCS_status_success) {goto Error;}

    // Type 
    if (InsertJsonStringObj (ArrayMember, "Type", NextArrayItem->Type) != RedfishCS_status_success) {goto Error;}

    // UefiDevicePath 
    if (InsertJsonStringObj (ArrayMember, "UefiDevicePath", NextArrayItem->UefiDevicePath) != RedfishCS_status_success) {goto Error;}

    // UefiKeywordName 
    if (InsertJsonStringObj (ArrayMember, "UefiKeywordName", NextArrayItem->UefiKeywordName) != RedfishCS_status_success) {goto Error;}

    // UefiNamespaceId 
    if (InsertJsonStringObj (ArrayMember, "UefiNamespaceId", NextArrayItem->UefiNamespaceId) != RedfishCS_status_success) {goto Error;}

    // UpperBound 
    if (InsertJsonInt64Obj (ArrayMember, "UpperBound", NextArrayItem->UpperBound) != RedfishCS_status_success) {goto Error;}

    // Value
    if (CS_To_JSON_RegistryEntriesAttributesValue(ArrayMember, "Value", NextArrayItem->Value) != RedfishCS_status_success) {goto Error;}

    // ValueExpression 
    if (InsertJsonStringObj (ArrayMember, "ValueExpression", NextArrayItem->ValueExpression) != RedfishCS_status_success) {goto Error;}

    // WarningText 
    if (InsertJsonStringObj (ArrayMember, "WarningText", NextArrayItem->WarningText) != RedfishCS_status_success) {goto Error;}

    // WriteOnly 
    if (InsertJsonBoolObj (ArrayMember, "WriteOnly", NextArrayItem->WriteOnly) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesDependenciesDependencyMapFrom(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_MapFrom_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_MapFrom_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // MapFromAttribute 
    if (InsertJsonStringObj (ArrayMember, "MapFromAttribute", NextArrayItem->MapFromAttribute) != RedfishCS_status_success) {goto Error;}

    // MapFromCondition 
    if (InsertJsonStringObj (ArrayMember, "MapFromCondition", NextArrayItem->MapFromCondition) != RedfishCS_status_success) {goto Error;}

    // MapFromProperty 
    if (InsertJsonStringObj (ArrayMember, "MapFromProperty", NextArrayItem->MapFromProperty) != RedfishCS_status_success) {goto Error;}

    // MapFromValue 
    if (InsertJsonVagueObj (ArrayMember, "MapFromValue", NextArrayItem->MapFromValue) != RedfishCS_status_success) {goto Error;}

    // MapTerms 
    if (InsertJsonStringObj (ArrayMember, "MapTerms", NextArrayItem->MapTerms) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesDependenciesDependency(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_Dependency_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // MapFrom
  if (CS_To_JSON_RegistryEntriesDependenciesDependencyMapFrom(CsJson, "MapFrom", CSPtr->MapFrom) != RedfishCS_status_success) {goto Error;}

  // MapToAttribute 
  if (InsertJsonStringObj (CsJson, "MapToAttribute", CSPtr->MapToAttribute) != RedfishCS_status_success) {goto Error;}

  // MapToProperty 
  if (InsertJsonStringObj (CsJson, "MapToProperty", CSPtr->MapToProperty) != RedfishCS_status_success) {goto Error;}

  // MapToValue 
  if (InsertJsonVagueObj (CsJson, "MapToValue", CSPtr->MapToValue) != RedfishCS_status_success) {goto Error;}

  // Set to parent JSON object.
  if (json_object_set_new (CsParentJson, Key, CsJson) == -1) {goto Error;}

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesDependencies(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_Dependencies_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_Dependencies_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // Dependency
    if (CS_To_JSON_RegistryEntriesDependenciesDependency(ArrayMember, "Dependency", NextArrayItem->Dependency) != RedfishCS_status_success) {goto Error;}

    // DependencyFor 
    if (InsertJsonStringObj (ArrayMember, "DependencyFor", NextArrayItem->DependencyFor) != RedfishCS_status_success) {goto Error;}

    // Type 
    if (InsertJsonStringObj (ArrayMember, "Type", NextArrayItem->Type) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesMenusOem(json_t *CsJson, char *Key, RedfishResource_Oem_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Check if this is RedfishCS_Type_CS_EmptyProp.
  CsEmptyPropLinkToJson(CsJson, Key, &CSPtr->Prop);
  // No JSON property for this structure.
  return RedfishCS_status_success;
}
static RedfishCS_status CS_To_JSON_RegistryEntriesMenus(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_Menus_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_Menus_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_Menus_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // DisplayName 
    if (InsertJsonStringObj (ArrayMember, "DisplayName", NextArrayItem->DisplayName) != RedfishCS_status_success) {goto Error;}

    // DisplayOrder 
    if (InsertJsonInt64Obj (ArrayMember, "DisplayOrder", NextArrayItem->DisplayOrder) != RedfishCS_status_success) {goto Error;}

    // GrayOut 
    if (InsertJsonBoolObj (ArrayMember, "GrayOut", NextArrayItem->GrayOut) != RedfishCS_status_success) {goto Error;}

    // Hidden 
    if (InsertJsonBoolObj (ArrayMember, "Hidden", NextArrayItem->Hidden) != RedfishCS_status_success) {goto Error;}

    // MenuName 
    if (InsertJsonStringObj (ArrayMember, "MenuName", NextArrayItem->MenuName) != RedfishCS_status_success) {goto Error;}

    // MenuPath 
    if (InsertJsonStringObj (ArrayMember, "MenuPath", NextArrayItem->MenuPath) != RedfishCS_status_success) {goto Error;}

    // Oem
    if (CS_To_JSON_RegistryEntriesMenusOem(ArrayMember, "Oem", NextArrayItem->Oem) != RedfishCS_status_success) {goto Error;}

    // ReadOnly 
    if (InsertJsonBoolObj (ArrayMember, "ReadOnly", NextArrayItem->ReadOnly) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_RegistryEntries(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_RegistryEntries_CS *CSPtr)
{
  json_t *CsParentJson;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }

  CsParentJson = CsJson;
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }

  // Attributes
  if (CS_To_JSON_RegistryEntriesAttributes(CsJson, "Attributes", CSPtr->Attributes) != RedfishCS_status_success) {goto Error;}

  // Dependencies
  if (CS_To_JSON_RegistryEntriesDependencies(CsJson, "Dependencies", CSPtr->Dependencies) != RedfishCS_status_success) {goto Error;}

  // Menus
  if (CS_To_JSON_RegistryEntriesMenus(CsJson, "Menus", CSPtr->Menus) != RedfishCS_status_success) {goto Error;}

  // Set to parent JSON object.
  if (json_object_set_new (CsParentJson, Key, CsJson) == -1) {goto Error;}

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_SupportedSystems(json_t *CsJson, char *Key, RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  RedfishAttributeRegistry_V1_3_6_SupportedSystems_Array_CS *NextArray;
  RedfishAttributeRegistry_V1_3_6_SupportedSystems_CS *NextArrayItem;

  if (CSPtr == NULL) {
    return RedfishCS_status_success;
  }
  ArrayJson = json_array();
  if (ArrayJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  NextArray = CSPtr;
  do {
    ArrayMember = json_object();
    if (ArrayMember == NULL) {
      return RedfishCS_status_unsupported;
    }

    NextArrayItem = NextArray->ArrayValue;
    // FirmwareVersion 
    if (InsertJsonStringObj (ArrayMember, "FirmwareVersion", NextArrayItem->FirmwareVersion) != RedfishCS_status_success) {goto Error;}

    // ProductName 
    if (InsertJsonStringObj (ArrayMember, "ProductName", NextArrayItem->ProductName) != RedfishCS_status_success) {goto Error;}

    // SystemId 
    if (InsertJsonStringObj (ArrayMember, "SystemId", NextArrayItem->SystemId) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}

//
// C structure to JSON for AttributeRegistry.V1_3_6.AttributeRegistry
//
RedfishCS_status CS_To_AttributeRegistry_V1_3_6_JSON(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *CSPtr, RedfishCS_char **JsonText)
{
  json_t  *CsJson;

  if (CSPtr == NULL || JsonText == NULL || CSPtr->Header.ResourceType != RedfishCS_Type_CS) {
    return RedfishCS_status_invalid_parameter;
  }
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  // Actions
  if (CS_To_JSON_Actions(CsJson, "Actions", CSPtr->Actions) != RedfishCS_status_success) {goto Error;}

  // Description 
  if (InsertJsonStringObj (CsJson, "Description", CSPtr->Description) != RedfishCS_status_success) {goto Error;}

  // Id 
  if (InsertJsonStringObj (CsJson, "Id", CSPtr->Id) != RedfishCS_status_success) {goto Error;}

  // Language 
  if (InsertJsonStringObj (CsJson, "Language", CSPtr->Language) != RedfishCS_status_success) {goto Error;}

  // Name 
  if (InsertJsonStringObj (CsJson, "Name", CSPtr->Name) != RedfishCS_status_success) {goto Error;}

  // Oem
  if (CS_To_JSON_Oem(CsJson, "Oem", CSPtr->Oem) != RedfishCS_status_success) {goto Error;}

  // OwningEntity 
  if (InsertJsonStringObj (CsJson, "OwningEntity", CSPtr->OwningEntity) != RedfishCS_status_success) {goto Error;}

  // RegistryEntries
  if (CS_To_JSON_RegistryEntries(CsJson, "RegistryEntries", CSPtr->RegistryEntries) != RedfishCS_status_success) {goto Error;}

  // RegistryVersion 
  if (InsertJsonStringObj (CsJson, "RegistryVersion", CSPtr->RegistryVersion) != RedfishCS_status_success) {goto Error;}

  // SupportedSystems
  if (CS_To_JSON_SupportedSystems(CsJson, "SupportedSystems", CSPtr->SupportedSystems) != RedfishCS_status_success) {goto Error;}

  // @odata.type 
  if (InsertJsonStringObj (CsJson, "@odata.type", CSPtr->odata_type) != RedfishCS_status_success) {goto Error;}

  *JsonText = (RedfishCS_char *)json_dumps(CsJson, JSON_INDENT(2 * 1) | JSON_ENSURE_ASCII);
  json_decref(CsJson);
  return RedfishCS_status_success;
Error:;
  json_decref(CsJson);
  return RedfishCS_status_unsupported;
}

//
// Destory C Structure for AttributeRegistry.V1_3_6.AttributeRegistry
//
RedfishCS_status DestroyAttributeRegistry_V1_3_6_CS(RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *CSPtr)
{
  RedfishCS_status Status;

  Status = DestoryCsMemory ((RedfishCS_void *)CSPtr);
  return Status;
}

//
// Destory JSON text for AttributeRegistry.V1_3_6.AttributeRegistry
//
RedfishCS_status DestroyAttributeRegistry_V1_3_6_Json(RedfishCS_char *JsonText)
{
  free ((RedfishCS_void *)JsonText);
  return RedfishCS_status_success;
}

//
//Generate C structure for AttributeRegistry.V1_3_6.AttributeRegistry
//
RedfishCS_status
Json_AttributeRegistry_V1_3_6_To_CS(RedfishCS_char *JsonRawText, RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS **ReturnedCs)
{
  RedfishCS_status  Status;
  json_t *JsonObj;
  RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS *Cs;

  Status = CreateJsonPayloadAndCs (JsonRawText, "AttributeRegistry", "v1_3_6", "AttributeRegistry", &JsonObj, (RedfishCS_void **)&Cs, sizeof (RedfishAttributeRegistry_V1_3_6_AttributeRegistry_CS));
  if (Status != RedfishCS_status_success) {
    goto Error;
  }

  // @odata.type
  Status = GetRedfishPropertyStr (Cs, JsonObj, "@odata.type", &Cs->odata_type);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Actions
  Status = GenActionsCs (Cs, JsonObj, "Actions", &Cs->Actions);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Description
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Description", &Cs->Description);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Id
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Id", &Cs->Id);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Language
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Language", &Cs->Language);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Name
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Name", &Cs->Name);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Oem
  Status = GenOemCs (Cs, JsonObj, "Oem", &Cs->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // OwningEntity
  Status = GetRedfishPropertyStr (Cs, JsonObj, "OwningEntity", &Cs->OwningEntity);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // RegistryEntries
  Status = GenRegistryEntriesCs (Cs, JsonObj, "RegistryEntries", &Cs->RegistryEntries);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // RegistryVersion
  Status = GetRedfishPropertyStr (Cs, JsonObj, "RegistryVersion", &Cs->RegistryVersion);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // SupportedSystems
  Status = GenSupportedSystems_ArrayCs (Cs, JsonObj, "SupportedSystems", &Cs->SupportedSystems);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  json_decref(JsonObj);
  *ReturnedCs = Cs;
  return RedfishCS_status_success;
Error:;
  json_decref(JsonObj);
  DestroyAttributeRegistry_V1_3_6_CS (Cs);
  return Status;
}
