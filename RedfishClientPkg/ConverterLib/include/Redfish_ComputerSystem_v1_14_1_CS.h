//
// Auto-generated file by Redfish Schema C Structure Generator.
// https://github.com/DMTF/Redfish-Schema-C-Struct-Generator.
//
//  (C) Copyright 2019-2021 Hewlett Packard Enterprise Development LP<BR>
//
// Copyright Notice:
// Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
// License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
//

#ifndef RedfishComputersystem_V1_14_1_CSTRUCT_H_
#define RedfishComputersystem_V1_14_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS Redfishodatav4_idRef_Array_CS;
#endif
typedef struct _RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS;
#ifndef RedfishResource_Condition_Array_CS_
typedef struct _RedfishResource_Condition_Array_CS RedfishResource_Condition_Array_CS;
#endif
typedef struct _RedfishComputerSystem_V1_14_1_Actions_CS RedfishComputerSystem_V1_14_1_Actions_CS;
typedef struct _RedfishComputerSystem_V1_14_1_AddResourceBlock_CS RedfishComputerSystem_V1_14_1_AddResourceBlock_CS;
typedef struct _RedfishComputerSystem_V1_14_1_Boot_CS RedfishComputerSystem_V1_14_1_Boot_CS;
typedef struct _RedfishComputerSystem_V1_14_1_BootProgress_CS RedfishComputerSystem_V1_14_1_BootProgress_CS;
typedef struct _RedfishComputerSystem_V1_14_1_ComputerSystem_CS RedfishComputerSystem_V1_14_1_ComputerSystem_CS;
typedef struct _RedfishComputerSystem_V1_14_1_HostGraphicalConsole_CS RedfishComputerSystem_V1_14_1_HostGraphicalConsole_CS;
typedef struct _RedfishComputerSystem_V1_14_1_HostSerialConsole_CS RedfishComputerSystem_V1_14_1_HostSerialConsole_CS;
typedef struct _RedfishComputerSystem_V1_14_1_HostedServices_CS RedfishComputerSystem_V1_14_1_HostedServices_CS;
typedef struct _RedfishComputerSystem_V1_14_1_Links_CS RedfishComputerSystem_V1_14_1_Links_CS;
typedef struct _RedfishComputerSystem_V1_14_1_MemorySummary_CS RedfishComputerSystem_V1_14_1_MemorySummary_CS;
typedef struct _RedfishComputerSystem_V1_14_1_OemActions_CS RedfishComputerSystem_V1_14_1_OemActions_CS;
typedef struct _RedfishComputerSystem_V1_14_1_ProcessorSummary_CS RedfishComputerSystem_V1_14_1_ProcessorSummary_CS;
typedef struct _RedfishComputerSystem_V1_14_1_RemoveResourceBlock_CS RedfishComputerSystem_V1_14_1_RemoveResourceBlock_CS;
typedef struct _RedfishComputerSystem_V1_14_1_Reset_CS RedfishComputerSystem_V1_14_1_Reset_CS;
typedef struct _RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS;
typedef struct _RedfishComputerSystem_V1_14_1_SetDefaultBootOrder_CS RedfishComputerSystem_V1_14_1_SetDefaultBootOrder_CS;
typedef struct _RedfishComputerSystem_V1_14_1_TrustedModules_CS RedfishComputerSystem_V1_14_1_TrustedModules_CS;
typedef struct _RedfishComputerSystem_V1_14_1_VirtualMediaConfig_CS RedfishComputerSystem_V1_14_1_VirtualMediaConfig_CS;
typedef struct _RedfishComputerSystem_V1_14_1_WatchdogTimer_CS RedfishComputerSystem_V1_14_1_WatchdogTimer_CS;
#ifndef RedfishResource_Condition_CS_
typedef struct _RedfishResource_Condition_CS RedfishResource_Condition_CS;
#endif

#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

