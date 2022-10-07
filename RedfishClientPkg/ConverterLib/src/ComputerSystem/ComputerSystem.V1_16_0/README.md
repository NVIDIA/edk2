# Definition of ComputerSystem.V1_16_0 and functions<br><br>

## Actions
    typedef struct _RedfishComputerSystem_V1_16_0_Actions_CS {
        RedfishComputerSystem_V1_16_0_AddResourceBlock_CS *ComputerSystem_AddResourceBlock;
        RedfishComputerSystem_V1_16_0_RemoveResourceBlock_CS *ComputerSystem_RemoveResourceBlock;
        RedfishComputerSystem_V1_16_0_Reset_CS *ComputerSystem_Reset;
        RedfishComputerSystem_V1_16_0_SetDefaultBootOrder_CS *ComputerSystem_SetDefaultBootOrder;
        RedfishComputerSystem_V1_16_0_OemActions_CS *Oem;
    } RedfishComputerSystem_V1_16_0_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ComputerSystem_AddResourceBlock**|RedfishComputerSystem_V1_16_0_AddResourceBlock_CS| Structure points to **#ComputerSystem.AddResourceBlock** property.| No| No
|**ComputerSystem_RemoveResourceBlock**|RedfishComputerSystem_V1_16_0_RemoveResourceBlock_CS| Structure points to **#ComputerSystem.RemoveResourceBlock** property.| No| No
|**ComputerSystem_Reset**|RedfishComputerSystem_V1_16_0_Reset_CS| Structure points to **#ComputerSystem.Reset** property.| No| No
|**ComputerSystem_SetDefaultBootOrder**|RedfishComputerSystem_V1_16_0_SetDefaultBootOrder_CS| Structure points to **#ComputerSystem.SetDefaultBootOrder** property.| No| No
|**Oem**|RedfishComputerSystem_V1_16_0_OemActions_CS| Structure points to **Oem** property.| No| No


## AddResourceBlock
    typedef struct _RedfishComputerSystem_V1_16_0_AddResourceBlock_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishComputerSystem_V1_16_0_AddResourceBlock_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## Boot
    typedef struct _RedfishComputerSystem_V1_16_0_Boot_CS {
        RedfishCS_char_Array *AliasBootOrder;
        RedfishCS_int64 *AutomaticRetryAttempts;
        RedfishCS_char *AutomaticRetryConfig;
        RedfishCS_char *BootNext;
        RedfishCS_Link BootOptions;
        RedfishCS_char_Array *BootOrder;
        RedfishCS_char *BootOrderPropertySelection;
        RedfishCS_char *BootSourceOverrideEnabled;
        RedfishCS_char *BootSourceOverrideMode;
        RedfishCS_char *BootSourceOverrideTarget;
        RedfishCS_Link Certificates;
        RedfishCS_char *HttpBootUri;
        RedfishCS_int64 *RemainingAutomaticRetryAttempts;
        RedfishCS_char *StopBootOnFault;
        RedfishCS_char *TrustedModuleRequiredToBoot;
        RedfishCS_char *UefiTargetBootSourceOverride;
    } RedfishComputerSystem_V1_16_0_Boot_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**AliasBootOrder**|RedfishCS_char_Array| Structure array points to one or more than one **RedfishCS_char *** for property **AliasBootOrder**.| No| No
