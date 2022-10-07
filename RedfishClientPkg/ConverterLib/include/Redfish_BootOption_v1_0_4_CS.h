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

#ifndef RedfishBootoption_V1_0_4_CSTRUCT_H_
#define RedfishBootoption_V1_0_4_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS Redfishodatav4_idRef_Array_CS;
#endif
typedef struct _RedfishBootOption_V1_0_4_Actions_CS RedfishBootOption_V1_0_4_Actions_CS;
typedef struct _RedfishBootOption_V1_0_4_BootOption_CS RedfishBootOption_V1_0_4_BootOption_CS;
typedef struct _RedfishBootOption_V1_0_4_OemActions_CS RedfishBootOption_V1_0_4_OemActions_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

#ifndef Redfishodatav4_idRef_CS_
typedef struct _Redfishodatav4_idRef_CS Redfishodatav4_idRef_CS;
#endif

//
// The available actions for this resource.
//
typedef struct _RedfishBootOption_V1_0_4_Actions_CS {
    RedfishBootOption_V1_0_4_OemActions_CS    *Oem;    // The available OEM-specific
                                                      // actions for this resource.
} RedfishBootOption_V1_0_4_Actions_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishBootOption_V1_0_4_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishBootOption_V1_0_4_OemActions_CS;

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
// A reference to a resource.
//
#ifndef Redfishodatav4_idRef_CS_
#define Redfishodatav4_idRef_CS_
typedef struct _Redfishodatav4_idRef_CS {
    RedfishCS_char    *odata_id;
} Redfishodatav4_idRef_CS;
#endif

//
// The BootOption schema reports information about a single boot option in a
// system.  It represents the properties of a bootable device available in the
// system.
//
typedef struct _RedfishBootOption_V1_0_4_BootOption_CS {
    RedfishCS_Header                       Header;
    RedfishCS_char                         *odata_context;        
    RedfishCS_char                         *odata_etag;           
    RedfishCS_char                         *odata_id;             
    RedfishCS_char                         *odata_type;           
    RedfishBootOption_V1_0_4_Actions_CS    *Actions;                  // The available actions for this
                                                                      // resource.
    RedfishCS_char                         *Alias;                    // The alias of this boot source.
    RedfishCS_bool                         *BootOptionEnabled;        // An indication of whether the
                                                                      // boot option is enabled.  If
                                                                      // `true`, it is enabled.  If
                                                                      // `false`, the boot option that
                                                                      // the boot order array on the
                                                                      // computer system contains is
                                                                      // skipped.  In the UEFI context,
                                                                      // this property shall influence
                                                                      // the load option active flag
                                                                      // for the boot option.
    RedfishCS_char                         *BootOptionReference;      // The unique boot option.
    RedfishCS_char                         *Description;          
    RedfishCS_char                         *DisplayName;              // The user-readable display name
                                                                      // of the boot option that
                                                                      // appears in the boot order list
                                                                      // in the user interface.
    RedfishCS_char                         *Id;                   
    RedfishCS_char                         *Name;                 
    RedfishResource_Oem_CS                 *Oem;                      // The OEM extension property.
    Redfishodatav4_idRef_Array_CS          *RelatedItem;              // An array of links to resources
                                                                      // or objects associated with
                                                                      // this boot option.
    RedfishCS_int64                        *RelatedItemodata_count;
    RedfishCS_char                         *UefiDevicePath;           // The UEFI device path to access
                                                                      // this UEFI boot option.
} RedfishBootOption_V1_0_4_BootOption_CS;

#ifndef Redfishodatav4_idRef_Array_CS_
#define Redfishodatav4_idRef_Array_CS_
typedef struct _Redfishodatav4_idRef_Array_CS  {
    Redfishodatav4_idRef_Array_CS    *Next;
    Redfishodatav4_idRef_CS    *ArrayValue;
} Redfishodatav4_idRef_Array_CS;
#endif

RedfishCS_status
Json_BootOption_V1_0_4_To_CS (char *JsonRawText, RedfishBootOption_V1_0_4_BootOption_CS **ReturnedCS);

RedfishCS_status
CS_To_BootOption_V1_0_4_JSON (RedfishBootOption_V1_0_4_BootOption_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyBootOption_V1_0_4_CS (RedfishBootOption_V1_0_4_BootOption_CS *CSPtr);

RedfishCS_status
DestroyBootOption_V1_0_4_Json (RedfishCS_char *JsonText);

#endif
