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

#ifndef RedfishCertificate_V1_1_1_CSTRUCT_H_
#define RedfishCertificate_V1_1_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

typedef struct _RedfishCertificate_V1_1_1_Actions_CS RedfishCertificate_V1_1_1_Actions_CS;
typedef struct _RedfishCertificate_V1_1_1_Certificate_CS RedfishCertificate_V1_1_1_Certificate_CS;
typedef struct _RedfishCertificate_V1_1_1_Identifier_CS RedfishCertificate_V1_1_1_Identifier_CS;
typedef struct _RedfishCertificate_V1_1_1_OemActions_CS RedfishCertificate_V1_1_1_OemActions_CS;
typedef struct _RedfishCertificate_V1_1_1_Rekey_CS RedfishCertificate_V1_1_1_Rekey_CS;
typedef struct _RedfishCertificate_V1_1_1_RekeyResponse_CS RedfishCertificate_V1_1_1_RekeyResponse_CS;
typedef struct _RedfishCertificate_V1_1_1_Renew_CS RedfishCertificate_V1_1_1_Renew_CS;
typedef struct _RedfishCertificate_V1_1_1_RenewResponse_CS RedfishCertificate_V1_1_1_RenewResponse_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

//
// The available actions for this Resource.
//
typedef struct _RedfishCertificate_V1_1_1_Actions_CS {
    RedfishCertificate_V1_1_1_Rekey_CS         *Certificate_Rekey;
    RedfishCertificate_V1_1_1_Renew_CS         *Certificate_Renew;
    RedfishCertificate_V1_1_1_OemActions_CS    *Oem;                 // The available OEM-specific
                                                                     // actions for this Resource.
} RedfishCertificate_V1_1_1_Actions_CS;

//
// The identifier information about a certificate.
//
typedef struct _RedfishCertificate_V1_1_1_Identifier_CS {
    RedfishCS_char    *City;                 // The city or locality of the
                                             // organization of the entity.
    RedfishCS_char    *CommonName;           // The fully qualified domain
                                             // name of the entity.
    RedfishCS_char    *Country;              // The country of the
                                             // organization of the entity.
    RedfishCS_char    *Email;                // The email address of the
                                             // contact within the
                                             // organization of the entity.
    RedfishCS_char    *Organization;         // The name of the organization
                                             // of the entity.
    RedfishCS_char    *OrganizationalUnit;    // The name of the unit or
                                             // division of the organization
                                             // of the entity.
    RedfishCS_char    *State;                // The state, province, or region
                                             // of the organization of the
                                             // entity.
} RedfishCertificate_V1_1_1_Identifier_CS;

//
// The available OEM-specific actions for this Resource.
//
typedef struct _RedfishCertificate_V1_1_1_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishCertificate_V1_1_1_OemActions_CS;

//
// This action generates a new key-pair for a certificate and produces a
// certificate signing request.
//
typedef struct _RedfishCertificate_V1_1_1_Rekey_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishCertificate_V1_1_1_Rekey_CS;

//
// The response body for the Rekey action.
//
typedef struct _RedfishCertificate_V1_1_1_RekeyResponse_CS {
    RedfishCS_char    *CSRString;    // The string for the certificate
                                     // signing request.
    RedfishCS_Link    Certificate;    // The link to the certificate
                                     // being rekeyed.
} RedfishCertificate_V1_1_1_RekeyResponse_CS;

//
// This action generates a certificate signing request by using the existing
// information and key-pair of the certificate.
//
typedef struct _RedfishCertificate_V1_1_1_Renew_CS {
    RedfishCS_char    *target;    // Link to invoke action
    RedfishCS_char    *title;    // Friendly action name
} RedfishCertificate_V1_1_1_Renew_CS;

//
// The response body for the Renew action.
//
typedef struct _RedfishCertificate_V1_1_1_RenewResponse_CS {
    RedfishCS_char    *CSRString;    // The string for the certificate
                                     // signing request.
    RedfishCS_Link    Certificate;    // The link to the certificate
                                     // being renewed.
} RedfishCertificate_V1_1_1_RenewResponse_CS;

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
// The Certificate schema describes a certificate that proves the identify of a
// component, account, or service.
//
typedef struct _RedfishCertificate_V1_1_1_Certificate_CS {
    RedfishCS_Header                           Header;
    RedfishCS_char                             *odata_context;   
    RedfishCS_char                             *odata_etag;      
    RedfishCS_char                             *odata_id;        
    RedfishCS_char                             *odata_type;      
    RedfishCertificate_V1_1_1_Actions_CS       *Actions;             // The available actions for this
                                                                     // Resource.
    RedfishCS_char                             *CertificateString;    // The string for the
                                                                     // certificate.
    RedfishCS_char                             *CertificateType;     // The format of the certificate.
    RedfishCS_char                             *Description;     
    RedfishCS_char                             *Id;              
    RedfishCertificate_V1_1_1_Identifier_CS    *Issuer;              // The issuer of the certificate.
    RedfishCS_char_Array                       *KeyUsage;            // The key usage extension, which
                                                                     // defines the purpose of the
                                                                     // public keys in this
                                                                     // certificate.
    RedfishCS_char                             *Name;            
    RedfishResource_Oem_CS                     *Oem;                 // The OEM extension property.
    RedfishCertificate_V1_1_1_Identifier_CS    *Subject;             // The subject of the
                                                                     // certificate.
    RedfishCS_char                             *ValidNotAfter;       // The date when the certificate
                                                                     // is no longer valid.
    RedfishCS_char                             *ValidNotBefore;      // The date when the certificate
                                                                     // becomes valid.
} RedfishCertificate_V1_1_1_Certificate_CS;

RedfishCS_status
Json_Certificate_V1_1_1_To_CS (char *JsonRawText, RedfishCertificate_V1_1_1_Certificate_CS **ReturnedCS);

RedfishCS_status
CS_To_Certificate_V1_1_1_JSON (RedfishCertificate_V1_1_1_Certificate_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyCertificate_V1_1_1_CS (RedfishCertificate_V1_1_1_Certificate_CS *CSPtr);

RedfishCS_status
DestroyCertificate_V1_1_1_Json (RedfishCS_char *JsonText);

#endif
