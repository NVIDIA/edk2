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

#include"Redfish_ServiceRoot_v1_6_0_CS.h"
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

static RedfishCS_status GenOemCs(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs, json_t *JsonObj, char *Key, RedfishResource_Oem_CS **Dst);
//
//Generate C structure for Oem
//
static RedfishCS_status GenOemCs(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs, json_t *JsonObj, char *Key, RedfishResource_Oem_CS **Dst)
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
//
//Generate C structure for Links
//
static RedfishCS_status GenLinksCs(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs, json_t *JsonObj, char *Key, RedfishServiceRoot_V1_6_0_Links_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishServiceRoot_V1_6_0_Links_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // Oem
  Status = GenOemCs (Cs, TempJsonObj, "Oem", &(*Dst)->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Sessions
  InitializeLinkHead (&(*Dst)->Sessions);
  Status = CreateCsUriOrJsonByNode (Cs, TempJsonObj, "Sessions", Cs->Header.ThisUri, &(*Dst)->Sessions);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
//
//Generate C structure for ExpandQuery
//
static RedfishCS_status GenExpandCs(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs, json_t *JsonObj, char *Key, RedfishServiceRoot_V1_6_0_Expand_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishServiceRoot_V1_6_0_Expand_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // ExpandAll
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "ExpandAll", &(*Dst)->ExpandAll);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Levels
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "Levels", &(*Dst)->Levels);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Links
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "Links", &(*Dst)->Links);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // MaxLevels
  Status = GetRedfishPropertyInt64 (Cs, TempJsonObj, "MaxLevels", &(*Dst)->MaxLevels);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // NoLinks
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "NoLinks", &(*Dst)->NoLinks);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
//
//Generate C structure for ProtocolFeaturesSupported
//
static RedfishCS_status GenProtocolFeaturesSupportedCs(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs, json_t *JsonObj, char *Key, RedfishServiceRoot_V1_6_0_ProtocolFeaturesSupported_CS **Dst)
{
  RedfishCS_status Status;
  json_t *TempJsonObj;

  Status = RedfishCS_status_success;
  TempJsonObj = json_object_get(JsonObj, Key);
  if (TempJsonObj == NULL) {
    return RedfishCS_status_not_found;
  }
  Status = allocateRecordCsMemory(Cs, sizeof(RedfishServiceRoot_V1_6_0_ProtocolFeaturesSupported_CS), (RedfishCS_void **)Dst);
  if (Status != RedfishCS_status_success){
    goto Error;
  }
  // ExcerptQuery
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "ExcerptQuery", &(*Dst)->ExcerptQuery);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ExpandQuery
  Status = GenExpandCs (Cs, TempJsonObj, "ExpandQuery", &(*Dst)->ExpandQuery);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // FilterQuery
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "FilterQuery", &(*Dst)->FilterQuery);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // OnlyMemberQuery
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "OnlyMemberQuery", &(*Dst)->OnlyMemberQuery);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // SelectQuery
  Status = GetRedfishPropertyBoolean (Cs, TempJsonObj, "SelectQuery", &(*Dst)->SelectQuery);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

