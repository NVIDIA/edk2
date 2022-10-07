# Definition of SecureBootDatabase.V1_0_0 and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Actions
    typedef struct _RedfishSecureBootDatabase_V1_0_0_Actions_CS {
        RedfishSecureBootDatabase_V1_0_0_ResetKeys_CS *SecureBootDatabase_ResetKeys;
        RedfishSecureBootDatabase_V1_0_0_OemActions_CS *Oem;
    } RedfishSecureBootDatabase_V1_0_0_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**SecureBootDatabase_ResetKeys**|RedfishSecureBootDatabase_V1_0_0_ResetKeys_CS| Structure points to **#SecureBootDatabase.ResetKeys** property.| No| No
|**Oem**|RedfishSecureBootDatabase_V1_0_0_OemActions_CS| Structure points to **Oem** property.| No| No


## OemActions
    typedef struct _RedfishSecureBootDatabase_V1_0_0_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishSecureBootDatabase_V1_0_0_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## ResetKeys
    typedef struct _RedfishSecureBootDatabase_V1_0_0_ResetKeys_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishSecureBootDatabase_V1_0_0_ResetKeys_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## SecureBootDatabase
    typedef struct _RedfishSecureBootDatabase_V1_0_0_SecureBootDatabase_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishSecureBootDatabase_V1_0_0_Actions_CS *Actions;
        RedfishCS_Link Certificates;
        RedfishCS_char *DatabaseId;
        RedfishCS_char *Description;
        RedfishCS_char *Id;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link Signatures;
    } RedfishSecureBootDatabase_V1_0_0_SecureBootDatabase_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishSecureBootDatabase_V1_0_0_Actions_CS| Structure points to **Actions** property.| No| No
|**Certificates**|RedfishCS_Link| Structure link list to **Certificates** property.| No| Yes
|**DatabaseId**|RedfishCS_char| String pointer to **DatabaseId** property.| No| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Signatures**|RedfishCS_Link| Structure link list to **Signatures** property.| No| Yes
## Redfish SecureBootDatabase V1_0_0 to C Structure Function
    RedfishCS_status
    Json_SecureBootDatabase_V1_0_0_To_CS (RedfishCS_char *JsonRawText, RedfishSecureBootDatabase_V1_0_0_SecureBootDatabase_CS **ReturnedCS);

## C Structure to Redfish SecureBootDatabase V1_0_0 JSON Function
    RedfishCS_status
    CS_To_SecureBootDatabase_V1_0_0_JSON (RedfishSecureBootDatabase_V1_0_0_SecureBootDatabase_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish SecureBootDatabase V1_0_0 C Structure Function
    RedfishCS_status
    DestroySecureBootDatabase_V1_0_0_CS (RedfishSecureBootDatabase_V1_0_0_SecureBootDatabase_CS *CSPtr);

