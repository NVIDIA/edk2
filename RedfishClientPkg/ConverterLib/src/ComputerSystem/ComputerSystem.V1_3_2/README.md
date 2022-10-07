# Definition of ComputerSystem.V1_3_2 and functions<br><br>

## Boot
    typedef struct _RedfishComputerSystem_V1_3_2_Boot_CS {
        RedfishCS_char *BootSourceOverrideEnabled;
        RedfishCS_char *BootSourceOverrideMode;
        RedfishCS_char *BootSourceOverrideTarget;
        RedfishCS_char *UefiTargetBootSourceOverride;
    } RedfishComputerSystem_V1_3_2_Boot_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**BootSourceOverrideEnabled**|RedfishCS_char| String pointer to **BootSourceOverrideEnabled** property.| No| No
|**BootSourceOverrideMode**|RedfishCS_char| String pointer to **BootSourceOverrideMode** property.| No| No
|**BootSourceOverrideTarget**|RedfishCS_char| String pointer to **BootSourceOverrideTarget** property.| No| No
|**UefiTargetBootSourceOverride**|RedfishCS_char| String pointer to **UefiTargetBootSourceOverride** property.| No| No


## ComputerSystem_Actions
    typedef struct _RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_CS {
        RedfishComputerSystem_V1_3_2_Reset_CS *ComputerSystem_Reset;
        RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_Oem_CS *Oem;
    } RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ComputerSystem_Reset**|RedfishComputerSystem_V1_3_2_Reset_CS| Structure points to **#ComputerSystem.Reset** property.| No| No
|**Oem**|RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_Oem_CS| Structure points to **Oem** property.| No| No


## ComputerSystem_Actions_Oem
    typedef struct _RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## ComputerSystem_Links
    typedef struct _RedfishComputerSystem_V1_3_2_ComputerSystem_Links_CS {
        RedfishCS_Link Chassis;
        RedfishCS_int64 *Chassisodata_count;
        RedfishCS_char *Chassisodata_navigationLink;
        Redfishodata_V4_0_0_idRef_CS *CooledBy;
        RedfishCS_int64 *CooledByodata_count;
        RedfishCS_char *CooledByodata_navigationLink;
        RedfishCS_Link Endpoints;
        RedfishCS_int64 *Endpointsodata_count;
        RedfishCS_char *Endpointsodata_navigationLink;
        RedfishCS_Link ManagedBy;
        RedfishCS_int64 *ManagedByodata_count;
        RedfishCS_char *ManagedByodata_navigationLink;
        RedfishResource_Oem_CS *Oem;
        Redfishodata_V4_0_0_idRef_CS *PoweredBy;
        RedfishCS_int64 *PoweredByodata_count;
        RedfishCS_char *PoweredByodata_navigationLink;
    } RedfishComputerSystem_V1_3_2_ComputerSystem_Links_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Chassis**|RedfishCS_Link| Structure link list to **Chassis** property.| No| Yes
|**Chassisodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Chassis@odata.count** property.| No| No
|**Chassisodata_navigationLink**|RedfishCS_char| String pointer to **Chassis@odata.navigationLink** property.| No| No
|**CooledBy**|Redfishodata_V4_0_0_idRef_CS| Structure points to **CooledBy** property.| No| Yes
|**CooledByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **CooledBy@odata.count** property.| No| No
|**CooledByodata_navigationLink**|RedfishCS_char| String pointer to **CooledBy@odata.navigationLink** property.| No| No
|**Endpoints**|RedfishCS_Link| Structure link list to **Endpoints** property.| No| Yes
|**Endpointsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Endpoints@odata.count** property.| No| No
|**Endpointsodata_navigationLink**|RedfishCS_char| String pointer to **Endpoints@odata.navigationLink** property.| No| No
|**ManagedBy**|RedfishCS_Link| Structure link list to **ManagedBy** property.| No| Yes
|**ManagedByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **ManagedBy@odata.count** property.| No| No
|**ManagedByodata_navigationLink**|RedfishCS_char| String pointer to **ManagedBy@odata.navigationLink** property.| No| No
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**PoweredBy**|Redfishodata_V4_0_0_idRef_CS| Structure points to **PoweredBy** property.| No| Yes
|**PoweredByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PoweredBy@odata.count** property.| No| No
|**PoweredByodata_navigationLink**|RedfishCS_char| String pointer to **PoweredBy@odata.navigationLink** property.| No| No