Error:;
  return Status;
}
static RedfishCS_status CS_To_JSON_LinksOem(json_t *CsJson, char *Key, RedfishResource_Oem_CS *CSPtr)
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
static RedfishCS_status CS_To_JSON_Links(json_t *CsJson, char *Key, RedfishServiceRoot_V1_6_0_Links_CS *CSPtr)
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
  if (CS_To_JSON_LinksOem(CsJson, "Oem", CSPtr->Oem) != RedfishCS_status_success) {goto Error;}

  // Sessions 
  if (InsertJsonLinkObj (CsJson, "Sessions", &CSPtr->Sessions) != RedfishCS_status_success) {goto Error;}

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
static RedfishCS_status CS_To_JSON_ProtocolFeaturesSupportedExpandQuery(json_t *CsJson, char *Key, RedfishServiceRoot_V1_6_0_Expand_CS *CSPtr)
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

  // ExpandAll 
  if (InsertJsonBoolObj (CsJson, "ExpandAll", CSPtr->ExpandAll) != RedfishCS_status_success) {goto Error;}

  // Levels 
  if (InsertJsonBoolObj (CsJson, "Levels", CSPtr->Levels) != RedfishCS_status_success) {goto Error;}

  // Links 
  if (InsertJsonBoolObj (CsJson, "Links", CSPtr->Links) != RedfishCS_status_success) {goto Error;}

  // MaxLevels 
  if (InsertJsonInt64Obj (CsJson, "MaxLevels", CSPtr->MaxLevels) != RedfishCS_status_success) {goto Error;}

  // NoLinks 
  if (InsertJsonBoolObj (CsJson, "NoLinks", CSPtr->NoLinks) != RedfishCS_status_success) {goto Error;}

  // Set to parent JSON object.
  if (json_object_set_new (CsParentJson, Key, CsJson) == -1) {goto Error;}

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}
static RedfishCS_status CS_To_JSON_ProtocolFeaturesSupported(json_t *CsJson, char *Key, RedfishServiceRoot_V1_6_0_ProtocolFeaturesSupported_CS *CSPtr)
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

  // ExcerptQuery 
  if (InsertJsonBoolObj (CsJson, "ExcerptQuery", CSPtr->ExcerptQuery) != RedfishCS_status_success) {goto Error;}

  // ExpandQuery
  if (CS_To_JSON_ProtocolFeaturesSupportedExpandQuery(CsJson, "ExpandQuery", CSPtr->ExpandQuery) != RedfishCS_status_success) {goto Error;}

  // FilterQuery 
  if (InsertJsonBoolObj (CsJson, "FilterQuery", CSPtr->FilterQuery) != RedfishCS_status_success) {goto Error;}

  // OnlyMemberQuery 
  if (InsertJsonBoolObj (CsJson, "OnlyMemberQuery", CSPtr->OnlyMemberQuery) != RedfishCS_status_success) {goto Error;}

  // SelectQuery 
  if (InsertJsonBoolObj (CsJson, "SelectQuery", CSPtr->SelectQuery) != RedfishCS_status_success) {goto Error;}

  // Set to parent JSON object.
  if (json_object_set_new (CsParentJson, Key, CsJson) == -1) {goto Error;}

  return RedfishCS_status_success;
Error:;
  return RedfishCS_status_unsupported;
}

