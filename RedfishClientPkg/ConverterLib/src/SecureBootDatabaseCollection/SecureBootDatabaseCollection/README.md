# Definition of SecureBootDatabaseCollection and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## SecureBootDatabaseCollection
    typedef struct _RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishCS_char *Description;
        RedfishCS_Link Members;
        RedfishCS_int64 *Membersodata_count;
        RedfishCS_char *Membersodata_nextLink;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
    } RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Members**|RedfishCS_Link| Structure link list to **Members** property.| Yes| Yes
|**Membersodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Members@odata.count** property.| Yes| No
|**Membersodata_nextLink**|RedfishCS_char| String pointer to **Members@odata.nextLink** property.| No| No
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
## Redfish SecureBootDatabaseCollection  to C Structure Function
    RedfishCS_status
    Json_SecureBootDatabaseCollection_To_CS (RedfishCS_char *JsonRawText, RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS **ReturnedCS);

## C Structure to Redfish SecureBootDatabaseCollection  JSON Function
    RedfishCS_status
    CS_To_SecureBootDatabaseCollection_JSON (RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish SecureBootDatabaseCollection  C Structure Function
    RedfishCS_status
    DestroySecureBootDatabaseCollection_CS (RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS *CSPtr);

