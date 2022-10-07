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

#ifndef RedfishSignature_V1_0_1_CSTRUCT_H_
#define RedfishSignature_V1_0_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

typedef struct _RedfishSignature_V1_0_1_Actions_CS RedfishSignature_V1_0_1_Actions_CS;
typedef struct _RedfishSignature_V1_0_1_OemActions_CS RedfishSignature_V1_0_1_OemActions_CS;
typedef struct _RedfishSignature_V1_0_1_Signature_CS RedfishSignature_V1_0_1_Signature_CS;
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
typedef struct _RedfishSignature_V1_0_1_Actions_CS {
    RedfishSignature_V1_0_1_OemActions_CS    *Oem;    // The available OEM-specific
                                                     // actions for this resource.
} RedfishSignature_V1_0_1_Actions_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishSignature_V1_0_1_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishSignature_V1_0_1_OemActions_CS;

//
// The Signature schema describes a signature or a hash.
//
typedef struct _RedfishSignature_V1_0_1_Signature_CS {
    RedfishCS_Header                      Header;
    RedfishCS_char                        *odata_context;       
    RedfishCS_char                        *odata_etag;          
    RedfishCS_char                        *odata_id;            
    RedfishCS_char                        *odata_type;          
    RedfishSignature_V1_0_1_Actions_CS    *Actions;                 // The available actions for this
                                                                    // resource.
    RedfishCS_char                        *Description;         
    RedfishCS_char                        *Id;                  
    RedfishCS_char                        *Name;                
    RedfishResource_Oem_CS                *Oem;                     // The OEM extension property.
    RedfishCS_char                        *SignatureString;         // The string for the signature.
    RedfishCS_char                        *SignatureType;           // The format of the signature.
    RedfishCS_char                        *SignatureTypeRegistry;    // The type of the signature.
    RedfishCS_char                        *UefiSignatureOwner;      // The UEFI signature owner for
                                                                    // this signature.
} RedfishSignature_V1_0_1_Signature_CS;

RedfishCS_status
Json_Signature_V1_0_1_To_CS (char *JsonRawText, RedfishSignature_V1_0_1_Signature_CS **ReturnedCS);

RedfishCS_status
CS_To_Signature_V1_0_1_JSON (RedfishSignature_V1_0_1_Signature_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroySignature_V1_0_1_CS (RedfishSignature_V1_0_1_Signature_CS *CSPtr);

RedfishCS_status
DestroySignature_V1_0_1_Json (RedfishCS_char *JsonText);

#endif
