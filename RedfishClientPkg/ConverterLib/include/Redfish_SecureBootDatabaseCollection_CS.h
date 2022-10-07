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

#ifndef RedfishSecurebootdatabasecollection_Noversioned_CSTRUCT_H_
#define RedfishSecurebootdatabasecollection_Noversioned_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

typedef struct _RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS;
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
// The collection of SecureBootDatabase resource instances.
//
typedef struct _RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS {
    RedfishCS_Header          Header;
    RedfishCS_char            *odata_context;       
    RedfishCS_char            *odata_etag;          
    RedfishCS_char            *odata_id;            
    RedfishCS_char            *odata_type;          
    RedfishCS_char            *Description;         
    RedfishCS_Link            Members;                  // The members of this
                                                        // collection.
    RedfishCS_int64           *Membersodata_count;  
    RedfishCS_char            *Membersodata_nextLink;
    RedfishCS_char            *Name;                
    RedfishResource_Oem_CS    *Oem;                     // The OEM extension property.
} RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS;

RedfishCS_status
Json_SecureBootDatabaseCollection_To_CS (char *JsonRawText, RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS **ReturnedCS);

RedfishCS_status
CS_To_SecureBootDatabaseCollection_JSON (RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroySecureBootDatabaseCollection_CS (RedfishSecureBootDatabaseCollection_SecureBootDatabaseCollection_CS *CSPtr);

RedfishCS_status
DestroySecureBootDatabaseCollection_Json (RedfishCS_char *JsonText);

#endif
