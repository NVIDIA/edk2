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

#ifndef RedfishSecureboot_V1_1_0_CSTRUCT_H_
#define RedfishSecureboot_V1_1_0_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

typedef struct _RedfishSecureBoot_V1_1_0_Actions_CS RedfishSecureBoot_V1_1_0_Actions_CS;
typedef struct _RedfishSecureBoot_V1_1_0_OemActions_CS RedfishSecureBoot_V1_1_0_OemActions_CS;
typedef struct _RedfishSecureBoot_V1_1_0_ResetKeys_CS RedfishSecureBoot_V1_1_0_ResetKeys_CS;
typedef struct _RedfishSecureBoot_V1_1_0_SecureBoot_CS RedfishSecureBoot_V1_1_0_SecureBoot_CS;
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
// The available actions for this resource.
//
typedef struct _RedfishSecureBoot_V1_1_0_Actions_CS {
    RedfishSecureBoot_V1_1_0_ResetKeys_CS     *SecureBoot_ResetKeys;
    RedfishSecureBoot_V1_1_0_OemActions_CS    *Oem;                    // The available OEM-specific
                                                                       // actions for this resource.
} RedfishSecureBoot_V1_1_0_Actions_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishSecureBoot_V1_1_0_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishSecureBoot_V1_1_0_OemActions_CS;

//
// This action resets the UEFI Secure Boot keys.
//
typedef struct _RedfishSecureBoot_V1_1_0_ResetKeys_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishSecureBoot_V1_1_0_ResetKeys_CS;

//
// The SecureBoot schema contains UEFI Secure Boot information and represents
// properties for managing the UEFI Secure Boot functionality of a system.
//
typedef struct _RedfishSecureBoot_V1_1_0_SecureBoot_CS {
    RedfishCS_Header                       Header;
    RedfishCS_char                         *odata_context;       
    RedfishCS_char                         *odata_etag;          
    RedfishCS_char                         *odata_id;            
    RedfishCS_char                         *odata_type;          
    RedfishSecureBoot_V1_1_0_Actions_CS    *Actions;                 // The available actions for this
                                                                     // resource.
    RedfishCS_char                         *Description;         
    RedfishCS_char                         *Id;                  
    RedfishCS_char                         *Name;                
    RedfishResource_Oem_CS                 *Oem;                     // The OEM extension property.
    RedfishCS_char                         *SecureBootCurrentBoot;    // The UEFI Secure Boot state
                                                                     // during the current boot cycle.
    RedfishCS_Link                         SecureBootDatabases;      // A link to the collection of
                                                                     // UEFI Secure Boot databases.
    RedfishCS_bool                         *SecureBootEnable;        // An indication of whether UEFI
                                                                     // Secure Boot is enabled.
    RedfishCS_char                         *SecureBootMode;          // The current UEFI Secure Boot
                                                                     // Mode.
} RedfishSecureBoot_V1_1_0_SecureBoot_CS;

RedfishCS_status
Json_SecureBoot_V1_1_0_To_CS (char *JsonRawText, RedfishSecureBoot_V1_1_0_SecureBoot_CS **ReturnedCS);

RedfishCS_status
CS_To_SecureBoot_V1_1_0_JSON (RedfishSecureBoot_V1_1_0_SecureBoot_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroySecureBoot_V1_1_0_CS (RedfishSecureBoot_V1_1_0_SecureBoot_CS *CSPtr);

RedfishCS_status
DestroySecureBoot_V1_1_0_Json (RedfishCS_char *JsonText);

#endif
