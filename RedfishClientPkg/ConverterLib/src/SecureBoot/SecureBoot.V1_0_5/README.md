# Definition of SecureBoot.V1_0_5 and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Actions
    typedef struct _RedfishSecureBoot_V1_0_5_Actions_CS {
        RedfishSecureBoot_V1_0_5_ResetKeys_CS *SecureBoot_ResetKeys;
        RedfishSecureBoot_V1_0_5_OemActions_CS *Oem;
    } RedfishSecureBoot_V1_0_5_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**SecureBoot_ResetKeys**|RedfishSecureBoot_V1_0_5_ResetKeys_CS| Structure points to **#SecureBoot.ResetKeys** property.| No| No
|**Oem**|RedfishSecureBoot_V1_0_5_OemActions_CS| Structure points to **Oem** property.| No| No


## OemActions
    typedef struct _RedfishSecureBoot_V1_0_5_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishSecureBoot_V1_0_5_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## ResetKeys
    typedef struct _RedfishSecureBoot_V1_0_5_ResetKeys_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishSecureBoot_V1_0_5_ResetKeys_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## SecureBoot
    typedef struct _RedfishSecureBoot_V1_0_5_SecureBoot_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishSecureBoot_V1_0_5_Actions_CS *Actions;
        RedfishCS_char *Description;
        RedfishCS_char *Id;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *SecureBootCurrentBoot;
        RedfishCS_bool *SecureBootEnable;
        RedfishCS_char *SecureBootMode;
    } RedfishSecureBoot_V1_0_5_SecureBoot_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishSecureBoot_V1_0_5_Actions_CS| Structure points to **Actions** property.| No| No
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**SecureBootCurrentBoot**|RedfishCS_char| String pointer to **SecureBootCurrentBoot** property.| No| Yes
|**SecureBootEnable**|RedfishCS_bool| Boolean pointer to **SecureBootEnable** property.| No| No
|**SecureBootMode**|RedfishCS_char| String pointer to **SecureBootMode** property.| No| Yes
## Redfish SecureBoot V1_0_5 to C Structure Function
    RedfishCS_status
    Json_SecureBoot_V1_0_5_To_CS (RedfishCS_char *JsonRawText, RedfishSecureBoot_V1_0_5_SecureBoot_CS **ReturnedCS);

## C Structure to Redfish SecureBoot V1_0_5 JSON Function
    RedfishCS_status
    CS_To_SecureBoot_V1_0_5_JSON (RedfishSecureBoot_V1_0_5_SecureBoot_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish SecureBoot V1_0_5 C Structure Function
    RedfishCS_status
    DestroySecureBoot_V1_0_5_CS (RedfishSecureBoot_V1_0_5_SecureBoot_CS *CSPtr);

