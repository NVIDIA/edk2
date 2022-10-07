# Definition of redfisherror.V1_0_1 and functions<br><br>

## RedfishErrorContents
    typedef struct _Redfishredfisherror_V1_0_1_RedfishErrorContents_CS {
        RedfishCS_Link Prop;
    } Redfishredfisherror_V1_0_1_RedfishErrorContents_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## RedfishError
    typedef struct _Redfishredfisherror_V1_0_1_RedfishError_CS {
        RedfishCS_Header     Header;
        RedfishCS_Link Prop;
    } Redfishredfisherror_V1_0_1_RedfishError_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---
## Redfish redfisherror V1_0_1 to C Structure Function
    RedfishCS_status
    Json_redfisherror_V1_0_1_To_CS (RedfishCS_char *JsonRawText, Redfishredfisherror_V1_0_1_RedfishError_CS **ReturnedCS);

## C Structure to Redfish redfisherror V1_0_1 JSON Function
    RedfishCS_status
    CS_To_redfisherror_V1_0_1_JSON (Redfishredfisherror_V1_0_1_RedfishError_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish redfisherror V1_0_1 C Structure Function
    RedfishCS_status
    Destroyredfisherror_V1_0_1_CS (Redfishredfisherror_V1_0_1_RedfishError_CS *CSPtr);