#ifndef RedfishResource_Status_CS_
typedef struct _RedfishResource_Status_CS RedfishResource_Status_CS;
#endif

#ifndef Redfishodatav4_idRef_CS_
typedef struct _Redfishodatav4_idRef_CS Redfishodatav4_idRef_CS;
#endif

//
// The available actions for this resource.
//
typedef struct _RedfishComputerSystem_V1_14_1_Actions_CS {
    RedfishComputerSystem_V1_14_1_AddResourceBlock_CS       *ComputerSystem_AddResourceBlock;  
    RedfishComputerSystem_V1_14_1_RemoveResourceBlock_CS    *ComputerSystem_RemoveResourceBlock;
    RedfishComputerSystem_V1_14_1_Reset_CS                  *ComputerSystem_Reset;             
    RedfishComputerSystem_V1_14_1_SetDefaultBootOrder_CS    *ComputerSystem_SetDefaultBootOrder;
    RedfishComputerSystem_V1_14_1_OemActions_CS             *Oem;                                  // The available OEM-specific
                                                                                                   // actions for this resource.
} RedfishComputerSystem_V1_14_1_Actions_CS;

//
// This action adds a resource block to a system.
//
typedef struct _RedfishComputerSystem_V1_14_1_AddResourceBlock_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishComputerSystem_V1_14_1_AddResourceBlock_CS;

//
// The boot information for this resource.
//
typedef struct _RedfishComputerSystem_V1_14_1_Boot_CS {
    RedfishCS_char_Array    *AliasBootOrder;                    // Ordered array of boot source
                                                                // aliases representing the
                                                                // persistent boot order
                                                                // associated with this computer
                                                                // system.
    RedfishCS_int64         *AutomaticRetryAttempts;            // The number of attempts the
                                                                // system will automatically
                                                                // retry booting.
    RedfishCS_char          *AutomaticRetryConfig;              // The configuration of how the
                                                                // system retries booting
                                                                // automatically.
    RedfishCS_char          *BootNext;                          // The BootOptionReference of the
                                                                // Boot Option to perform a one-
                                                                // time boot from when
                                                                // BootSourceOverrideTarget is
                                                                // `UefiBootNext`.
    RedfishCS_Link          BootOptions;                        // The link to the collection of
                                                                // the UEFI boot options
                                                                // associated with this computer
                                                                // system.
    RedfishCS_char_Array    *BootOrder;                         // An array of
                                                                // BootOptionReference strings
                                                                // that represent the persistent
                                                                // boot order for with this
                                                                // computer system.
    RedfishCS_char          *BootOrderPropertySelection;        // The name of the boot order
                                                                // property that the system uses
                                                                // for the persistent boot order.
    RedfishCS_char          *BootSourceOverrideEnabled;         // The state of the boot source
                                                                // override feature.
    RedfishCS_char          *BootSourceOverrideMode;            // The BIOS boot mode to use when
                                                                // the system boots from the
                                                                // BootSourceOverrideTarget boot
                                                                // source.
    RedfishCS_char          *BootSourceOverrideTarget;          // The current boot source to use
                                                                // at the next boot instead of
                                                                // the normal boot device, if
                                                                // BootSourceOverrideEnabled is
                                                                // `true`.
    RedfishCS_Link          Certificates;                       // The link to a collection of
                                                                // certificates used for booting
                                                                // through HTTPS by this computer
                                                                // system.
    RedfishCS_char          *HttpBootUri;                       // The URI to boot from when
                                                                // BootSourceOverrideTarget is
                                                                // set to `UefiHttp`.
    RedfishCS_int64         *RemainingAutomaticRetryAttempts;    // The number of remaining
                                                                // automatic retry boots.
    RedfishCS_char          *TrustedModuleRequiredToBoot;       // The Trusted Module boot
                                                                // requirement.
    RedfishCS_char          *UefiTargetBootSourceOverride;      // The UEFI device path of the
                                                                // device from which to boot when
                                                                // BootSourceOverrideTarget is
                                                                // `UefiTarget`.
} RedfishComputerSystem_V1_14_1_Boot_CS;