|**AutomaticRetryAttempts**|RedfishCS_int64| 64-bit long long interger pointer to **AutomaticRetryAttempts** property.| No| No
|**AutomaticRetryConfig**|RedfishCS_char| String pointer to **AutomaticRetryConfig** property.| No| No
|**BootNext**|RedfishCS_char| String pointer to **BootNext** property.| No| No
|**BootOptions**|RedfishCS_Link| Structure link list to **BootOptions** property.| No| Yes
|**BootOrder**|RedfishCS_char_Array| Structure array points to one or more than one **RedfishCS_char *** for property **BootOrder**.| No| No
|**BootOrderPropertySelection**|RedfishCS_char| String pointer to **BootOrderPropertySelection** property.| No| No
|**BootSourceOverrideEnabled**|RedfishCS_char| String pointer to **BootSourceOverrideEnabled** property.| No| No
|**BootSourceOverrideMode**|RedfishCS_char| String pointer to **BootSourceOverrideMode** property.| No| No
|**BootSourceOverrideTarget**|RedfishCS_char| String pointer to **BootSourceOverrideTarget** property.| No| No
|**Certificates**|RedfishCS_Link| Structure link list to **Certificates** property.| No| Yes
|**HttpBootUri**|RedfishCS_char| String pointer to **HttpBootUri** property.| No| No
|**RemainingAutomaticRetryAttempts**|RedfishCS_int64| 64-bit long long interger pointer to **RemainingAutomaticRetryAttempts** property.| No| Yes
|**StopBootOnFault**|RedfishCS_char| String pointer to **StopBootOnFault** property.| No| No
|**TrustedModuleRequiredToBoot**|RedfishCS_char| String pointer to **TrustedModuleRequiredToBoot** property.| No| No
|**UefiTargetBootSourceOverride**|RedfishCS_char| String pointer to **UefiTargetBootSourceOverride** property.| No| No


## BootProgress
    typedef struct _RedfishComputerSystem_V1_16_0_BootProgress_CS {
        RedfishCS_char *LastState;
        RedfishCS_char *LastStateTime;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *OemLastState;
    } RedfishComputerSystem_V1_16_0_BootProgress_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**LastState**|RedfishCS_char| String pointer to **LastState** property.| No| Yes
|**LastStateTime**|RedfishCS_char| String pointer to **LastStateTime** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**OemLastState**|RedfishCS_char| String pointer to **OemLastState** property.| No| Yes


## HostGraphicalConsole
    typedef struct _RedfishComputerSystem_V1_16_0_HostGraphicalConsole_CS {
        RedfishCS_char_Array *ConnectTypesSupported;
        RedfishCS_int64 *MaxConcurrentSessions;
        RedfishCS_int64 *Port;
        RedfishCS_bool *ServiceEnabled;
    } RedfishComputerSystem_V1_16_0_HostGraphicalConsole_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ConnectTypesSupported**|RedfishCS_char_Array| Structure array points to one or more than one **RedfishCS_char *** for property **ConnectTypesSupported**.| No| Yes
|**MaxConcurrentSessions**|RedfishCS_int64| 64-bit long long interger pointer to **MaxConcurrentSessions** property.| No| Yes
|**Port**|RedfishCS_int64| 64-bit long long interger pointer to **Port** property.| No| No
|**ServiceEnabled**|RedfishCS_bool| Boolean pointer to **ServiceEnabled** property.| No| No


## HostSerialConsole
    typedef struct _RedfishComputerSystem_V1_16_0_HostSerialConsole_CS {
        RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS *IPMI;
        RedfishCS_int64 *MaxConcurrentSessions;
        RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS *SSH;
        RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS *Telnet;
    } RedfishComputerSystem_V1_16_0_HostSerialConsole_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**IPMI**|RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS| Structure points to **IPMI** property.| No| No
|**MaxConcurrentSessions**|RedfishCS_int64| 64-bit long long interger pointer to **MaxConcurrentSessions** property.| No| Yes
|**SSH**|RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS| Structure points to **SSH** property.| No| No
|**Telnet**|RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS| Structure points to **Telnet** property.| No| No


## HostedServices
    typedef struct _RedfishComputerSystem_V1_16_0_HostedServices_CS {
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link StorageServices;
    } RedfishComputerSystem_V1_16_0_HostedServices_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**StorageServices**|RedfishCS_Link| Structure link list to **StorageServices** property.| No| Yes


