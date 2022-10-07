# Definition of ServiceRoot.V1_4_3 and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Expand
    typedef struct _RedfishServiceRoot_V1_4_3_Expand_CS {
        RedfishCS_bool *ExpandAll;
        RedfishCS_bool *Levels;
        RedfishCS_bool *Links;
        RedfishCS_int64 *MaxLevels;
        RedfishCS_bool *NoLinks;
    } RedfishServiceRoot_V1_4_3_Expand_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ExpandAll**|RedfishCS_bool| Boolean pointer to **ExpandAll** property.| No| Yes
|**Levels**|RedfishCS_bool| Boolean pointer to **Levels** property.| No| Yes
|**Links**|RedfishCS_bool| Boolean pointer to **Links** property.| Yes| Yes
|**MaxLevels**|RedfishCS_int64| 64-bit long long interger pointer to **MaxLevels** property.| No| Yes
|**NoLinks**|RedfishCS_bool| Boolean pointer to **NoLinks** property.| No| Yes


## Links
    typedef struct _RedfishServiceRoot_V1_4_3_Links_CS {
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link Sessions;
    } RedfishServiceRoot_V1_4_3_Links_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Sessions**|RedfishCS_Link| Structure link list to **Sessions** property.| No| Yes


## ProtocolFeaturesSupported
    typedef struct _RedfishServiceRoot_V1_4_3_ProtocolFeaturesSupported_CS {
        RedfishCS_bool *ExcerptQuery;
        RedfishServiceRoot_V1_4_3_Expand_CS *ExpandQuery;
        RedfishCS_bool *FilterQuery;
        RedfishCS_bool *OnlyMemberQuery;
        RedfishCS_bool *SelectQuery;
    } RedfishServiceRoot_V1_4_3_ProtocolFeaturesSupported_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ExcerptQuery**|RedfishCS_bool| Boolean pointer to **ExcerptQuery** property.| No| Yes
|**ExpandQuery**|RedfishServiceRoot_V1_4_3_Expand_CS| Structure points to **ExpandQuery** property.| No| No
|**FilterQuery**|RedfishCS_bool| Boolean pointer to **FilterQuery** property.| No| Yes
|**OnlyMemberQuery**|RedfishCS_bool| Boolean pointer to **OnlyMemberQuery** property.| No| Yes
|**SelectQuery**|RedfishCS_bool| Boolean pointer to **SelectQuery** property.| No| Yes


## ServiceRoot
    typedef struct _RedfishServiceRoot_V1_4_3_ServiceRoot_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishCS_Link AccountService;
        RedfishCS_Link Chassis;
        RedfishCS_Link CompositionService;
        RedfishCS_char *Description;
        RedfishCS_Link EventService;
        RedfishCS_Link Fabrics;
        RedfishCS_char *Id;
        RedfishCS_Link JobService;
        RedfishCS_Link JsonSchemas;
        RedfishServiceRoot_V1_4_3_Links_CS *Links;
        RedfishCS_Link Managers;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *Product;
        RedfishServiceRoot_V1_4_3_ProtocolFeaturesSupported_CS *ProtocolFeaturesSupported;
        RedfishCS_char *RedfishVersion;
        RedfishCS_Link Registries;
        RedfishCS_Link SessionService;
        RedfishCS_Link StorageServices;
        RedfishCS_Link StorageSystems;
        RedfishCS_Link Systems;
        RedfishCS_Link Tasks;
        RedfishCS_Link TelemetryService;
        RedfishCS_char *UUID;
        RedfishCS_Link UpdateService;
    } RedfishServiceRoot_V1_4_3_ServiceRoot_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**AccountService**|RedfishCS_Link| Structure link list to **AccountService** property.| No| Yes
|**Chassis**|RedfishCS_Link| Structure link list to **Chassis** property.| No| Yes
|**CompositionService**|RedfishCS_Link| Structure link list to **CompositionService** property.| No| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**EventService**|RedfishCS_Link| Structure link list to **EventService** property.| No| Yes
|**Fabrics**|RedfishCS_Link| Structure link list to **Fabrics** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**JobService**|RedfishCS_Link| Structure link list to **JobService** property.| No| Yes
|**JsonSchemas**|RedfishCS_Link| Structure link list to **JsonSchemas** property.| No| Yes
|**Links**|RedfishServiceRoot_V1_4_3_Links_CS| Structure points to **Links** property.| Yes| No
|**Managers**|RedfishCS_Link| Structure link list to **Managers** property.| No| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Product**|RedfishCS_char| String pointer to **Product** property.| No| Yes
|**ProtocolFeaturesSupported**|RedfishServiceRoot_V1_4_3_ProtocolFeaturesSupported_CS| Structure points to **ProtocolFeaturesSupported** property.| No| No
|**RedfishVersion**|RedfishCS_char| String pointer to **RedfishVersion** property.| No| Yes
|**Registries**|RedfishCS_Link| Structure link list to **Registries** property.| No| Yes
|**SessionService**|RedfishCS_Link| Structure link list to **SessionService** property.| No| Yes
|**StorageServices**|RedfishCS_Link| Structure link list to **StorageServices** property.| No| Yes
|**StorageSystems**|RedfishCS_Link| Structure link list to **StorageSystems** property.| No| Yes
|**Systems**|RedfishCS_Link| Structure link list to **Systems** property.| No| Yes
|**Tasks**|RedfishCS_Link| Structure link list to **Tasks** property.| No| Yes
|**TelemetryService**|RedfishCS_Link| Structure link list to **TelemetryService** property.| No| Yes
|**UUID**|RedfishCS_char| String pointer to **UUID** property.| No| Yes
|**UpdateService**|RedfishCS_Link| Structure link list to **UpdateService** property.| No| Yes
## Redfish ServiceRoot V1_4_3 to C Structure Function
    RedfishCS_status
    Json_ServiceRoot_V1_4_3_To_CS (RedfishCS_char *JsonRawText, RedfishServiceRoot_V1_4_3_ServiceRoot_CS **ReturnedCS);

## C Structure to Redfish ServiceRoot V1_4_3 JSON Function
    RedfishCS_status
    CS_To_ServiceRoot_V1_4_3_JSON (RedfishServiceRoot_V1_4_3_ServiceRoot_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish ServiceRoot V1_4_3 C Structure Function
    RedfishCS_status
    DestroyServiceRoot_V1_4_3_CS (RedfishServiceRoot_V1_4_3_ServiceRoot_CS *CSPtr);