//
// This object describes the last boot progress state.
//
typedef struct _RedfishComputerSystem_V1_14_1_BootProgress_CS {
    RedfishCS_char            *LastState;       // The last boot progress state.
    RedfishCS_char            *LastStateTime;    // The date and time when the
                                                // last boot state was updated.
    RedfishResource_Oem_CS    *Oem;             // The OEM extension property.
    RedfishCS_char            *OemLastState;    // The OEM-specific last state,
                                                // if the LastState type is
                                                // `OEM`.
} RedfishComputerSystem_V1_14_1_BootProgress_CS;

//
// The information about a graphical console service for this system.
//
typedef struct _RedfishComputerSystem_V1_14_1_HostGraphicalConsole_CS {
    RedfishCS_char_Array    *ConnectTypesSupported;    // This property enumerates the
                                                      // graphical console connection
                                                      // types that the implementation
                                                      // allows.
    RedfishCS_int64         *MaxConcurrentSessions;    // The maximum number of service
                                                      // sessions, regardless of
                                                      // protocol, that this system can
                                                      // support.
    RedfishCS_int64         *Port;                    // The protocol port.
    RedfishCS_bool          *ServiceEnabled;          // An indication of whether the
                                                      // service is enabled for this
                                                      // system.
} RedfishComputerSystem_V1_14_1_HostGraphicalConsole_CS;

//
// The information about the serial console services that this system provides.
//
typedef struct _RedfishComputerSystem_V1_14_1_HostSerialConsole_CS {
    RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS    *IPMI;                    // The connection details for an
                                                                                        // IPMI Serial-over-LAN service.
    RedfishCS_int64                                           *MaxConcurrentSessions;    // The maximum number of service
                                                                                        // sessions, regardless of
                                                                                        // protocol, that this system can
                                                                                        // support.
    RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS    *SSH;                     // The connection details for an
                                                                                        // SSH serial console service.
    RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS    *Telnet;                  // The connection details for a
                                                                                        // Telnet serial console service.
} RedfishComputerSystem_V1_14_1_HostSerialConsole_CS;

//
// The services that might be running or installed on the system.
//
typedef struct _RedfishComputerSystem_V1_14_1_HostedServices_CS {
    RedfishResource_Oem_CS    *Oem;              // The OEM extension property.
    RedfishCS_Link            StorageServices;    // The link to a collection of
                                                 // storage services that this
                                                 // computer system supports.
} RedfishComputerSystem_V1_14_1_HostedServices_CS;

//
// The links to other resources that are related to this resource.
//
typedef struct _RedfishComputerSystem_V1_14_1_Links_CS {
    RedfishCS_Link                   Chassis;                                // An array of links to the
                                                                             // chassis that contains this
                                                                             // system.
    RedfishCS_int64                  *Chassisodata_count;                
    RedfishCS_Link                   ConsumingComputerSystems;               // An array of links to
                                                                             // ComputerSystems that are
                                                                             // realized, in whole or in part,
                                                                             // from this ComputerSystem.
    RedfishCS_int64                  *ConsumingComputerSystemsodata_count;
    Redfishodatav4_idRef_Array_CS    *CooledBy;                              // An array of links to resources
                                                                             // or objects that that cool this
                                                                             // computer system.  Normally,
                                                                             // the link is for either a
                                                                             // chassis or a specific set of
                                                                             // fans.
    RedfishCS_int64                  *CooledByodata_count;               
    RedfishCS_Link                   Endpoints;                              // An array of links to the
                                                                             // endpoints that connect to this
                                                                             // system.
    RedfishCS_int64                  *Endpointsodata_count;              
    RedfishCS_Link                   ManagedBy;                              // An array of links to the
                                                                             // managers responsible for this
                                                                             // system.
    RedfishCS_int64                  *ManagedByodata_count;              
    RedfishResource_Oem_CS           *Oem;                                   // The OEM extension property.
    Redfishodatav4_idRef_Array_CS    *PoweredBy;                             // An array of links to resources
                                                                             // or objects that power this
                                                                             // computer system.  Normally,
                                                                             // the link is for either a
                                                                             // chassis or a specific set of
                                                                             // power supplies.
    RedfishCS_int64                  *PoweredByodata_count;              
    RedfishCS_Link                   ResourceBlocks;                         // An array of links to the
                                                                             // resource blocks that are used
                                                                             // in this computer system.
    RedfishCS_int64                  *ResourceBlocksodata_count;         
    RedfishCS_Link                   SupplyingComputerSystems;               // An array of links to
                                                                             // ComputerSystems that
                                                                             // contribute, in whole or in
                                                                             // part, to the implementation of
                                                                             // this ComputerSystem.
    RedfishCS_int64                  *SupplyingComputerSystemsodata_count;
} RedfishComputerSystem_V1_14_1_Links_CS;