## HostedServices
    typedef struct _RedfishComputerSystem_V1_3_2_HostedServices_CS {
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link StorageServices;
    } RedfishComputerSystem_V1_3_2_HostedServices_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**StorageServices**|RedfishCS_Link| Structure link list to **StorageServices** property.| No| Yes


## MemorySummary
    typedef struct _RedfishComputerSystem_V1_3_2_MemorySummary_CS {
        RedfishCS_char *MemoryMirroring;
        RedfishResource_Status_CS *Status;
        RedfishCS_int64 *TotalSystemMemoryGiB;
    } RedfishComputerSystem_V1_3_2_MemorySummary_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**MemoryMirroring**|RedfishCS_char| String pointer to **MemoryMirroring** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**TotalSystemMemoryGiB**|RedfishCS_int64| 64-bit long long interger pointer to **TotalSystemMemoryGiB** property.| No| Yes


## ProcessorSummary
    typedef struct _RedfishComputerSystem_V1_3_2_ProcessorSummary_CS {
        RedfishCS_int64 *Count;
        RedfishCS_char *Model;
        RedfishResource_Status_CS *Status;
    } RedfishComputerSystem_V1_3_2_ProcessorSummary_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Count**|RedfishCS_int64| 64-bit long long interger pointer to **Count** property.| No| Yes
|**Model**|RedfishCS_char| String pointer to **Model** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No


## Reset
    typedef struct _RedfishComputerSystem_V1_3_2_Reset_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishComputerSystem_V1_3_2_Reset_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## TrustedModules
    typedef struct _RedfishComputerSystem_V1_3_2_TrustedModules_CS {
        RedfishCS_char *FirmwareVersion;
        RedfishCS_char *FirmwareVersion2;
        RedfishCS_char *InterfaceType;
        RedfishCS_char *InterfaceTypeSelection;
        RedfishResource_Oem_CS *Oem;
        RedfishResource_Status_CS *Status;
    } RedfishComputerSystem_V1_3_2_TrustedModules_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**FirmwareVersion**|RedfishCS_char| String pointer to **FirmwareVersion** property.| No| Yes
|**FirmwareVersion2**|RedfishCS_char| String pointer to **FirmwareVersion2** property.| No| Yes
|**InterfaceType**|RedfishCS_char| String pointer to **InterfaceType** property.| No| Yes
|**InterfaceTypeSelection**|RedfishCS_char| String pointer to **InterfaceTypeSelection** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No


## Condition
    typedef struct _RedfishResource_Condition_CS {
        RedfishCS_Link LogEntry;
        RedfishCS_char *Message;
        RedfishCS_char_Array *MessageArgs;
        RedfishCS_char *MessageId;
        Redfishodatav4_idRef_CS *OriginOfCondition;
        RedfishCS_char *Resolution;
        RedfishCS_char *Severity;
        RedfishCS_char *Timestamp;
    } RedfishResource_Condition_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**LogEntry**|RedfishCS_Link| Structure link list to **LogEntry** property.| No| Yes
|**Message**|RedfishCS_char| String pointer to **Message** property.| No| Yes
|**MessageArgs**|RedfishCS_char_Array| Structure array points to one or more than one **RedfishCS_char *** for property **MessageArgs**.| No| Yes
|**MessageId**|RedfishCS_char| String pointer to **MessageId** property.| No| Yes
|**OriginOfCondition**|Redfishodatav4_idRef_CS| Structure points to **OriginOfCondition** property.| No| Yes
|**Resolution**|RedfishCS_char| String pointer to **Resolution** property.| No| Yes
|**Severity**|RedfishCS_char| String pointer to **Severity** property.| No| Yes
|**Timestamp**|RedfishCS_char| String pointer to **Timestamp** property.| No| Yes


## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Status
    typedef struct _RedfishResource_Status_CS {
        RedfishResource_Condition_Array_CS *Conditions;
        RedfishCS_char *Health;
        RedfishCS_char *HealthRollup;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *State;
    } RedfishResource_Status_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Conditions**|RedfishResource_Condition_Array_CS| Structure array points to one or more than one **RedfishResource_Condition_Array_CS** structures for property **Conditions**.| No| No
|**Health**|RedfishCS_char| String pointer to **Health** property.| No| Yes
|**HealthRollup**|RedfishCS_char| String pointer to **HealthRollup** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**State**|RedfishCS_char| String pointer to **State** property.| No| Yes


## idRef
    typedef struct _Redfishodata_V4_0_0_idRef_CS {
        RedfishCS_char *odata_id;
    } Redfishodata_V4_0_0_idRef_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| No| No


## idRef
    typedef struct _Redfishodatav4_idRef_CS {
        RedfishCS_char *odata_id;
    } Redfishodatav4_idRef_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| No| No


## RedfishCS_char_Array
    typedef struct _RedfishCS_char_Array  {
        RedfishCS_Link *Next;
        RedfishCS_char *ArrayValue;
    } RedfishCS_char_Array;



## RedfishResource_Condition_Array_CS
    typedef struct _RedfishResource_Condition_Array_CS  {
        RedfishCS_Link *Next;
        RedfishResource_Condition_CS *ArrayValue;
    } RedfishResource_Condition_Array_CS;



## ComputerSystem
    typedef struct _RedfishComputerSystem_V1_3_2_ComputerSystem_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_CS *Actions;
        RedfishCS_char *AssetTag;
        RedfishCS_Link Bios;
        RedfishCS_char *BiosVersion;
        RedfishComputerSystem_V1_3_2_Boot_CS *Boot;
        RedfishCS_char *Description;
        RedfishCS_Link EthernetInterfaces;
        RedfishCS_char *HostName;
        RedfishComputerSystem_V1_3_2_HostedServices_CS *HostedServices;
        RedfishCS_char *HostingRoles;
        RedfishCS_char *Id;
        RedfishCS_char *IndicatorLED;
        RedfishComputerSystem_V1_3_2_ComputerSystem_Links_CS *Links;
        RedfishCS_Link LogServices;
        RedfishCS_char *Manufacturer;
        RedfishCS_Link Memory;
        RedfishCS_Link MemoryDomains;
        RedfishComputerSystem_V1_3_2_MemorySummary_CS *MemorySummary;
        RedfishCS_char *Model;
        RedfishCS_char *Name;
        RedfishCS_Link NetworkInterfaces;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link PCIeDevices;
        RedfishCS_int64 *PCIeDevicesodata_count;
        RedfishCS_char *PCIeDevicesodata_navigationLink;
        RedfishCS_Link PCIeFunctions;
        RedfishCS_int64 *PCIeFunctionsodata_count;
        RedfishCS_char *PCIeFunctionsodata_navigationLink;
        RedfishCS_char *PartNumber;
        RedfishCS_char *PowerState;
        RedfishComputerSystem_V1_3_2_ProcessorSummary_CS *ProcessorSummary;
        RedfishCS_Link Processors;
        RedfishCS_char *SKU;
        RedfishCS_Link SecureBoot;
        RedfishCS_char *SerialNumber;
        RedfishCS_Link SimpleStorage;
        RedfishResource_Status_CS *Status;
        RedfishCS_Link Storage;
        RedfishCS_char *SystemType;
        RedfishComputerSystem_V1_3_2_TrustedModules_CS *TrustedModules;
        RedfishCS_char *UUID;
    } RedfishComputerSystem_V1_3_2_ComputerSystem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| No| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| No| No
