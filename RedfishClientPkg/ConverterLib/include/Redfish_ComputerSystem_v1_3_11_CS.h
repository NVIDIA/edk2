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

#ifndef RedfishComputersystem_V1_3_11_CSTRUCT_H_
#define RedfishComputersystem_V1_3_11_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS Redfishodatav4_idRef_Array_CS;
#endif
typedef struct _RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS;
#ifndef RedfishResource_Condition_Array_CS_
typedef struct _RedfishResource_Condition_Array_CS RedfishResource_Condition_Array_CS;
#endif
typedef struct _RedfishComputerSystem_V1_3_11_Actions_CS RedfishComputerSystem_V1_3_11_Actions_CS;
typedef struct _RedfishComputerSystem_V1_3_11_Boot_CS RedfishComputerSystem_V1_3_11_Boot_CS;
typedef struct _RedfishComputerSystem_V1_3_11_ComputerSystem_CS RedfishComputerSystem_V1_3_11_ComputerSystem_CS;
typedef struct _RedfishComputerSystem_V1_3_11_HostedServices_CS RedfishComputerSystem_V1_3_11_HostedServices_CS;
typedef struct _RedfishComputerSystem_V1_3_11_Links_CS RedfishComputerSystem_V1_3_11_Links_CS;
typedef struct _RedfishComputerSystem_V1_3_11_MemorySummary_CS RedfishComputerSystem_V1_3_11_MemorySummary_CS;
typedef struct _RedfishComputerSystem_V1_3_11_OemActions_CS RedfishComputerSystem_V1_3_11_OemActions_CS;
typedef struct _RedfishComputerSystem_V1_3_11_ProcessorSummary_CS RedfishComputerSystem_V1_3_11_ProcessorSummary_CS;
typedef struct _RedfishComputerSystem_V1_3_11_Reset_CS RedfishComputerSystem_V1_3_11_Reset_CS;
typedef struct _RedfishComputerSystem_V1_3_11_TrustedModules_CS RedfishComputerSystem_V1_3_11_TrustedModules_CS;
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
typedef struct _RedfishComputerSystem_V1_3_11_Actions_CS {
    RedfishComputerSystem_V1_3_11_Reset_CS         *ComputerSystem_Reset;
    RedfishComputerSystem_V1_3_11_OemActions_CS    *Oem;                    // The available OEM-specific
                                                                            // actions for this resource.
} RedfishComputerSystem_V1_3_11_Actions_CS;

//
// The boot information for this resource.
//
typedef struct _RedfishComputerSystem_V1_3_11_Boot_CS {
    RedfishCS_char    *BootSourceOverrideEnabled;      // The state of the boot source
                                                       // override feature.
    RedfishCS_char    *BootSourceOverrideMode;         // The BIOS boot mode to use when
                                                       // the system boots from the
                                                       // BootSourceOverrideTarget boot
                                                       // source.
    RedfishCS_char    *BootSourceOverrideTarget;       // The current boot source to use
                                                       // at the next boot instead of
                                                       // the normal boot device, if
                                                       // BootSourceOverrideEnabled is
                                                       // `true`.
    RedfishCS_char    *UefiTargetBootSourceOverride;    // The UEFI device path of the
                                                       // device from which to boot when
                                                       // BootSourceOverrideTarget is
                                                       // `UefiTarget`.
} RedfishComputerSystem_V1_3_11_Boot_CS;

//
// The services that might be running or installed on the system.
//
typedef struct _RedfishComputerSystem_V1_3_11_HostedServices_CS {
    RedfishResource_Oem_CS    *Oem;              // The OEM extension property.
    RedfishCS_Link            StorageServices;    // The link to a collection of
                                                 // storage services that this
                                                 // computer system supports.
} RedfishComputerSystem_V1_3_11_HostedServices_CS;