## IdlePowerSaver
    typedef struct _RedfishComputerSystem_V1_16_0_IdlePowerSaver_CS {
        RedfishCS_bool *Enabled;
        RedfishCS_int64 *EnterDwellTimeSeconds;
        RedfishCS_int64 *EnterUtilizationPercent;
        RedfishCS_int64 *ExitDwellTimeSeconds;
        RedfishCS_int64 *ExitUtilizationPercent;
    } RedfishComputerSystem_V1_16_0_IdlePowerSaver_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Enabled**|RedfishCS_bool| Boolean pointer to **Enabled** property.| No| No
|**EnterDwellTimeSeconds**|RedfishCS_int64| 64-bit long long interger pointer to **EnterDwellTimeSeconds** property.| No| No
|**EnterUtilizationPercent**|RedfishCS_int64| 64-bit long long interger pointer to **EnterUtilizationPercent** property.| No| No
|**ExitDwellTimeSeconds**|RedfishCS_int64| 64-bit long long interger pointer to **ExitDwellTimeSeconds** property.| No| No
|**ExitUtilizationPercent**|RedfishCS_int64| 64-bit long long interger pointer to **ExitUtilizationPercent** property.| No| No


## KMIPServer
    typedef struct _RedfishComputerSystem_V1_16_0_KMIPServer_CS {
        RedfishCS_char *Address;
        RedfishCS_char *Password;
        RedfishCS_int64 *Port;
        RedfishCS_char *Username;
    } RedfishComputerSystem_V1_16_0_KMIPServer_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Address**|RedfishCS_char| String pointer to **Address** property.| No| No
|**Password**|RedfishCS_char| String pointer to **Password** property.| No| No
|**Port**|RedfishCS_int64| 64-bit long long interger pointer to **Port** property.| No| No
|**Username**|RedfishCS_char| String pointer to **Username** property.| No| No


## KeyManagement
    typedef struct _RedfishComputerSystem_V1_16_0_KeyManagement_CS {
        RedfishCS_Link KMIPCertificates;
        RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS *KMIPServers;
    } RedfishComputerSystem_V1_16_0_KeyManagement_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**KMIPCertificates**|RedfishCS_Link| Structure link list to **KMIPCertificates** property.| No| Yes
|**KMIPServers**|RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS| Structure array points to one or more than one **RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS** structures for property **KMIPServers**.| No| No


## Links
    typedef struct _RedfishComputerSystem_V1_16_0_Links_CS {
        RedfishCS_Link Chassis;
        RedfishCS_int64 *Chassisodata_count;
        RedfishCS_Link ConsumingComputerSystems;
        RedfishCS_int64 *ConsumingComputerSystemsodata_count;
        Redfishodatav4_idRef_Array_CS *CooledBy;
        RedfishCS_int64 *CooledByodata_count;
        RedfishCS_Link Endpoints;
        RedfishCS_int64 *Endpointsodata_count;
        RedfishCS_Link ManagedBy;
        RedfishCS_int64 *ManagedByodata_count;
        RedfishResource_Oem_CS *Oem;
        Redfishodatav4_idRef_Array_CS *PoweredBy;
        RedfishCS_int64 *PoweredByodata_count;
        RedfishCS_Link ResourceBlocks;
        RedfishCS_int64 *ResourceBlocksodata_count;
        RedfishCS_Link SupplyingComputerSystems;
        RedfishCS_int64 *SupplyingComputerSystemsodata_count;
    } RedfishComputerSystem_V1_16_0_Links_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Chassis**|RedfishCS_Link| Structure link list to **Chassis** property.| No| Yes