|**Actions**|RedfishComputerSystem_V1_3_2_ComputerSystem_Actions_CS| Structure points to **Actions** property.| No| No
|**AssetTag**|RedfishCS_char| String pointer to **AssetTag** property.| No| No
|**Bios**|RedfishCS_Link| Structure link list to **Bios** property.| No| Yes
|**BiosVersion**|RedfishCS_char| String pointer to **BiosVersion** property.| No| Yes
|**Boot**|RedfishComputerSystem_V1_3_2_Boot_CS| Structure points to **Boot** property.| No| No
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**EthernetInterfaces**|RedfishCS_Link| Structure link list to **EthernetInterfaces** property.| No| Yes
|**HostName**|RedfishCS_char| String pointer to **HostName** property.| No| No
|**HostedServices**|RedfishComputerSystem_V1_3_2_HostedServices_CS| Structure points to **HostedServices** property.| No| No
|**HostingRoles**|RedfishCS_char| String pointer to **HostingRoles** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**IndicatorLED**|RedfishCS_char| String pointer to **IndicatorLED** property.| No| No
|**Links**|RedfishComputerSystem_V1_3_2_ComputerSystem_Links_CS| Structure points to **Links** property.| No| No
|**LogServices**|RedfishCS_Link| Structure link list to **LogServices** property.| No| Yes
|**Manufacturer**|RedfishCS_char| String pointer to **Manufacturer** property.| No| Yes
|**Memory**|RedfishCS_Link| Structure link list to **Memory** property.| No| Yes
|**MemoryDomains**|RedfishCS_Link| Structure link list to **MemoryDomains** property.| No| Yes
|**MemorySummary**|RedfishComputerSystem_V1_3_2_MemorySummary_CS| Structure points to **MemorySummary** property.| No| No
|**Model**|RedfishCS_char| String pointer to **Model** property.| No| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**NetworkInterfaces**|RedfishCS_Link| Structure link list to **NetworkInterfaces** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**PCIeDevices**|RedfishCS_Link| Structure link list to **PCIeDevices** property.| No| Yes
|**PCIeDevicesodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PCIeDevices@odata.count** property.| No| No
|**PCIeDevicesodata_navigationLink**|RedfishCS_char| String pointer to **PCIeDevices@odata.navigationLink** property.| No| No
|**PCIeFunctions**|RedfishCS_Link| Structure link list to **PCIeFunctions** property.| No| Yes
|**PCIeFunctionsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PCIeFunctions@odata.count** property.| No| No
|**PCIeFunctionsodata_navigationLink**|RedfishCS_char| String pointer to **PCIeFunctions@odata.navigationLink** property.| No| No
|**PartNumber**|RedfishCS_char| String pointer to **PartNumber** property.| No| Yes
|**PowerState**|RedfishCS_char| String pointer to **PowerState** property.| No| Yes
|**ProcessorSummary**|RedfishComputerSystem_V1_3_2_ProcessorSummary_CS| Structure points to **ProcessorSummary** property.| No| No
|**Processors**|RedfishCS_Link| Structure link list to **Processors** property.| No| Yes
|**SKU**|RedfishCS_char| String pointer to **SKU** property.| No| Yes
|**SecureBoot**|RedfishCS_Link| Structure link list to **SecureBoot** property.| No| Yes
|**SerialNumber**|RedfishCS_char| String pointer to **SerialNumber** property.| No| Yes
|**SimpleStorage**|RedfishCS_Link| Structure link list to **SimpleStorage** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**Storage**|RedfishCS_Link| Structure link list to **Storage** property.| No| Yes
|**SystemType**|RedfishCS_char| String pointer to **SystemType** property.| No| Yes
|**TrustedModules**|RedfishComputerSystem_V1_3_2_TrustedModules_CS| Structure points to **TrustedModules** property.| No| No
|**UUID**|RedfishCS_char| String pointer to **UUID** property.| No| Yes
## Redfish ComputerSystem V1_3_2 to C Structure Function
    RedfishCS_status
    Json_ComputerSystem_V1_3_2_To_CS (RedfishCS_char *JsonRawText, RedfishComputerSystem_V1_3_2_ComputerSystem_CS **ReturnedCS);

## C Structure to Redfish ComputerSystem V1_3_2 JSON Function
    RedfishCS_status
    CS_To_ComputerSystem_V1_3_2_JSON (RedfishComputerSystem_V1_3_2_ComputerSystem_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish ComputerSystem V1_3_2 C Structure Function
    RedfishCS_status
    DestroyComputerSystem_V1_3_2_CS (RedfishComputerSystem_V1_3_2_ComputerSystem_CS *CSPtr);