//
// The links to other resources that are related to this resource.
//
typedef struct _RedfishComputerSystem_V1_3_11_Links_CS {
    RedfishCS_Link                   Chassis;                 // An array of links to the
                                                              // chassis that contains this
                                                              // system.
    RedfishCS_int64                  *Chassisodata_count; 
    Redfishodatav4_idRef_Array_CS    *CooledBy;               // An array of links to resources
                                                              // or objects that that cool this
                                                              // computer system.  Normally,
                                                              // the link is for either a
                                                              // chassis or a specific set of
                                                              // fans.
    RedfishCS_int64                  *CooledByodata_count;
    RedfishCS_Link                   Endpoints;               // An array of links to the
                                                              // endpoints that connect to this
                                                              // system.
    RedfishCS_int64                  *Endpointsodata_count;
    RedfishCS_Link                   ManagedBy;               // An array of links to the
                                                              // managers responsible for this
                                                              // system.
    RedfishCS_int64                  *ManagedByodata_count;
    RedfishResource_Oem_CS           *Oem;                    // The OEM extension property.
    Redfishodatav4_idRef_Array_CS    *PoweredBy;              // An array of links to resources
                                                              // or objects that power this
                                                              // computer system.  Normally,
                                                              // the link is for either a
                                                              // chassis or a specific set of
                                                              // power supplies.
    RedfishCS_int64                  *PoweredByodata_count;
} RedfishComputerSystem_V1_3_11_Links_CS;

//
// The memory of the system in general detail.
//
typedef struct _RedfishComputerSystem_V1_3_11_MemorySummary_CS {
    RedfishCS_char               *MemoryMirroring;        // The ability and type of memory
                                                          // mirroring that this computer
                                                          // system supports.
    RedfishResource_Status_CS    *Status;                 // The status and health of the
                                                          // resource and its subordinate
                                                          // or dependent resources.
    RedfishCS_int64              *TotalSystemMemoryGiB;    // The total configured operating
                                                          // system-accessible memory
                                                          // (RAM), measured in GiB.
} RedfishComputerSystem_V1_3_11_MemorySummary_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishComputerSystem_V1_3_11_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishComputerSystem_V1_3_11_OemActions_CS;

//
// The central processors of the system in general detail.
//
typedef struct _RedfishComputerSystem_V1_3_11_ProcessorSummary_CS {
    RedfishCS_int64              *Count;    // The number of physical
                                            // processors in the system.
    RedfishCS_char               *Model;    // The processor model for the
                                            // primary or majority of
                                            // processors in this system.
    RedfishResource_Status_CS    *Status;    // The status and health of the
                                            // resource and its subordinate
                                            // or dependent resources.
} RedfishComputerSystem_V1_3_11_ProcessorSummary_CS;

//
// This action resets the system.
//
typedef struct _RedfishComputerSystem_V1_3_11_Reset_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishComputerSystem_V1_3_11_Reset_CS;