|**Chassisodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Chassis@odata.count** property.| No| No
|**ConsumingComputerSystems**|RedfishCS_Link| Structure link list to **ConsumingComputerSystems** property.| No| Yes
|**ConsumingComputerSystemsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **ConsumingComputerSystems@odata.count** property.| No| No
|**CooledBy**|Redfishodatav4_idRef_Array_CS| Structure array points to one or more than one **Redfishodatav4_idRef_Array_CS** structures for property **CooledBy**.| No| Yes
|**CooledByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **CooledBy@odata.count** property.| No| No
|**Endpoints**|RedfishCS_Link| Structure link list to **Endpoints** property.| No| Yes
|**Endpointsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Endpoints@odata.count** property.| No| No
|**ManagedBy**|RedfishCS_Link| Structure link list to **ManagedBy** property.| No| Yes
|**ManagedByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **ManagedBy@odata.count** property.| No| No
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**PoweredBy**|Redfishodatav4_idRef_Array_CS| Structure array points to one or more than one **Redfishodatav4_idRef_Array_CS** structures for property **PoweredBy**.| No| Yes
|**PoweredByodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PoweredBy@odata.count** property.| No| No
|**ResourceBlocks**|RedfishCS_Link| Structure link list to **ResourceBlocks** property.| No| No
|**ResourceBlocksodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **ResourceBlocks@odata.count** property.| No| No
|**SupplyingComputerSystems**|RedfishCS_Link| Structure link list to **SupplyingComputerSystems** property.| No| Yes
|**SupplyingComputerSystemsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **SupplyingComputerSystems@odata.count** property.| No| No


## MemorySummary
    typedef struct _RedfishComputerSystem_V1_16_0_MemorySummary_CS {
        RedfishCS_char *MemoryMirroring;
        RedfishCS_Link Metrics;
        RedfishResource_Status_CS *Status;
        RedfishCS_int64 *TotalSystemMemoryGiB;
        RedfishCS_int64 *TotalSystemPersistentMemoryGiB;
    } RedfishComputerSystem_V1_16_0_MemorySummary_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**MemoryMirroring**|RedfishCS_char| String pointer to **MemoryMirroring** property.| No| Yes
|**Metrics**|RedfishCS_Link| Structure link list to **Metrics** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**TotalSystemMemoryGiB**|RedfishCS_int64| 64-bit long long interger pointer to **TotalSystemMemoryGiB** property.| No| Yes
|**TotalSystemPersistentMemoryGiB**|RedfishCS_int64| 64-bit long long interger pointer to **TotalSystemPersistentMemoryGiB** property.| No| Yes


## OemActions
    typedef struct _RedfishComputerSystem_V1_16_0_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishComputerSystem_V1_16_0_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## ProcessorSummary
    typedef struct _RedfishComputerSystem_V1_16_0_ProcessorSummary_CS {
        RedfishCS_int64 *CoreCount;
        RedfishCS_int64 *Count;
        RedfishCS_int64 *LogicalProcessorCount;
        RedfishCS_Link Metrics;
        RedfishCS_char *Model;
        RedfishResource_Status_CS *Status;
        RedfishCS_bool *ThreadingEnabled;
    } RedfishComputerSystem_V1_16_0_ProcessorSummary_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**CoreCount**|RedfishCS_int64| 64-bit long long interger pointer to **CoreCount** property.| No| Yes
|**Count**|RedfishCS_int64| 64-bit long long interger pointer to **Count** property.| No| Yes
|**LogicalProcessorCount**|RedfishCS_int64| 64-bit long long interger pointer to **LogicalProcessorCount** property.| No| Yes
|**Metrics**|RedfishCS_Link| Structure link list to **Metrics** property.| No| Yes
|**Model**|RedfishCS_char| String pointer to **Model** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**ThreadingEnabled**|RedfishCS_bool| Boolean pointer to **ThreadingEnabled** property.| No| No