//
// The memory of the system in general detail.
//
typedef struct _RedfishComputerSystem_V1_14_1_MemorySummary_CS {
    RedfishCS_char               *MemoryMirroring;                  // The ability and type of memory
                                                                    // mirroring that this computer
                                                                    // system supports.
    RedfishCS_Link               Metrics;                           // The link to the metrics
                                                                    // associated with all memory in
                                                                    // this system.
    RedfishResource_Status_CS    *Status;                           // The status and health of the
                                                                    // resource and its subordinate
                                                                    // or dependent resources.
    RedfishCS_int64              *TotalSystemMemoryGiB;             // The total configured operating
                                                                    // system-accessible memory
                                                                    // (RAM), measured in GiB.
    RedfishCS_int64              *TotalSystemPersistentMemoryGiB;    // The total configured, system-
                                                                    // accessible persistent memory,
                                                                    // measured in GiB.
} RedfishComputerSystem_V1_14_1_MemorySummary_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishComputerSystem_V1_14_1_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishComputerSystem_V1_14_1_OemActions_CS;

//
// The central processors of the system in general detail.
//
typedef struct _RedfishComputerSystem_V1_14_1_ProcessorSummary_CS {
    RedfishCS_int64              *CoreCount;               // The number of processor cores
                                                           // in the system.
    RedfishCS_int64              *Count;                   // The number of physical
                                                           // processors in the system.
    RedfishCS_int64              *LogicalProcessorCount;    // The number of logical
                                                           // processors in the system.
    RedfishCS_Link               Metrics;                  // The link to the metrics
                                                           // associated with all processors
                                                           // in this system.
    RedfishCS_char               *Model;                   // The processor model for the
                                                           // primary or majority of
                                                           // processors in this system.
    RedfishResource_Status_CS    *Status;                  // The status and health of the
                                                           // resource and its subordinate
                                                           // or dependent resources.
} RedfishComputerSystem_V1_14_1_ProcessorSummary_CS;

//
// This action removes a resource block from a system.
//
typedef struct _RedfishComputerSystem_V1_14_1_RemoveResourceBlock_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishComputerSystem_V1_14_1_RemoveResourceBlock_CS;

//
// This action resets the system.
//
typedef struct _RedfishComputerSystem_V1_14_1_Reset_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishComputerSystem_V1_14_1_Reset_CS;

//
// The information about a serial console service that this system provides.
//
typedef struct _RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS {
    RedfishCS_char     *ConsoleEntryCommand;     // The command string passed to
                                                 // the service to select or enter
                                                 // the system's serial console.
    RedfishCS_char     *HotKeySequenceDisplay;    // The hotkey sequence available
                                                 // for the user to exit the
                                                 // serial console session.
    RedfishCS_int64    *Port;                    // The protocol port.
    RedfishCS_bool     *ServiceEnabled;          // An indication of whether the
                                                 // service is enabled for this
                                                 // system.
    RedfishCS_bool     *SharedWithManagerCLI;    // Indicates whether the serial
                                                 // console service is shared with
                                                 // access to the manager's
                                                 // command-line interface (CLI).
} RedfishComputerSystem_V1_14_1_SerialConsoleProtocol_CS;

