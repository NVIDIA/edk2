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

#ifndef RedfishBootoption_V1_0_0_CSTRUCT_H_
#define RedfishBootoption_V1_0_0_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef Redfishodata_V4_0_1_idRef_Array_CS_
typedef struct _Redfishodata_V4_0_1_idRef_Array_CS Redfishodata_V4_0_1_idRef_Array_CS;
#endif
typedef struct _RedfishBootOption_V1_0_0_Actions_CS RedfishBootOption_V1_0_0_Actions_CS;
typedef struct _RedfishBootOption_V1_0_0_BootOption_CS RedfishBootOption_V1_0_0_BootOption_CS;
typedef struct _RedfishBootOption_V1_0_0_OemActions_CS RedfishBootOption_V1_0_0_OemActions_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

#ifndef Redfishodata_V4_0_1_idRef_CS_
typedef struct _Redfishodata_V4_0_1_idRef_CS Redfishodata_V4_0_1_idRef_CS;
#endif

//
// The available actions for this resource.
//
typedef struct _RedfishBootOption_V1_0_0_Actions_CS {
    RedfishBootOption_V1_0_0_OemActions_CS    *Oem;
} RedfishBootOption_V1_0_0_Actions_CS;

//
// The available OEM specific actions for this resource.
//
typedef struct _RedfishBootOption_V1_0_0_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishBootOption_V1_0_0_OemActions_CS;

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
#ifndef Redfishodata_V4_0_1_idRef_CS_
#define Redfishodata_V4_0_1_idRef_CS_
typedef struct _Redfishodata_V4_0_1_idRef_CS {
    RedfishCS_char    *odata_id;
} Redfishodata_V4_0_1_idRef_CS;
#endif

//
// This is the schema definition for the BootOption resource. It represents the
// properties of a bootable device available in the System.
//
typedef struct _RedfishBootOption_V1_0_0_BootOption_CS {
    RedfishCS_Header                       Header;
    RedfishCS_char                         *odata_context;        
    RedfishCS_char                         *odata_id;             
    RedfishCS_char                         *odata_type;           
    RedfishBootOption_V1_0_0_Actions_CS    *Actions;                  // The available actions for this
                                                                      // resource.
    RedfishCS_char                         *Alias;                    // The alias of this Boot Source
                                                                      // when described in the
                                                                      // BootSourceOverrideTarget
                                                                      // property in the Computersystem
                                                                      // resource.
    RedfishCS_bool                         *BootOptionEnabled;        // A flag that shows if the Boot
                                                                      // Option is enabled.
    RedfishCS_char                         *BootOptionReference;      // The unique boot option string
                                                                      // that is referenced in the
                                                                      // BootOrder.
    RedfishCS_char                         *Description;          
    RedfishCS_char                         *DisplayName;              // The user-readable display
                                                                      // string of the Boot Option.
    RedfishCS_char                         *Id;                   
    RedfishCS_char                         *Name;                 
    RedfishResource_Oem_CS                 *Oem;                      // This is the
                                                                      // manufacturer/provider specific
                                                                      // extension moniker used to
                                                                      // divide the Oem object into
                                                                      // sections.
    Redfishodata_V4_0_1_idRef_Array_CS     *RelatedItem;              // The ID(s) of the resources
                                                                      // associated with this Boot
                                                                      // Option.
    RedfishCS_int64                        *RelatedItemodata_count;
    RedfishCS_char                         *UefiDevicePath;           // The UEFI device path used to
                                                                      // access this UEFI Boot Option.
} RedfishBootOption_V1_0_0_BootOption_CS;

#ifndef Redfishodata_V4_0_1_idRef_Array_CS_
#define Redfishodata_V4_0_1_idRef_Array_CS_
typedef struct _Redfishodata_V4_0_1_idRef_Array_CS  {
    Redfishodata_V4_0_1_idRef_Array_CS    *Next;
    Redfishodata_V4_0_1_idRef_CS    *ArrayValue;
} Redfishodata_V4_0_1_idRef_Array_CS;
#endif

RedfishCS_status
Json_BootOption_V1_0_0_To_CS (char *JsonRawText, RedfishBootOption_V1_0_0_BootOption_CS **ReturnedCS);

RedfishCS_status
CS_To_BootOption_V1_0_0_JSON (RedfishBootOption_V1_0_0_BootOption_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyBootOption_V1_0_0_CS (RedfishBootOption_V1_0_0_BootOption_CS *CSPtr);

RedfishCS_status
DestroyBootOption_V1_0_0_Json (RedfishCS_char *JsonText);

#endif
