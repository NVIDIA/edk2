# Definition of Signature.V1_0_0 and functions<br><br>

## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Actions
    typedef struct _RedfishSignature_V1_0_0_Actions_CS {
        RedfishSignature_V1_0_0_OemActions_CS *Oem;
    } RedfishSignature_V1_0_0_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishSignature_V1_0_0_OemActions_CS| Structure points to **Oem** property.| No| No


## OemActions
    typedef struct _RedfishSignature_V1_0_0_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishSignature_V1_0_0_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Signature
    typedef struct _RedfishSignature_V1_0_0_Signature_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishSignature_V1_0_0_Actions_CS *Actions;
        RedfishCS_char *Description;
        RedfishCS_char *Id;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *SignatureString;
        RedfishCS_char *SignatureType;
        RedfishCS_char *SignatureTypeRegistry;
        RedfishCS_char *UefiSignatureOwner;
    } RedfishSignature_V1_0_0_Signature_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishSignature_V1_0_0_Actions_CS| Structure points to **Actions** property.| No| No
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**SignatureString**|RedfishCS_char| String pointer to **SignatureString** property.| No| Yes
|**SignatureType**|RedfishCS_char| String pointer to **SignatureType** property.| No| Yes
|**SignatureTypeRegistry**|RedfishCS_char| String pointer to **SignatureTypeRegistry** property.| No| Yes
|**UefiSignatureOwner**|RedfishCS_char| String pointer to **UefiSignatureOwner** property.| No| Yes
## Redfish Signature V1_0_0 to C Structure Function
    RedfishCS_status
    Json_Signature_V1_0_0_To_CS (RedfishCS_char *JsonRawText, RedfishSignature_V1_0_0_Signature_CS **ReturnedCS);

## C Structure to Redfish Signature V1_0_0 JSON Function
    RedfishCS_status
    CS_To_Signature_V1_0_0_JSON (RedfishSignature_V1_0_0_Signature_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish Signature V1_0_0 C Structure Function
    RedfishCS_status
    DestroySignature_V1_0_0_CS (RedfishSignature_V1_0_0_Signature_CS *CSPtr);

