# Definition of ServiceRoot.V1_0_4 and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## ServiceRoot_Links
    typedef struct _RedfishServiceRoot_V1_0_4_ServiceRoot_Links_CS {
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link Sessions;
    } RedfishServiceRoot_V1_0_4_ServiceRoot_Links_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Sessions**|RedfishCS_Link| Structure link list to **Sessions** property.| No| Yes


## ServiceRoot
    typedef struct _RedfishServiceRoot_V1_0_4_ServiceRoot_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishCS_Link AccountService;
        RedfishCS_Link Chassis;
        RedfishCS_char *Description;
        RedfishCS_Link EventService;
        RedfishCS_char *Id;
        RedfishCS_Link JsonSchemas;
        RedfishServiceRoot_V1_0_4_ServiceRoot_Links_CS *Links;
        RedfishCS_Link Managers;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *RedfishVersion;
        RedfishCS_Link Registries;
        RedfishCS_Link SessionService;
        RedfishCS_Link Systems;
        RedfishCS_Link Tasks;
        RedfishCS_char *UUID;
    } RedfishServiceRoot_V1_0_4_ServiceRoot_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| No| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| No| No
|**AccountService**|RedfishCS_Link| Structure link list to **AccountService** property.| No| Yes
|**Chassis**|RedfishCS_Link| Structure link list to **Chassis** property.| No| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**EventService**|RedfishCS_Link| Structure link list to **EventService** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**JsonSchemas**|RedfishCS_Link| Structure link list to **JsonSchemas** property.| No| Yes
|**Links**|RedfishServiceRoot_V1_0_4_ServiceRoot_Links_CS| Structure points to **Links** property.| Yes| No
|**Managers**|RedfishCS_Link| Structure link list to **Managers** property.| No| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**RedfishVersion**|RedfishCS_char| String pointer to **RedfishVersion** property.| No| Yes
|**Registries**|RedfishCS_Link| Structure link list to **Registries** property.| No| Yes
|**SessionService**|RedfishCS_Link| Structure link list to **SessionService** property.| No| Yes
|**Systems**|RedfishCS_Link| Structure link list to **Systems** property.| No| Yes
|**Tasks**|RedfishCS_Link| Structure link list to **Tasks** property.| No| Yes
|**UUID**|RedfishCS_char| String pointer to **UUID** property.| No| Yes
## Redfish ServiceRoot V1_0_4 to C Structure Function
    RedfishCS_status
    Json_ServiceRoot_V1_0_4_To_CS (RedfishCS_char *JsonRawText, RedfishServiceRoot_V1_0_4_ServiceRoot_CS **ReturnedCS);

## C Structure to Redfish ServiceRoot V1_0_4 JSON Function
    RedfishCS_status
    CS_To_ServiceRoot_V1_0_4_JSON (RedfishServiceRoot_V1_0_4_ServiceRoot_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish ServiceRoot V1_0_4 C Structure Function
    RedfishCS_status
    DestroyServiceRoot_V1_0_4_CS (RedfishServiceRoot_V1_0_4_ServiceRoot_CS *CSPtr);