//
// This action sets the BootOrder to the default settings.
//
typedef struct _RedfishComputerSystem_V1_14_1_SetDefaultBootOrder_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishComputerSystem_V1_14_1_SetDefaultBootOrder_CS;

//
// The Trusted Module installed in the system.
//
typedef struct _RedfishComputerSystem_V1_14_1_TrustedModules_CS {
    RedfishCS_char               *FirmwareVersion;          // The firmware version of this
                                                            // Trusted Module.
    RedfishCS_char               *FirmwareVersion2;         // The second firmware version of
                                                            // this Trusted Module, if
                                                            // applicable.
    RedfishCS_char               *InterfaceType;            // The interface type of the
                                                            // Trusted Module.
    RedfishCS_char               *InterfaceTypeSelection;    // The interface type selection
                                                            // supported by this Trusted
                                                            // Module.
    RedfishResource_Oem_CS       *Oem;                      // The OEM extension property.
    RedfishResource_Status_CS    *Status;                   // The status and health of the
                                                            // resource and its subordinate
                                                            // or dependent resources.
} RedfishComputerSystem_V1_14_1_TrustedModules_CS;

//
// The information about virtual media service for this system.
//
typedef struct _RedfishComputerSystem_V1_14_1_VirtualMediaConfig_CS {
    RedfishCS_int64    *Port;             // The protocol port.
    RedfishCS_bool     *ServiceEnabled;    // An indication of whether the
                                          // service is enabled for this
                                          // system.
} RedfishComputerSystem_V1_14_1_VirtualMediaConfig_CS;

//
// This type describes the host watchdog timer functionality for this system.
//
typedef struct _RedfishComputerSystem_V1_14_1_WatchdogTimer_CS {
    RedfishCS_bool               *FunctionEnabled;    // An indication of whether a
                                                     // user has enabled the host
                                                     // watchdog timer functionality.
                                                     // This property indicates only
                                                     // that a user has enabled the
                                                     // timer.  To activate the timer,
                                                     // installation of additional
                                                     // host-based software is
                                                     // necessary; an update to this
                                                     // property does not initiate the
                                                     // timer.
    RedfishResource_Oem_CS       *Oem;               // The OEM extension property.
    RedfishResource_Status_CS    *Status;            // The status and health of the
                                                     // resource and its subordinate
                                                     // or dependent resources.
    RedfishCS_char               *TimeoutAction;     // The action to perform when the
                                                     // watchdog timer reaches its
                                                     // timeout value.
    RedfishCS_char               *WarningAction;     // The action to perform when the
                                                     // watchdog timer is close to
                                                     // reaching its timeout value.
                                                     // This action typically occurs
                                                     // from three to ten seconds
                                                     // before to the timeout value,
                                                     // but the exact timing is
                                                     // dependent on the
                                                     // implementation.
} RedfishComputerSystem_V1_14_1_WatchdogTimer_CS;

//
// A condition that requires attention.
//
#ifndef RedfishResource_Condition_CS_
#define RedfishResource_Condition_CS_
typedef struct _RedfishResource_Condition_CS {
    RedfishCS_Link             LogEntry;             // The link to the log entry
                                                     // created for this condition.
    RedfishCS_char             *Message;             // The human-readable message for
                                                     // this condition.
    RedfishCS_char_Array       *MessageArgs;         // An array of message arguments
                                                     // that are substituted for the
                                                     // arguments in the message when
                                                     // looked up in the message
                                                     // registry.
    RedfishCS_char             *MessageId;           // The identifier for the
                                                     // message.
    Redfishodatav4_idRef_CS    *OriginOfCondition;    // A link to the resource or
                                                     // object that originated the
                                                     // condition.
    RedfishCS_char             *Resolution;          // Suggestions on how to resolve
                                                     // the condition.
    RedfishCS_char             *Severity;            // The severity of the condition.
    RedfishCS_char             *Timestamp;           // The time the condition
                                                     // occurred.
} RedfishResource_Condition_CS;
#endif

