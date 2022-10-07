# Definition of BootOption.V1_0_3 and functions<br><br>

## Actions
    typedef struct _RedfishBootOption_V1_0_3_Actions_CS {
        RedfishBootOption_V1_0_3_OemActions_CS *Oem;
    } RedfishBootOption_V1_0_3_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishBootOption_V1_0_3_OemActions_CS| Structure points to **Oem** property.| No| No


## OemActions
    typedef struct _RedfishBootOption_V1_0_3_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishBootOption_V1_0_3_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## idRef
    typedef struct _Redfishodatav4_idRef_CS {
        RedfishCS_char *odata_id;
    } Redfishodatav4_idRef_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No


## BootOption
    typedef struct _RedfishBootOption_V1_0_3_BootOption_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishBootOption_V1_0_3_Actions_CS *Actions;
        RedfishCS_char *Alias;
        RedfishCS_bool *BootOptionEnabled;
        RedfishCS_char *BootOptionReference;
        RedfishCS_char *Description;
        RedfishCS_char *DisplayName;
        RedfishCS_char *Id;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        Redfishodatav4_idRef_CS *RelatedItem;
        RedfishCS_int64 *RelatedItemodata_count;
        RedfishCS_char *UefiDevicePath;
    } RedfishBootOption_V1_0_3_BootOption_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishBootOption_V1_0_3_Actions_CS| Structure points to **Actions** property.| No| No
|**Alias**|RedfishCS_char| String pointer to **Alias** property.| No| Yes
|**BootOptionEnabled**|RedfishCS_bool| Boolean pointer to **BootOptionEnabled** property.| No| No
|**BootOptionReference**|RedfishCS_char| String pointer to **BootOptionReference** property.| Yes| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**DisplayName**|RedfishCS_char| String pointer to **DisplayName** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**RelatedItem**|Redfishodatav4_idRef_CS| Structure points to **RelatedItem** property.| No| Yes
|**RelatedItemodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **RelatedItem@odata.count** property.| No| No
|**UefiDevicePath**|RedfishCS_char| String pointer to **UefiDevicePath** property.| No| Yes
## Redfish BootOption V1_0_3 to C Structure Function
    RedfishCS_status
    Json_BootOption_V1_0_3_To_CS (RedfishCS_char *JsonRawText, RedfishBootOption_V1_0_3_BootOption_CS **ReturnedCS);

## C Structure to Redfish BootOption V1_0_3 JSON Function
    RedfishCS_status
    CS_To_BootOption_V1_0_3_JSON (RedfishBootOption_V1_0_3_BootOption_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish BootOption V1_0_3 C Structure Function
    RedfishCS_status
    DestroyBootOption_V1_0_3_CS (RedfishBootOption_V1_0_3_BootOption_CS *CSPtr);