## RemoveResourceBlock
    typedef struct _RedfishComputerSystem_V1_16_0_RemoveResourceBlock_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishComputerSystem_V1_16_0_RemoveResourceBlock_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## Reset
    typedef struct _RedfishComputerSystem_V1_16_0_Reset_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishComputerSystem_V1_16_0_Reset_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## SerialConsoleProtocol
    typedef struct _RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS {
        RedfishCS_char *ConsoleEntryCommand;
        RedfishCS_char *HotKeySequenceDisplay;
        RedfishCS_int64 *Port;
        RedfishCS_bool *ServiceEnabled;
        RedfishCS_bool *SharedWithManagerCLI;
    } RedfishComputerSystem_V1_16_0_SerialConsoleProtocol_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ConsoleEntryCommand**|RedfishCS_char| String pointer to **ConsoleEntryCommand** property.| No| Yes
|**HotKeySequenceDisplay**|RedfishCS_char| String pointer to **HotKeySequenceDisplay** property.| No| Yes
|**Port**|RedfishCS_int64| 64-bit long long interger pointer to **Port** property.| No| No
|**ServiceEnabled**|RedfishCS_bool| Boolean pointer to **ServiceEnabled** property.| No| No
|**SharedWithManagerCLI**|RedfishCS_bool| Boolean pointer to **SharedWithManagerCLI** property.| No| Yes


## SetDefaultBootOrder
    typedef struct _RedfishComputerSystem_V1_16_0_SetDefaultBootOrder_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishComputerSystem_V1_16_0_SetDefaultBootOrder_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## TrustedModules
    typedef struct _RedfishComputerSystem_V1_16_0_TrustedModules_CS {
        RedfishCS_char *FirmwareVersion;
        RedfishCS_char *FirmwareVersion2;
        RedfishCS_char *InterfaceType;
        RedfishCS_char *InterfaceTypeSelection;
        RedfishResource_Oem_CS *Oem;
        RedfishResource_Status_CS *Status;
    } RedfishComputerSystem_V1_16_0_TrustedModules_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**FirmwareVersion**|RedfishCS_char| String pointer to **FirmwareVersion** property.| No| Yes
|**FirmwareVersion2**|RedfishCS_char| String pointer to **FirmwareVersion2** property.| No| Yes
|**InterfaceType**|RedfishCS_char| String pointer to **InterfaceType** property.| No| Yes
|**InterfaceTypeSelection**|RedfishCS_char| String pointer to **InterfaceTypeSelection** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No


## VirtualMediaConfig
    typedef struct _RedfishComputerSystem_V1_16_0_VirtualMediaConfig_CS {
        RedfishCS_int64 *Port;
        RedfishCS_bool *ServiceEnabled;
    } RedfishComputerSystem_V1_16_0_VirtualMediaConfig_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Port**|RedfishCS_int64| 64-bit long long interger pointer to **Port** property.| No| No
|**ServiceEnabled**|RedfishCS_bool| Boolean pointer to **ServiceEnabled** property.| No| No


## WatchdogTimer
    typedef struct _RedfishComputerSystem_V1_16_0_WatchdogTimer_CS {
        RedfishCS_bool *FunctionEnabled;
        RedfishResource_Oem_CS *Oem;
        RedfishResource_Status_CS *Status;
        RedfishCS_char *TimeoutAction;
        RedfishCS_char *WarningAction;
    } RedfishComputerSystem_V1_16_0_WatchdogTimer_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**FunctionEnabled**|RedfishCS_bool| Boolean pointer to **FunctionEnabled** property.| No| No
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**TimeoutAction**|RedfishCS_char| String pointer to **TimeoutAction** property.| No| No
|**WarningAction**|RedfishCS_char| String pointer to **WarningAction** property.| No| No


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
    typedef struct _Redfishodatav4_idRef_CS {
        RedfishCS_char *odata_id;
    } Redfishodatav4_idRef_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No


## RedfishCS_char_Array
    typedef struct _RedfishCS_char_Array  {
        RedfishCS_Link *Next;
        RedfishCS_char *ArrayValue;
    } RedfishCS_char_Array;



## RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS
    typedef struct _RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS  {
        RedfishCS_Link *Next;
        RedfishComputerSystem_V1_16_0_KMIPServer_CS *ArrayValue;
    } RedfishComputerSystem_V1_16_0_KMIPServer_Array_CS;



