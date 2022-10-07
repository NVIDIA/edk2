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

#ifndef RedfishBios_V1_1_0_CSTRUCT_H_
#define RedfishBios_V1_1_0_CSTRUCT_H_

#include "RedfishCsCommon.h"

typedef struct _RedfishBios_V1_1_0_Actions_CS RedfishBios_V1_1_0_Actions_CS;
typedef struct _RedfishBios_V1_1_0_Attributes_CS RedfishBios_V1_1_0_Attributes_CS;
typedef struct _RedfishBios_V1_1_0_Bios_CS RedfishBios_V1_1_0_Bios_CS;
typedef struct _RedfishBios_V1_1_0_ChangePassword_CS RedfishBios_V1_1_0_ChangePassword_CS;
typedef struct _RedfishBios_V1_1_0_Links_CS RedfishBios_V1_1_0_Links_CS;
typedef struct _RedfishBios_V1_1_0_OemActions_CS RedfishBios_V1_1_0_OemActions_CS;
typedef struct _RedfishBios_V1_1_0_ResetBios_CS RedfishBios_V1_1_0_ResetBios_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

//
// The available actions for this Resource.
//
typedef struct _RedfishBios_V1_1_0_Actions_CS {
    RedfishBios_V1_1_0_ChangePassword_CS    *Bios_ChangePassword;
    RedfishBios_V1_1_0_ResetBios_CS         *Bios_ResetBios;    
    RedfishBios_V1_1_0_OemActions_CS        *Oem;                   // The available OEM-specific
                                                                    // actions for this Resource.
} RedfishBios_V1_1_0_Actions_CS;

//
// The list of BIOS attributes and their values as determined by the manufacturer
// or provider.
//
typedef struct _RedfishBios_V1_1_0_Attributes_CS {
    RedfishCS_Link    Prop;
} RedfishBios_V1_1_0_Attributes_CS;

//
// This action changes a BIOS password.
//
typedef struct _RedfishBios_V1_1_0_ChangePassword_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishBios_V1_1_0_ChangePassword_CS;

//
// The links to other Resources that are related to this Resource.
//
typedef struct _RedfishBios_V1_1_0_Links_CS {
    RedfishCS_Link            ActiveSoftwareImage;          // The link to the
                                                            // SoftwareInventory that
                                                            // represents the active BIOS
                                                            // firmware image.
    RedfishResource_Oem_CS    *Oem;                         // The OEM extension property.
    RedfishCS_Link            SoftwareImages;               // The images that are associated
                                                            // with this BIOS.
    RedfishCS_int64           *SoftwareImagesodata_count;
} RedfishBios_V1_1_0_Links_CS;

//
// The available OEM-specific actions for this Resource.
//
typedef struct _RedfishBios_V1_1_0_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishBios_V1_1_0_OemActions_CS;

//
// This action resets the BIOS attributes to default.
//
typedef struct _RedfishBios_V1_1_0_ResetBios_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishBios_V1_1_0_ResetBios_CS;

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
// The Bios schema contains properties related to the BIOS Attribute Registry.
// The Attribute Registry describes the system-specific BIOS attributes and
// actions for changing to BIOS settings.  Changes to the BIOS typically require a
// system reset before they take effect.  It is likely that a client finds the
// `@Redfish.Settings` term in this Resource, and if it is found, the client makes
// requests to change BIOS settings by modifying the Resource identified by the
// `@Redfish.Settings` term.
//
typedef struct _RedfishBios_V1_1_0_Bios_CS {
    RedfishCS_Header                    Header;
    RedfishCS_char                      *odata_context;   
    RedfishCS_char                      *odata_etag;      
    RedfishCS_char                      *odata_id;        
    RedfishCS_char                      *odata_type;      
    RedfishBios_V1_1_0_Actions_CS       *Actions;             // The available actions for this
                                                              // Resource.
    RedfishCS_char                      *AttributeRegistry;    // The Resource ID of the
                                                              // Attribute Registry that has
                                                              // the system-specific
                                                              // information about a BIOS
                                                              // Resource.
    RedfishBios_V1_1_0_Attributes_CS    *Attributes;          // The list of BIOS attributes
                                                              // specific to the manufacturer
                                                              // or provider.
    RedfishCS_char                      *Description;     
    RedfishCS_char                      *Id;              
    RedfishBios_V1_1_0_Links_CS         *Links;               // The links to other Resources
                                                              // that are related to this
                                                              // Resource.
    RedfishCS_char                      *Name;            
    RedfishResource_Oem_CS              *Oem;                 // The OEM extension property.
} RedfishBios_V1_1_0_Bios_CS;

RedfishCS_status
Json_Bios_V1_1_0_To_CS (char *JsonRawText, RedfishBios_V1_1_0_Bios_CS **ReturnedCS);

RedfishCS_status
CS_To_Bios_V1_1_0_JSON (RedfishBios_V1_1_0_Bios_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyBios_V1_1_0_CS (RedfishBios_V1_1_0_Bios_CS *CSPtr);

RedfishCS_status
DestroyBios_V1_1_0_Json (RedfishCS_char *JsonText);

#endif
