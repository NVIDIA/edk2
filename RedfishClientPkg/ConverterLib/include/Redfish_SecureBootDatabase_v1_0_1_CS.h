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

#ifndef RedfishSecurebootdatabase_V1_0_1_CSTRUCT_H_
#define RedfishSecurebootdatabase_V1_0_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

typedef struct _RedfishSecureBootDatabase_V1_0_1_Actions_CS RedfishSecureBootDatabase_V1_0_1_Actions_CS;
typedef struct _RedfishSecureBootDatabase_V1_0_1_OemActions_CS RedfishSecureBootDatabase_V1_0_1_OemActions_CS;
typedef struct _RedfishSecureBootDatabase_V1_0_1_ResetKeys_CS RedfishSecureBootDatabase_V1_0_1_ResetKeys_CS;
typedef struct _RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS;
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
typedef struct _RedfishSecureBootDatabase_V1_0_1_Actions_CS {
    RedfishSecureBootDatabase_V1_0_1_ResetKeys_CS     *SecureBootDatabase_ResetKeys;
    RedfishSecureBootDatabase_V1_0_1_OemActions_CS    *Oem;                            // The available OEM-specific
                                                                                       // actions for this resource.
} RedfishSecureBootDatabase_V1_0_1_Actions_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishSecureBootDatabase_V1_0_1_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishSecureBootDatabase_V1_0_1_OemActions_CS;

//
// This action is used to reset the UEFI Secure Boot keys of this database.
//
typedef struct _RedfishSecureBootDatabase_V1_0_1_ResetKeys_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishSecureBootDatabase_V1_0_1_ResetKeys_CS;

//
// The SecureBootDatabase schema describes a UEFI Secure Boot database used to
// store certificates or hashes.
//
typedef struct _RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS {
    RedfishCS_Header                               Header;
    RedfishCS_char                                 *odata_context;
    RedfishCS_char                                 *odata_etag;  
    RedfishCS_char                                 *odata_id;    
    RedfishCS_char                                 *odata_type;  
    RedfishSecureBootDatabase_V1_0_1_Actions_CS    *Actions;         // The available actions for this
                                                                     // resource.
    RedfishCS_Link                                 Certificates;     // A link to the collection of
                                                                     // certificates contained in this
                                                                     // UEFI Secure Boot database.
    RedfishCS_char                                 *DatabaseId;      // This property contains the
                                                                     // name of the UEFI Secure Boot
                                                                     // database.
    RedfishCS_char                                 *Description; 
    RedfishCS_char                                 *Id;          
    RedfishCS_char                                 *Name;        
    RedfishResource_Oem_CS                         *Oem;             // The OEM extension property.
    RedfishCS_Link                                 Signatures;       // A link to the collection of
                                                                     // signatures contained in this
                                                                     // UEFI Secure Boot database.
} RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS;

RedfishCS_status
Json_SecureBootDatabase_V1_0_1_To_CS (char *JsonRawText, RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS **ReturnedCS);

RedfishCS_status
CS_To_SecureBootDatabase_V1_0_1_JSON (RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroySecureBootDatabase_V1_0_1_CS (RedfishSecureBootDatabase_V1_0_1_SecureBootDatabase_CS *CSPtr);

RedfishCS_status
DestroySecureBootDatabase_V1_0_1_Json (RedfishCS_char *JsonText);

#endif
