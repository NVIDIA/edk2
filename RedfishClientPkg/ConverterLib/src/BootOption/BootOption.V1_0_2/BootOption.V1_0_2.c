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

#include"Redfish_BootOption_v1_0_2_CS.h"
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

//
//Generate C structure for Oem
//
static RedfishCS_status GenOemActionsCs(RedfishBootOption_V1_0_2_BootOption_CS *Cs, json_t *JsonObj, char *Key, RedfishBootOption_V1_0_2_OemActions_CS **Dst)
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
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishBootOption_V1_0_2_OemActions_CS), (RedfishCS_void **)Dst);
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
static RedfishCS_status GenActionsCs(RedfishBootOption_V1_0_2_BootOption_CS *Cs, json_t *JsonObj, char *Key, RedfishBootOption_V1_0_2_Actions_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishBootOption_V1_0_2_Actions_CS), (RedfishCS_void **)Dst);
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
static RedfishCS_status GenOemCs(RedfishBootOption_V1_0_2_BootOption_CS *Cs, json_t *JsonObj, char *Key, RedfishResource_Oem_CS **Dst)
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
static RedfishCS_status GenidRef_Array_ElementCs(RedfishBootOption_V1_0_2_BootOption_CS *Cs, json_t *JsonObj, RedfishCS_uint64 Index,  Redfishodatav4_idRef_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_array_get (JsonObj, (RedfishCS_int)Index);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsZeroMemory(Cs, sizeof(Redfishodatav4_idRef_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // @odata.id
  Status = GetRedfishPropertyStr (Cs, TempJsonObj, "@odata.id", &(*Dst)->odata_id);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  return RedfishCS_status_success;
Error:;
  return Status;
}
//
//Generate C structure for Redfishodatav4_idRef_Array_CS
//
static RedfishCS_status GenidRef_ArrayCs(RedfishBootOption_V1_0_2_BootOption_CS *Cs, json_t *JsonObj, char *Key, Redfishodatav4_idRef_Array_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;
  RedfishCS_uint64 ArraySize;
  RedfishCS_uint64 Index;
  Redfishodatav4_idRef_Array_CS *ThisElement;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }

  if (json_is_array(TempJsonObj) != RedfishCS_boolean_true) {
    return RedfishCS_status_invalid_parameter;
  }

  ArraySize = json_array_size (TempJsonObj);
  Status = allocateArrayRecordCsMemory(Cs, sizeof (Redfishodatav4_idRef_Array_CS), ArraySize, (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  if (*Dst == NULL) {
    // Empty array
    return RedfishCS_status_success;
  }
  ThisElement = *Dst;
  for (Index = 0; Index < ArraySize; Index ++) {
    Status = GenidRef_Array_ElementCs(Cs, TempJsonObj, Index, &ThisElement->ArrayValue);
    if (Status != RedfishCS_status_success){
      goto Error;
    }
    ThisElement = ThisElement->Next;
  }
Error:;
  return Status;
}
static RedfishCS_status CS_To_JSON_ActionsOem(json_t *CsJson, char *Key, RedfishBootOption_V1_0_2_OemActions_CS *CSPtr)
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
static RedfishCS_status CS_To_JSON_Actions(json_t *CsJson, char *Key, RedfishBootOption_V1_0_2_Actions_CS *CSPtr)
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
static RedfishCS_status CS_To_JSON_RelatedItem(json_t *CsJson, char *Key, Redfishodatav4_idRef_Array_CS *CSPtr)
{
  json_t *ArrayJson;
  json_t *ArrayMember;
  Redfishodatav4_idRef_Array_CS *NextArray;
  Redfishodatav4_idRef_CS *NextArrayItem;

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
    // @odata.id 
    if (InsertJsonStringObj (ArrayMember, "@odata.id", NextArrayItem->odata_id) != RedfishCS_status_success) {goto Error;}

    if (json_array_append_new (ArrayJson, ArrayMember) != 0) {goto Error;}
    NextArray = NextArray->Next;
  } while (NextArray != NULL);
  json_object_set_new (CsJson, Key, ArrayJson);

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}

//
// C structure to JSON for BootOption.V1_0_2.BootOption
//
RedfishCS_status CS_To_BootOption_V1_0_2_JSON(RedfishBootOption_V1_0_2_BootOption_CS *CSPtr, RedfishCS_char **JsonText)
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

  // Alias 
  if (InsertJsonStringObj (CsJson, "Alias", CSPtr->Alias) != RedfishCS_status_success) {goto Error;}

  // BootOptionEnabled 
  if (InsertJsonBoolObj (CsJson, "BootOptionEnabled", CSPtr->BootOptionEnabled) != RedfishCS_status_success) {goto Error;}

  // BootOptionReference 
  if (InsertJsonStringObj (CsJson, "BootOptionReference", CSPtr->BootOptionReference) != RedfishCS_status_success) {goto Error;}

  // Description 
  if (InsertJsonStringObj (CsJson, "Description", CSPtr->Description) != RedfishCS_status_success) {goto Error;}

  // DisplayName 
  if (InsertJsonStringObj (CsJson, "DisplayName", CSPtr->DisplayName) != RedfishCS_status_success) {goto Error;}

  // Id 
  if (InsertJsonStringObj (CsJson, "Id", CSPtr->Id) != RedfishCS_status_success) {goto Error;}

  // Name 
  if (InsertJsonStringObj (CsJson, "Name", CSPtr->Name) != RedfishCS_status_success) {goto Error;}

  // Oem
  if (CS_To_JSON_Oem(CsJson, "Oem", CSPtr->Oem) != RedfishCS_status_success) {goto Error;}

  // RelatedItem
  if (CS_To_JSON_RelatedItem(CsJson, "RelatedItem", CSPtr->RelatedItem) != RedfishCS_status_success) {goto Error;}

  // RelatedItem@odata.count 
  if (InsertJsonInt64Obj (CsJson, "RelatedItem@odata.count", CSPtr->RelatedItemodata_count) != RedfishCS_status_success) {goto Error;}

  // UefiDevicePath 
  if (InsertJsonStringObj (CsJson, "UefiDevicePath", CSPtr->UefiDevicePath) != RedfishCS_status_success) {goto Error;}

  // @odata.context 
  if (InsertJsonStringObj (CsJson, "@odata.context", CSPtr->odata_context) != RedfishCS_status_success) {goto Error;}

  // @odata.etag 
  if (InsertJsonStringObj (CsJson, "@odata.etag", CSPtr->odata_etag) != RedfishCS_status_success) {goto Error;}

  // @odata.id 
  if (InsertJsonStringObj (CsJson, "@odata.id", CSPtr->odata_id) != RedfishCS_status_success) {goto Error;}

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
// Destory C Structure for BootOption.V1_0_2.BootOption
//
RedfishCS_status DestroyBootOption_V1_0_2_CS(RedfishBootOption_V1_0_2_BootOption_CS *CSPtr)
{
  RedfishCS_status Status;

  Status = DestoryCsMemory ((RedfishCS_void *)CSPtr);
  return Status;
}

//
// Destory JSON text for BootOption.V1_0_2.BootOption
//
RedfishCS_status DestroyBootOption_V1_0_2_Json(RedfishCS_char *JsonText)
{
  free ((RedfishCS_void *)JsonText);
  return RedfishCS_status_success;
}

//
//Generate C structure for BootOption.V1_0_2.BootOption
//
RedfishCS_status
Json_BootOption_V1_0_2_To_CS(RedfishCS_char *JsonRawText, RedfishBootOption_V1_0_2_BootOption_CS **ReturnedCs)
{
  RedfishCS_status  Status;
  json_t *JsonObj;
  RedfishBootOption_V1_0_2_BootOption_CS *Cs;

  Status = CreateJsonPayloadAndCs (JsonRawText, "BootOption", "v1_0_2", "BootOption", &JsonObj, (RedfishCS_void **)&Cs, sizeof (RedfishBootOption_V1_0_2_BootOption_CS));
  if (Status != RedfishCS_status_success) {
    goto Error;
  }

  // @odata.context
  Status = GetRedfishPropertyStr (Cs, JsonObj, "@odata.context", &Cs->odata_context);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // @odata.etag
  Status = GetRedfishPropertyStr (Cs, JsonObj, "@odata.etag", &Cs->odata_etag);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // @odata.id
  Status = GetRedfishPropertyStr (Cs, JsonObj, "@odata.id", &Cs->odata_id);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // @odata.type
  Status = GetRedfishPropertyStr (Cs, JsonObj, "@odata.type", &Cs->odata_type);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Actions
  Status = GenActionsCs (Cs, JsonObj, "Actions", &Cs->Actions);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Alias
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Alias", &Cs->Alias);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // BootOptionEnabled
  Status = GetRedfishPropertyBoolean (Cs, JsonObj, "BootOptionEnabled", &Cs->BootOptionEnabled);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // BootOptionReference
  Status = GetRedfishPropertyStr (Cs, JsonObj, "BootOptionReference", &Cs->BootOptionReference);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Description
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Description", &Cs->Description);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // DisplayName
  Status = GetRedfishPropertyStr (Cs, JsonObj, "DisplayName", &Cs->DisplayName);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Id
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Id", &Cs->Id);
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

  // RelatedItem
  Status = GenidRef_ArrayCs (Cs, JsonObj, "RelatedItem", &Cs->RelatedItem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // RelatedItem@odata.count
  Status = GetRedfishPropertyInt64 (Cs, JsonObj, "RelatedItem@odata.count", &Cs->RelatedItemodata_count);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UefiDevicePath
  Status = GetRedfishPropertyStr (Cs, JsonObj, "UefiDevicePath", &Cs->UefiDevicePath);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  json_decref(JsonObj);
  *ReturnedCs = Cs;
  return RedfishCS_status_success;
Error:;
  json_decref(JsonObj);
  DestroyBootOption_V1_0_2_CS (Cs);
  return Status;
}