## Redfishodatav4_idRef_Array_CS
    typedef struct _Redfishodatav4_idRef_Array_CS  {
        RedfishCS_Link *Next;
        Redfishodatav4_idRef_CS *ArrayValue;
    } Redfishodatav4_idRef_Array_CS;



## RedfishResource_Condition_Array_CS
    typedef struct _RedfishResource_Condition_Array_CS  {
        RedfishCS_Link *Next;
        RedfishResource_Condition_CS *ArrayValue;
    } RedfishResource_Condition_Array_CS;



## ComputerSystem
    typedef struct _RedfishComputerSystem_V1_16_0_ComputerSystem_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishComputerSystem_V1_16_0_Actions_CS *Actions;
        RedfishCS_char *AssetTag;
        RedfishCS_Link Bios;
        RedfishCS_char *BiosVersion;
        RedfishComputerSystem_V1_16_0_Boot_CS *Boot;
        RedfishComputerSystem_V1_16_0_BootProgress_CS *BootProgress;
        RedfishCS_Link Certificates;
        RedfishCS_char *Description;
        RedfishCS_Link EthernetInterfaces;
        RedfishCS_Link FabricAdapters;
        RedfishComputerSystem_V1_16_0_HostGraphicalConsole_CS *GraphicalConsole;
        RedfishCS_Link GraphicsControllers;
        RedfishCS_char *HostName;
        RedfishComputerSystem_V1_16_0_WatchdogTimer_CS *HostWatchdogTimer;
        RedfishComputerSystem_V1_16_0_HostedServices_CS *HostedServices;
        RedfishCS_char *HostingRoles;
        RedfishCS_char *Id;
        RedfishComputerSystem_V1_16_0_IdlePowerSaver_CS *IdlePowerSaver;
        RedfishCS_char *IndicatorLED;
        RedfishComputerSystem_V1_16_0_KeyManagement_CS *KeyManagement;
        RedfishCS_char *LastResetTime;
        RedfishComputerSystem_V1_16_0_Links_CS *Links;
        RedfishCS_bool *LocationIndicatorActive;
        RedfishCS_Link LogServices;
        RedfishCS_char *Manufacturer;
        RedfishCS_Link Measurements;
        RedfishCS_Link Memory;
        RedfishCS_Link MemoryDomains;
        RedfishComputerSystem_V1_16_0_MemorySummary_CS *MemorySummary;
        RedfishCS_char *Model;
        RedfishCS_char *Name;
        RedfishCS_Link NetworkInterfaces;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_Link PCIeDevices;
        RedfishCS_int64 *PCIeDevicesodata_count;
        RedfishCS_Link PCIeFunctions;
        RedfishCS_int64 *PCIeFunctionsodata_count;
        RedfishCS_char *PartNumber;
        RedfishCS_int64 *PowerCycleDelaySeconds;
        RedfishCS_char *PowerMode;
        RedfishCS_int64 *PowerOffDelaySeconds;
        RedfishCS_int64 *PowerOnDelaySeconds;
        RedfishCS_char *PowerRestorePolicy;
        RedfishCS_char *PowerState;
        RedfishComputerSystem_V1_16_0_ProcessorSummary_CS *ProcessorSummary;
        RedfishCS_Link Processors;
        RedfishCS_Link Redundancy;
        RedfishCS_int64 *Redundancyodata_count;
        RedfishCS_char *SKU;
        RedfishCS_Link SecureBoot;
        RedfishComputerSystem_V1_16_0_HostSerialConsole_CS *SerialConsole;
        RedfishCS_char *SerialNumber;
        RedfishCS_Link SimpleStorage;
        RedfishResource_Status_CS *Status;
        RedfishCS_Link Storage;
        RedfishCS_char *SubModel;
        RedfishCS_char *SystemType;
        RedfishComputerSystem_V1_16_0_TrustedModules_CS *TrustedModules;
        RedfishCS_Link USBControllers;
        RedfishCS_char *UUID;
        RedfishCS_Link VirtualMedia;
        RedfishComputerSystem_V1_16_0_VirtualMediaConfig_CS *VirtualMediaConfig;
    } RedfishComputerSystem_V1_16_0_ComputerSystem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishComputerSystem_V1_16_0_Actions_CS| Structure points to **Actions** property.| No| No