//
// C structure to JSON for ServiceRoot.V1_6_0.ServiceRoot
//
RedfishCS_status CS_To_ServiceRoot_V1_6_0_JSON(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *CSPtr, RedfishCS_char **JsonText)
{
  json_t  *CsJson;

  if (CSPtr == NULL || JsonText == NULL || CSPtr->Header.ResourceType != RedfishCS_Type_CS) {
    return RedfishCS_status_invalid_parameter;
  }
  CsJson = json_object();
  if (CsJson == NULL) {
    return RedfishCS_status_unsupported;
  }
  // AccountService 
  if (InsertJsonLinkObj (CsJson, "AccountService", &CSPtr->AccountService) != RedfishCS_status_success) {goto Error;}

  // CertificateService 
  if (InsertJsonLinkObj (CsJson, "CertificateService", &CSPtr->CertificateService) != RedfishCS_status_success) {goto Error;}

  // Chassis 
  if (InsertJsonLinkObj (CsJson, "Chassis", &CSPtr->Chassis) != RedfishCS_status_success) {goto Error;}

  // CompositionService 
  if (InsertJsonLinkObj (CsJson, "CompositionService", &CSPtr->CompositionService) != RedfishCS_status_success) {goto Error;}

  // Description 
  if (InsertJsonStringObj (CsJson, "Description", CSPtr->Description) != RedfishCS_status_success) {goto Error;}

  // EventService 
  if (InsertJsonLinkObj (CsJson, "EventService", &CSPtr->EventService) != RedfishCS_status_success) {goto Error;}

  // Fabrics 
  if (InsertJsonLinkObj (CsJson, "Fabrics", &CSPtr->Fabrics) != RedfishCS_status_success) {goto Error;}

  // Facilities 
  if (InsertJsonLinkObj (CsJson, "Facilities", &CSPtr->Facilities) != RedfishCS_status_success) {goto Error;}

  // Id 
  if (InsertJsonStringObj (CsJson, "Id", CSPtr->Id) != RedfishCS_status_success) {goto Error;}

  // JobService 
  if (InsertJsonLinkObj (CsJson, "JobService", &CSPtr->JobService) != RedfishCS_status_success) {goto Error;}

  // JsonSchemas 
  if (InsertJsonLinkObj (CsJson, "JsonSchemas", &CSPtr->JsonSchemas) != RedfishCS_status_success) {goto Error;}

  // Links
  if (CS_To_JSON_Links(CsJson, "Links", CSPtr->Links) != RedfishCS_status_success) {goto Error;}

  // Managers 
  if (InsertJsonLinkObj (CsJson, "Managers", &CSPtr->Managers) != RedfishCS_status_success) {goto Error;}

  // Name 
  if (InsertJsonStringObj (CsJson, "Name", CSPtr->Name) != RedfishCS_status_success) {goto Error;}

  // Oem
  if (CS_To_JSON_Oem(CsJson, "Oem", CSPtr->Oem) != RedfishCS_status_success) {goto Error;}

  // PowerEquipment 
  if (InsertJsonLinkObj (CsJson, "PowerEquipment", &CSPtr->PowerEquipment) != RedfishCS_status_success) {goto Error;}

  // Product 
  if (InsertJsonStringObj (CsJson, "Product", CSPtr->Product) != RedfishCS_status_success) {goto Error;}

  // ProtocolFeaturesSupported
  if (CS_To_JSON_ProtocolFeaturesSupported(CsJson, "ProtocolFeaturesSupported", CSPtr->ProtocolFeaturesSupported) != RedfishCS_status_success) {goto Error;}

  // RedfishVersion 
  if (InsertJsonStringObj (CsJson, "RedfishVersion", CSPtr->RedfishVersion) != RedfishCS_status_success) {goto Error;}

  // Registries 
  if (InsertJsonLinkObj (CsJson, "Registries", &CSPtr->Registries) != RedfishCS_status_success) {goto Error;}

  // ResourceBlocks 
  if (InsertJsonLinkObj (CsJson, "ResourceBlocks", &CSPtr->ResourceBlocks) != RedfishCS_status_success) {goto Error;}

  // SessionService 
  if (InsertJsonLinkObj (CsJson, "SessionService", &CSPtr->SessionService) != RedfishCS_status_success) {goto Error;}

  // StorageServices 
  if (InsertJsonLinkObj (CsJson, "StorageServices", &CSPtr->StorageServices) != RedfishCS_status_success) {goto Error;}

  // StorageSystems 
  if (InsertJsonLinkObj (CsJson, "StorageSystems", &CSPtr->StorageSystems) != RedfishCS_status_success) {goto Error;}

  // Systems 
  if (InsertJsonLinkObj (CsJson, "Systems", &CSPtr->Systems) != RedfishCS_status_success) {goto Error;}

  // Tasks 
  if (InsertJsonLinkObj (CsJson, "Tasks", &CSPtr->Tasks) != RedfishCS_status_success) {goto Error;}

  // TelemetryService 
  if (InsertJsonLinkObj (CsJson, "TelemetryService", &CSPtr->TelemetryService) != RedfishCS_status_success) {goto Error;}

  // UUID 
  if (InsertJsonStringObj (CsJson, "UUID", CSPtr->UUID) != RedfishCS_status_success) {goto Error;}

  // UpdateService 
  if (InsertJsonLinkObj (CsJson, "UpdateService", &CSPtr->UpdateService) != RedfishCS_status_success) {goto Error;}

  // Vendor 
  if (InsertJsonStringObj (CsJson, "Vendor", CSPtr->Vendor) != RedfishCS_status_success) {goto Error;}

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
// Destory C Structure for ServiceRoot.V1_6_0.ServiceRoot
//
RedfishCS_status DestroyServiceRoot_V1_6_0_CS(RedfishServiceRoot_V1_6_0_ServiceRoot_CS *CSPtr)
{
  RedfishCS_status Status;

  Status = DestoryCsMemory ((RedfishCS_void *)CSPtr);
  return Status;
}

//
// Destory JSON text for ServiceRoot.V1_6_0.ServiceRoot
//
RedfishCS_status DestroyServiceRoot_V1_6_0_Json(RedfishCS_char *JsonText)
{
  free ((RedfishCS_void *)JsonText);
  return RedfishCS_status_success;
}

//
//Generate C structure for ServiceRoot.V1_6_0.ServiceRoot
//
RedfishCS_status
Json_ServiceRoot_V1_6_0_To_CS(RedfishCS_char *JsonRawText, RedfishServiceRoot_V1_6_0_ServiceRoot_CS **ReturnedCs)
{
  RedfishCS_status  Status;
  json_t *JsonObj;
  RedfishServiceRoot_V1_6_0_ServiceRoot_CS *Cs;

  Status = CreateJsonPayloadAndCs (JsonRawText, "ServiceRoot", "v1_6_0", "ServiceRoot", &JsonObj, (RedfishCS_void **)&Cs, sizeof (RedfishServiceRoot_V1_6_0_ServiceRoot_CS));
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

  // AccountService
  InitializeLinkHead (&Cs->AccountService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "AccountService", Cs->Header.ThisUri, &Cs->AccountService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // CertificateService
  InitializeLinkHead (&Cs->CertificateService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "CertificateService", Cs->Header.ThisUri, &Cs->CertificateService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Chassis
  InitializeLinkHead (&Cs->Chassis);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Chassis", Cs->Header.ThisUri, &Cs->Chassis);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // CompositionService
  InitializeLinkHead (&Cs->CompositionService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "CompositionService", Cs->Header.ThisUri, &Cs->CompositionService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Description
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Description", &Cs->Description);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // EventService
  InitializeLinkHead (&Cs->EventService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "EventService", Cs->Header.ThisUri, &Cs->EventService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Fabrics
  InitializeLinkHead (&Cs->Fabrics);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Fabrics", Cs->Header.ThisUri, &Cs->Fabrics);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Facilities
  InitializeLinkHead (&Cs->Facilities);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Facilities", Cs->Header.ThisUri, &Cs->Facilities);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Id
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Id", &Cs->Id);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // JobService
  InitializeLinkHead (&Cs->JobService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "JobService", Cs->Header.ThisUri, &Cs->JobService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // JsonSchemas
  InitializeLinkHead (&Cs->JsonSchemas);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "JsonSchemas", Cs->Header.ThisUri, &Cs->JsonSchemas);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Links
  Status = GenLinksCs (Cs, JsonObj, "Links", &Cs->Links);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Managers
  InitializeLinkHead (&Cs->Managers);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Managers", Cs->Header.ThisUri, &Cs->Managers);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Name
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Name", &Cs->Name);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){goto Error;}}

  // Oem
  Status = GenOemCs (Cs, JsonObj, "Oem", &Cs->Oem);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // PowerEquipment
  InitializeLinkHead (&Cs->PowerEquipment);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "PowerEquipment", Cs->Header.ThisUri, &Cs->PowerEquipment);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Product
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Product", &Cs->Product);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ProtocolFeaturesSupported
  Status = GenProtocolFeaturesSupportedCs (Cs, JsonObj, "ProtocolFeaturesSupported", &Cs->ProtocolFeaturesSupported);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // RedfishVersion
  Status = GetRedfishPropertyStr (Cs, JsonObj, "RedfishVersion", &Cs->RedfishVersion);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Registries
  InitializeLinkHead (&Cs->Registries);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Registries", Cs->Header.ThisUri, &Cs->Registries);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // ResourceBlocks
  InitializeLinkHead (&Cs->ResourceBlocks);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "ResourceBlocks", Cs->Header.ThisUri, &Cs->ResourceBlocks);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // SessionService
  InitializeLinkHead (&Cs->SessionService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "SessionService", Cs->Header.ThisUri, &Cs->SessionService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // StorageServices
  InitializeLinkHead (&Cs->StorageServices);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "StorageServices", Cs->Header.ThisUri, &Cs->StorageServices);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // StorageSystems
  InitializeLinkHead (&Cs->StorageSystems);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "StorageSystems", Cs->Header.ThisUri, &Cs->StorageSystems);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Systems
  InitializeLinkHead (&Cs->Systems);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Systems", Cs->Header.ThisUri, &Cs->Systems);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Tasks
  InitializeLinkHead (&Cs->Tasks);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "Tasks", Cs->Header.ThisUri, &Cs->Tasks);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // TelemetryService
  InitializeLinkHead (&Cs->TelemetryService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "TelemetryService", Cs->Header.ThisUri, &Cs->TelemetryService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UUID
  Status = GetRedfishPropertyStr (Cs, JsonObj, "UUID", &Cs->UUID);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // UpdateService
  InitializeLinkHead (&Cs->UpdateService);
  Status = CreateCsUriOrJsonByNode (Cs, JsonObj, "UpdateService", Cs->Header.ThisUri, &Cs->UpdateService);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  // Vendor
  Status = GetRedfishPropertyStr (Cs, JsonObj, "Vendor", &Cs->Vendor);
  if (Status != RedfishCS_status_success && Status != RedfishCS_status_not_found) {goto Error;}
  else {if (Status == RedfishCS_status_not_found){/*This is not the required property.*/}}

  json_decref(JsonObj);
  *ReturnedCs = Cs;
  return RedfishCS_status_success;
Error:;
  json_decref(JsonObj);
  DestroyServiceRoot_V1_6_0_CS (Cs);
  return Status;
}