//
// The OEM extension.
//
#ifndef RedfishResource_Oem_CS_
#define RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS {
    RedfishCS_Link    Prop;
} RedfishResource_Oem_CS;
#endif

//
// The status and health of a resource and its children.
//
#ifndef RedfishResource_Status_CS_
#define RedfishResource_Status_CS_
typedef struct _RedfishResource_Status_CS {
    RedfishResource_Condition_Array_CS    *Conditions;     // Conditions in this resource
                                                           // that require attention.
    RedfishCS_char                        *Health;         // The health state of this
                                                           // resource in the absence of its
                                                           // dependent resources.
    RedfishCS_char                        *HealthRollup;    // The overall health state from
                                                           // the view of this resource.
    RedfishResource_Oem_CS                *Oem;            // The OEM extension property.
    RedfishCS_char                        *State;          // The known state of the
                                                           // resource, such as, enabled.
} RedfishResource_Status_CS;
#endif

//
// A reference to a resource.
//
#ifndef Redfishodatav4_idRef_CS_
#define Redfishodatav4_idRef_CS_
typedef struct _Redfishodatav4_idRef_CS {
    RedfishCS_char    *odata_id;
} Redfishodatav4_idRef_CS;
#endif

//
// The ComputerSystem schema represents a computer or system instance and the
// software-visible resources, or items within the data plane, such as memory,
// CPU, and other devices that it can access.  Details of those resources or
// subsystems are also linked through this resource.
//
typedef struct _RedfishComputerSystem_V1_14_1_ComputerSystem_CS {
    RedfishCS_Header                                         Header;
    RedfishCS_char                                           *odata_context;          
    RedfishCS_char                                           *odata_etag;             
    RedfishCS_char                                           *odata_id;               
    RedfishCS_char                                           *odata_type;             
    RedfishComputerSystem_V1_14_1_Actions_CS                 *Actions;                    // The available actions for this
                                                                                          // resource.
    RedfishCS_char                                           *AssetTag;                   // The user-definable tag that
                                                                                          // can track this computer system
                                                                                          // for inventory or other client
                                                                                          // purposes.
    RedfishCS_Link                                           Bios;                        // The link to the BIOS settings
                                                                                          // associated with this system.
    RedfishCS_char                                           *BiosVersion;                // The version of the system BIOS
                                                                                          // or primary system firmware.
    RedfishComputerSystem_V1_14_1_Boot_CS                    *Boot;                       // The boot settings for this
                                                                                          // system.
    RedfishComputerSystem_V1_14_1_BootProgress_CS            *BootProgress;               // This object describes the last
                                                                                          // boot progress state.
    RedfishCS_Link                                           Certificates;                // The link to a collection of
                                                                                          // certificates for device
                                                                                          // identity and attestation.
    RedfishCS_char                                           *Description;            
    RedfishCS_Link                                           EthernetInterfaces;          // The link to the collection of
                                                                                          // Ethernet interfaces associated
                                                                                          // with this system.
    RedfishCS_Link                                           FabricAdapters;              // The link to the collection of
                                                                                          // fabric adapters associated
                                                                                          // with this system.
    RedfishComputerSystem_V1_14_1_HostGraphicalConsole_CS    *GraphicalConsole;           // The information about the
                                                                                          // graphical console (KVM-IP)
                                                                                          // service of this system.
    RedfishCS_char                                           *HostName;                   // The DNS host name, without any
                                                                                          // domain information.
    RedfishComputerSystem_V1_14_1_WatchdogTimer_CS           *HostWatchdogTimer;          // The host watchdog timer
                                                                                          // functionality for this system.
    RedfishComputerSystem_V1_14_1_HostedServices_CS          *HostedServices;             // The services that this
                                                                                          // computer system supports.
    RedfishCS_char_Array                                     *HostingRoles;               // The hosting roles that this
                                                                                          // computer system supports.
    RedfishCS_char                                           *Id;                     
    RedfishCS_char                                           *IndicatorLED;               // The state of the indicator
                                                                                          // LED, which identifies the
                                                                                          // system.
    RedfishCS_char                                           *LastResetTime;              // The date and time when the
                                                                                          // system was last reset or
                                                                                          // rebooted.
    RedfishComputerSystem_V1_14_1_Links_CS                   *Links;                      // The links to other resources
                                                                                          // that are related to this
                                                                                          // resource.
    RedfishCS_bool                                           *LocationIndicatorActive;    // An indicator allowing an
                                                                                          // operator to physically locate
                                                                                          // this resource.
    RedfishCS_Link                                           LogServices;                 // The link to the collection of
                                                                                          // log services associated with
                                                                                          // this system.
    RedfishCS_char                                           *Manufacturer;               // The manufacturer or OEM of
                                                                                          // this system.
    RedfishCS_Link                                           Measurements;                // An array of DSP0274-defined
                                                                                          // measurement blocks.
    RedfishCS_Link                                           Memory;                      // The link to the collection of
                                                                                          // memory associated with this
                                                                                          // system.
    RedfishCS_Link                                           MemoryDomains;               // The link to the collection of
                                                                                          // memory domains associated with
                                                                                          // this system.
    RedfishComputerSystem_V1_14_1_MemorySummary_CS           *MemorySummary;              // The central memory of the
                                                                                          // system in general detail.
    RedfishCS_char                                           *Model;                      // The product name for this
                                                                                          // system, without the
                                                                                          // manufacturer name.
    RedfishCS_char                                           *Name;                   
    RedfishCS_Link                                           NetworkInterfaces;           // The link to the collection of
                                                                                          // Network Interfaces associated
                                                                                          // with this system.
    RedfishResource_Oem_CS                                   *Oem;                        // The OEM extension property.
    RedfishCS_Link                                           PCIeDevices;                 // The link to a collection of
                                                                                          // PCIe devices that this
                                                                                          // computer system uses.
    RedfishCS_int64                                          *PCIeDevicesodata_count; 
    RedfishCS_Link                                           PCIeFunctions;               // The link to a collection of
                                                                                          // PCIe functions that this
                                                                                          // computer system uses.
    RedfishCS_int64                                          *PCIeFunctionsodata_count;
    RedfishCS_char                                           *PartNumber;                 // The part number for this
                                                                                          // system.
    RedfishCS_int64                                          *PowerCycleDelaySeconds;     // The number of seconds to delay
                                                                                          // power on after a `Reset`
                                                                                          // action requesting
                                                                                          // `PowerCycle`.  Zero seconds
                                                                                          // indicates no delay.
    RedfishCS_int64                                          *PowerOffDelaySeconds;       // The number of seconds to delay
                                                                                          // power off during a reset.
                                                                                          // Zero seconds indicates no
                                                                                          // delay to power off.
    RedfishCS_int64                                          *PowerOnDelaySeconds;        // The number of seconds to delay
                                                                                          // power on after a power cycle
                                                                                          // or during a reset.  Zero
                                                                                          // seconds indicates no delay to
                                                                                          // power up.
    RedfishCS_char                                           *PowerRestorePolicy;         // The desired power state of the
                                                                                          // system when power is restored
                                                                                          // after a power loss.
    RedfishCS_char                                           *PowerState;                 // The current power state of the
                                                                                          // system.
    RedfishComputerSystem_V1_14_1_ProcessorSummary_CS        *ProcessorSummary;           // The central processors of the
                                                                                          // system in general detail.
    RedfishCS_Link                                           Processors;                  // The link to the collection of
                                                                                          // processors associated with
                                                                                          // this system.
    RedfishCS_Link                                           Redundancy;                  // The link to a collection of
                                                                                          // redundancy entities.  Each
                                                                                          // entity specifies a kind and
                                                                                          // level of redundancy and a
                                                                                          // collection, or redundancy set,
                                                                                          // of other computer systems that
                                                                                          // provide the specified
                                                                                          // redundancy to this computer
                                                                                          // system.
    RedfishCS_int64                                          *Redundancyodata_count;  
    RedfishCS_char                                           *SKU;                        // The manufacturer SKU for this
                                                                                          // system.
    RedfishCS_Link                                           SecureBoot;                  // The link to the UEFI Secure
                                                                                          // Boot associated with this
                                                                                          // system.
    RedfishComputerSystem_V1_14_1_HostSerialConsole_CS       *SerialConsole;              // The serial console services
                                                                                          // that this system provides.
    RedfishCS_char                                           *SerialNumber;               // The serial number for this
                                                                                          // system.
    RedfishCS_Link                                           SimpleStorage;               // The link to the collection of
                                                                                          // storage devices associated
                                                                                          // with this system.
    RedfishResource_Status_CS                                *Status;                     // The status and health of the
                                                                                          // resource and its subordinate
                                                                                          // or dependent resources.
    RedfishCS_Link                                           Storage;                     // The link to the collection of
                                                                                          // storage devices associated
                                                                                          // with this system.
    RedfishCS_char                                           *SubModel;                   // The sub-model for this system.
    RedfishCS_char                                           *SystemType;                 // The type of computer system
                                                                                          // that this resource represents.
    RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS    *TrustedModules;             // An array of trusted modules in
                                                                                          // the system.
    RedfishCS_char                                           *UUID;                       // The UUID for this system.
    RedfishCS_Link                                           VirtualMedia;                // The link to the virtual media
                                                                                          // services for this system.
    RedfishComputerSystem_V1_14_1_VirtualMediaConfig_CS      *VirtualMediaConfig;         // The information about the
                                                                                          // virtual media service of this
                                                                                          // system.
} RedfishComputerSystem_V1_14_1_ComputerSystem_CS;