//
// The Trusted Module installed in the system.
//
typedef struct _RedfishComputerSystem_V1_3_11_TrustedModules_CS {
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
} RedfishComputerSystem_V1_3_11_TrustedModules_CS;

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
typedef struct _RedfishComputerSystem_V1_3_11_ComputerSystem_CS {
    RedfishCS_Header                                         Header;
    RedfishCS_char                                           *odata_context;          
    RedfishCS_char                                           *odata_etag;             
    RedfishCS_char                                           *odata_id;               
    RedfishCS_char                                           *odata_type;             
    RedfishComputerSystem_V1_3_11_Actions_CS                 *Actions;                    // The available actions for this
                                                                                          // resource.
    RedfishCS_char                                           *AssetTag;                   // The user-definable tag that
                                                                                          // can track this computer system
                                                                                          // for inventory or other client
                                                                                          // purposes.
    RedfishCS_Link                                           Bios;                        // The link to the BIOS settings
                                                                                          // associated with this system.
    RedfishCS_char                                           *BiosVersion;                // The version of the system BIOS
                                                                                          // or primary system firmware.
    RedfishComputerSystem_V1_3_11_Boot_CS                    *Boot;                       // The boot settings for this
                                                                                          // system.
    RedfishCS_char                                           *Description;            
    RedfishCS_Link                                           EthernetInterfaces;          // The link to the collection of
                                                                                          // Ethernet interfaces associated
                                                                                          // with this system.
    RedfishCS_char                                           *HostName;                   // The DNS host name, without any
                                                                                          // domain information.
    RedfishComputerSystem_V1_3_11_HostedServices_CS          *HostedServices;             // The services that this
                                                                                          // computer system supports.
    RedfishCS_char_Array                                     *HostingRoles;               // The hosting roles that this
                                                                                          // computer system supports.
    RedfishCS_char                                           *Id;                     
    RedfishCS_char                                           *IndicatorLED;               // The state of the indicator
                                                                                          // LED, which identifies the
                                                                                          // system.
    RedfishComputerSystem_V1_3_11_Links_CS                   *Links;                      // The links to other resources
                                                                                          // that are related to this
                                                                                          // resource.
    RedfishCS_Link                                           LogServices;                 // The link to the collection of
                                                                                          // log services associated with
                                                                                          // this system.
    RedfishCS_char                                           *Manufacturer;               // The manufacturer or OEM of
                                                                                          // this system.
    RedfishCS_Link                                           Memory;                      // The link to the collection of
                                                                                          // memory associated with this
                                                                                          // system.
    RedfishCS_Link                                           MemoryDomains;               // The link to the collection of
                                                                                          // memory domains associated with
                                                                                          // this system.
    RedfishComputerSystem_V1_3_11_MemorySummary_CS           *MemorySummary;              // The central memory of the
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
    RedfishCS_char                                           *PowerState;                 // The current power state of the
                                                                                          // system.
    RedfishComputerSystem_V1_3_11_ProcessorSummary_CS        *ProcessorSummary;           // The central processors of the
                                                                                          // system in general detail.
    RedfishCS_Link                                           Processors;                  // The link to the collection of
                                                                                          // processors associated with
                                                                                          // this system.
    RedfishCS_char                                           *SKU;                        // The manufacturer SKU for this
                                                                                          // system.
    RedfishCS_Link                                           SecureBoot;                  // The link to the UEFI Secure
                                                                                          // Boot associated with this
                                                                                          // system.
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
    RedfishCS_char                                           *SystemType;                 // The type of computer system
                                                                                          // that this resource represents.
    RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS    *TrustedModules;             // An array of trusted modules in
                                                                                          // the system.
    RedfishCS_char                                           *UUID;                       // The UUID for this system.
} RedfishComputerSystem_V1_3_11_ComputerSystem_CS;

#ifndef Redfishodatav4_idRef_Array_CS_
#define Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS  {
    Redfishodatav4_idRef_Array_CS    *Next;
    Redfishodatav4_idRef_CS    *ArrayValue;
} Redfishodatav4_idRef_Array_CS;
#endif

typedef struct _RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS  {
    RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS    *Next;
    RedfishComputerSystem_V1_3_11_TrustedModules_CS    *ArrayValue;
} RedfishComputerSystem_V1_3_11_TrustedModules_Array_CS;

#ifndef RedfishResource_Condition_Array_CS_
#define RedfishResource_Condition_Array_CS_
typedef struct _RedfishResource_Condition_Array_CS  {
    RedfishResource_Condition_Array_CS    *Next;
    RedfishResource_Condition_CS    *ArrayValue;
} RedfishResource_Condition_Array_CS;
#endif

RedfishCS_status
Json_ComputerSystem_V1_3_11_To_CS (char *JsonRawText, RedfishComputerSystem_V1_3_11_ComputerSystem_CS **ReturnedCS);

RedfishCS_status
CS_To_ComputerSystem_V1_3_11_JSON (RedfishComputerSystem_V1_3_11_ComputerSystem_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyComputerSystem_V1_3_11_CS (RedfishComputerSystem_V1_3_11_ComputerSystem_CS *CSPtr);

RedfishCS_status
DestroyComputerSystem_V1_3_11_Json (RedfishCS_char *JsonText);

#endif