|**AssetTag**|RedfishCS_char| String pointer to **AssetTag** property.| No| No
|**Bios**|RedfishCS_Link| Structure link list to **Bios** property.| No| Yes
|**BiosVersion**|RedfishCS_char| String pointer to **BiosVersion** property.| No| Yes
|**Boot**|RedfishComputerSystem_V1_16_0_Boot_CS| Structure points to **Boot** property.| No| No
|**BootProgress**|RedfishComputerSystem_V1_16_0_BootProgress_CS| Structure points to **BootProgress** property.| No| No
|**Certificates**|RedfishCS_Link| Structure link list to **Certificates** property.| No| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**EthernetInterfaces**|RedfishCS_Link| Structure link list to **EthernetInterfaces** property.| No| Yes
|**FabricAdapters**|RedfishCS_Link| Structure link list to **FabricAdapters** property.| No| Yes
|**GraphicalConsole**|RedfishComputerSystem_V1_16_0_HostGraphicalConsole_CS| Structure points to **GraphicalConsole** property.| No| No
|**GraphicsControllers**|RedfishCS_Link| Structure link list to **GraphicsControllers** property.| No| Yes
|**HostName**|RedfishCS_char| String pointer to **HostName** property.| No| No
|**HostWatchdogTimer**|RedfishComputerSystem_V1_16_0_WatchdogTimer_CS| Structure points to **HostWatchdogTimer** property.| No| No
|**HostedServices**|RedfishComputerSystem_V1_16_0_HostedServices_CS| Structure points to **HostedServices** property.| No| No
|**HostingRoles**|RedfishCS_char| String pointer to **HostingRoles** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**IdlePowerSaver**|RedfishComputerSystem_V1_16_0_IdlePowerSaver_CS| Structure points to **IdlePowerSaver** property.| No| No
|**IndicatorLED**|RedfishCS_char| String pointer to **IndicatorLED** property.| No| No
|**KeyManagement**|RedfishComputerSystem_V1_16_0_KeyManagement_CS| Structure points to **KeyManagement** property.| No| No
|**LastResetTime**|RedfishCS_char| String pointer to **LastResetTime** property.| No| Yes
|**Links**|RedfishComputerSystem_V1_16_0_Links_CS| Structure points to **Links** property.| No| No
|**LocationIndicatorActive**|RedfishCS_bool| Boolean pointer to **LocationIndicatorActive** property.| No| No
|**LogServices**|RedfishCS_Link| Structure link list to **LogServices** property.| No| Yes
|**Manufacturer**|RedfishCS_char| String pointer to **Manufacturer** property.| No| Yes
|**Measurements**|RedfishCS_Link| Structure link list to **Measurements** property.| No| No
|**Memory**|RedfishCS_Link| Structure link list to **Memory** property.| No| Yes
|**MemoryDomains**|RedfishCS_Link| Structure link list to **MemoryDomains** property.| No| Yes
|**MemorySummary**|RedfishComputerSystem_V1_16_0_MemorySummary_CS| Structure points to **MemorySummary** property.| No| No
|**Model**|RedfishCS_char| String pointer to **Model** property.| No| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**NetworkInterfaces**|RedfishCS_Link| Structure link list to **NetworkInterfaces** property.| No| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**PCIeDevices**|RedfishCS_Link| Structure link list to **PCIeDevices** property.| No| Yes
|**PCIeDevicesodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PCIeDevices@odata.count** property.| No| No
|**PCIeFunctions**|RedfishCS_Link| Structure link list to **PCIeFunctions** property.| No| Yes
|**PCIeFunctionsodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **PCIeFunctions@odata.count** property.| No| No
|**PartNumber**|RedfishCS_char| String pointer to **PartNumber** property.| No| Yes
|**PowerCycleDelaySeconds**|RedfishCS_int64| 64-bit long long interger pointer to **PowerCycleDelaySeconds** property.| No| No
|**PowerMode**|RedfishCS_char| String pointer to **PowerMode** property.| No| No
|**PowerOffDelaySeconds**|RedfishCS_int64| 64-bit long long interger pointer to **PowerOffDelaySeconds** property.| No| No
|**PowerOnDelaySeconds**|RedfishCS_int64| 64-bit long long interger pointer to **PowerOnDelaySeconds** property.| No| No
|**PowerRestorePolicy**|RedfishCS_char| String pointer to **PowerRestorePolicy** property.| No| No
|**PowerState**|RedfishCS_char| String pointer to **PowerState** property.| No| Yes
|**ProcessorSummary**|RedfishComputerSystem_V1_16_0_ProcessorSummary_CS| Structure points to **ProcessorSummary** property.| No| No
|**Processors**|RedfishCS_Link| Structure link list to **Processors** property.| No| Yes
|**Redundancy**|RedfishCS_Link| Structure link list to **Redundancy** property.| No| Yes
|**Redundancyodata_count**|RedfishCS_int64| 64-bit long long interger pointer to **Redundancy@odata.count** property.| No| No
|**SKU**|RedfishCS_char| String pointer to **SKU** property.| No| Yes
|**SecureBoot**|RedfishCS_Link| Structure link list to **SecureBoot** property.| No| Yes
|**SerialConsole**|RedfishComputerSystem_V1_16_0_HostSerialConsole_CS| Structure points to **SerialConsole** property.| No| No
|**SerialNumber**|RedfishCS_char| String pointer to **SerialNumber** property.| No| Yes
|**SimpleStorage**|RedfishCS_Link| Structure link list to **SimpleStorage** property.| No| Yes
|**Status**|RedfishResource_Status_CS| Structure points to **Status** property.| No| No
|**Storage**|RedfishCS_Link| Structure link list to **Storage** property.| No| Yes
|**SubModel**|RedfishCS_char| String pointer to **SubModel** property.| No| Yes
|**SystemType**|RedfishCS_char| String pointer to **SystemType** property.| No| Yes
|**TrustedModules**|RedfishComputerSystem_V1_16_0_TrustedModules_CS| Structure points to **TrustedModules** property.| No| No
|**USBControllers**|RedfishCS_Link| Structure link list to **USBControllers** property.| No| Yes
|**UUID**|RedfishCS_char| String pointer to **UUID** property.| No| Yes
|**VirtualMedia**|RedfishCS_Link| Structure link list to **VirtualMedia** property.| No| Yes
|**VirtualMediaConfig**|RedfishComputerSystem_V1_16_0_VirtualMediaConfig_CS| Structure points to **VirtualMediaConfig** property.| No| No
## Redfish ComputerSystem V1_16_0 to C Structure Function
    RedfishCS_status
    Json_ComputerSystem_V1_16_0_To_CS (RedfishCS_char *JsonRawText, RedfishComputerSystem_V1_16_0_ComputerSystem_CS **ReturnedCS);

## C Structure to Redfish ComputerSystem V1_16_0 JSON Function
    RedfishCS_status
    CS_To_ComputerSystem_V1_16_0_JSON (RedfishComputerSystem_V1_16_0_ComputerSystem_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish ComputerSystem V1_16_0 C Structure Function
    RedfishCS_status
    DestroyComputerSystem_V1_16_0_CS (RedfishComputerSystem_V1_16_0_ComputerSystem_CS *CSPtr);