#ifndef Redfishodatav4_idRef_Array_CS_
#define Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS  {
    Redfishodatav4_idRef_Array_CS    *Next;
    Redfishodatav4_idRef_CS    *ArrayValue;
} Redfishodatav4_idRef_Array_CS;
#endif

typedef struct _RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS  {
    RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS    *Next;
    RedfishComputerSystem_V1_14_1_TrustedModules_CS    *ArrayValue;
} RedfishComputerSystem_V1_14_1_TrustedModules_Array_CS;

#ifndef RedfishResource_Condition_Array_CS_
#define RedfishResource_Condition_Array_CS_
typedef struct _RedfishResource_Condition_Array_CS  {
    RedfishResource_Condition_Array_CS    *Next;
    RedfishResource_Condition_CS    *ArrayValue;
} RedfishResource_Condition_Array_CS;
#endif

RedfishCS_status
Json_ComputerSystem_V1_14_1_To_CS (char *JsonRawText, RedfishComputerSystem_V1_14_1_ComputerSystem_CS **ReturnedCS);

RedfishCS_status
CS_To_ComputerSystem_V1_14_1_JSON (RedfishComputerSystem_V1_14_1_ComputerSystem_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyComputerSystem_V1_14_1_CS (RedfishComputerSystem_V1_14_1_ComputerSystem_CS *CSPtr);

RedfishCS_status
DestroyComputerSystem_V1_14_1_Json (RedfishCS_char *JsonText);

#endif
