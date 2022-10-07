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

#ifndef RedfishBios_V1_0_1_CSTRUCT_H_
#define RedfishBios_V1_0_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

typedef struct _RedfishBios_V1_0_1_Attributes_CS RedfishBios_V1_0_1_Attributes_CS;
typedef struct _RedfishBios_V1_0_1_Bios_CS RedfishBios_V1_0_1_Bios_CS;
typedef struct _RedfishBios_V1_0_1_Bios_Actions_CS RedfishBios_V1_0_1_Bios_Actions_CS;
typedef struct _RedfishBios_V1_0_1_Bios_Actions_Oem_CS RedfishBios_V1_0_1_Bios_Actions_Oem_CS;
typedef struct _RedfishBios_V1_0_1_ChangePassword_CS RedfishBios_V1_0_1_ChangePassword_CS;
typedef struct _RedfishBios_V1_0_1_ResetBios_CS RedfishBios_V1_0_1_ResetBios_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

typedef struct _RedfishBios_V1_0_1_Attributes_CS {
    RedfishCS_Link    Prop;
} RedfishBios_V1_0_1_Attributes_CS;

//
// The available actions for this resource.
//
typedef struct _RedfishBios_V1_0_1_Bios_Actions_CS {
    RedfishBios_V1_0_1_ChangePassword_CS      *Bios_ChangePassword;
    RedfishBios_V1_0_1_ResetBios_CS           *Bios_ResetBios;    
    RedfishBios_V1_0_1_Bios_Actions_Oem_CS    *Oem;               
} RedfishBios_V1_0_1_Bios_Actions_CS;

typedef struct _RedfishBios_V1_0_1_Bios_Actions_Oem_CS {
    RedfishCS_Link    Prop;
} RedfishBios_V1_0_1_Bios_Actions_Oem_CS;

//
// This action is used to change the BIOS passwords.
//
typedef struct _RedfishBios_V1_0_1_ChangePassword_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishBios_V1_0_1_ChangePassword_CS;

//
// This action is used to reset the BIOS attributes to default.
//
typedef struct _RedfishBios_V1_0_1_ResetBios_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishBios_V1_0_1_ResetBios_CS;

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
// Bios contains properties surrounding a BIOS Attribute Registry (where the
// system-specific BIOS attributes are described) and the Actions needed to
// perform changes to BIOS settings, which typically require a system reset to
// apply.
//
typedef struct _RedfishBios_V1_0_1_Bios_CS {
    RedfishCS_Header                      Header;
    RedfishCS_char                        *odata_context;   
    RedfishCS_char                        *odata_id;        
    RedfishCS_char                        *odata_type;      
    RedfishBios_V1_0_1_Bios_Actions_CS    *Actions;             // The available actions for this
                                                                // resource.
    RedfishCS_char                        *AttributeRegistry;    // The Resource ID of the
                                                                // Attribute Registry for the
                                                                // BIOS Attributes resource.
    RedfishBios_V1_0_1_Attributes_CS      *Attributes;          // This is the
                                                                // manufacturer/provider specific
                                                                // list of BIOS attributes.
    RedfishCS_char                        *Description;     
    RedfishCS_char                        *Id;              
    RedfishCS_char                        *Name;            
    RedfishResource_Oem_CS                *Oem;                 // This is the
                                                                // manufacturer/provider specific
                                                                // extension moniker used to
                                                                // divide the Oem object into
                                                                // sections.
} RedfishBios_V1_0_1_Bios_CS;

RedfishCS_status
Json_Bios_V1_0_1_To_CS (char *JsonRawText, RedfishBios_V1_0_1_Bios_CS **ReturnedCS);

RedfishCS_status
CS_To_Bios_V1_0_1_JSON (RedfishBios_V1_0_1_Bios_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyBios_V1_0_1_CS (RedfishBios_V1_0_1_Bios_CS *CSPtr);

RedfishCS_status
DestroyBios_V1_0_1_Json (RedfishCS_char *JsonText);

#endif
