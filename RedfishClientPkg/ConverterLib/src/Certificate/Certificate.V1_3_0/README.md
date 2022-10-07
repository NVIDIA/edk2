# Definition of Certificate.V1_3_0 and functions<br><br>

## Actions
    typedef struct _RedfishCertificate_V1_3_0_Actions_CS {
        RedfishCertificate_V1_3_0_Rekey_CS *Certificate_Rekey;
        RedfishCertificate_V1_3_0_Renew_CS *Certificate_Renew;
        RedfishCertificate_V1_3_0_OemActions_CS *Oem;
    } RedfishCertificate_V1_3_0_Actions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Certificate_Rekey**|RedfishCertificate_V1_3_0_Rekey_CS| Structure points to **#Certificate.Rekey** property.| No| No
|**Certificate_Renew**|RedfishCertificate_V1_3_0_Renew_CS| Structure points to **#Certificate.Renew** property.| No| No
|**Oem**|RedfishCertificate_V1_3_0_OemActions_CS| Structure points to **Oem** property.| No| No


## Identifier
    typedef struct _RedfishCertificate_V1_3_0_Identifier_CS {
        RedfishCS_char *City;
        RedfishCS_char *CommonName;
        RedfishCS_char *Country;
        RedfishCS_char *Email;
        RedfishCS_char *Organization;
        RedfishCS_char *OrganizationalUnit;
        RedfishCS_char *State;
    } RedfishCertificate_V1_3_0_Identifier_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**City**|RedfishCS_char| String pointer to **City** property.| No| Yes
|**CommonName**|RedfishCS_char| String pointer to **CommonName** property.| No| Yes
|**Country**|RedfishCS_char| String pointer to **Country** property.| No| Yes
|**Email**|RedfishCS_char| String pointer to **Email** property.| No| Yes
|**Organization**|RedfishCS_char| String pointer to **Organization** property.| No| Yes
|**OrganizationalUnit**|RedfishCS_char| String pointer to **OrganizationalUnit** property.| No| Yes
|**State**|RedfishCS_char| String pointer to **State** property.| No| Yes


## OemActions
    typedef struct _RedfishCertificate_V1_3_0_OemActions_CS {
        RedfishCS_Link Prop;
    } RedfishCertificate_V1_3_0_OemActions_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Rekey
    typedef struct _RedfishCertificate_V1_3_0_Rekey_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishCertificate_V1_3_0_Rekey_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## RekeyResponse
    typedef struct _RedfishCertificate_V1_3_0_RekeyResponse_CS {
        RedfishCS_char *CSRString;
        RedfishCS_Link Certificate;
    } RedfishCertificate_V1_3_0_RekeyResponse_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**CSRString**|RedfishCS_char| String pointer to **CSRString** property.| No| Yes
|**Certificate**|RedfishCS_Link| Structure link list to **Certificate** property.| No| Yes


## Renew
    typedef struct _RedfishCertificate_V1_3_0_Renew_CS {
        RedfishCS_char *target;
        RedfishCS_char *title;
    } RedfishCertificate_V1_3_0_Renew_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**target**|RedfishCS_char| String pointer to **target** property.| No| No
|**title**|RedfishCS_char| String pointer to **title** property.| No| No


## RenewResponse
    typedef struct _RedfishCertificate_V1_3_0_RenewResponse_CS {
        RedfishCS_char *CSRString;
        RedfishCS_Link Certificate;
    } RedfishCertificate_V1_3_0_RenewResponse_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**CSRString**|RedfishCS_char| String pointer to **CSRString** property.| No| Yes
|**Certificate**|RedfishCS_Link| Structure link list to **Certificate** property.| No| Yes


## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## Certificate
    typedef struct _RedfishCertificate_V1_3_0_Certificate_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_context;
        RedfishCS_char *odata_etag;
        RedfishCS_char *odata_id;
        RedfishCS_char *odata_type;
        RedfishCertificate_V1_3_0_Actions_CS *Actions;
        RedfishCS_char *CertificateString;
        RedfishCS_char *CertificateType;
        RedfishCS_char *Description;
        RedfishCS_char *Fingerprint;
        RedfishCS_char *FingerprintHashAlgorithm;
        RedfishCS_char *Id;
        RedfishCertificate_V1_3_0_Identifier_CS *Issuer;
        RedfishCS_char *KeyUsage;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *SerialNumber;
        RedfishCS_char *SignatureAlgorithm;
        RedfishCertificate_V1_3_0_Identifier_CS *Subject;
        RedfishCS_char *UefiSignatureOwner;
        RedfishCS_char *ValidNotAfter;
        RedfishCS_char *ValidNotBefore;
    } RedfishCertificate_V1_3_0_Certificate_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_context**|RedfishCS_char| String pointer to **@odata.context** property.| No| No
|**odata_etag**|RedfishCS_char| String pointer to **@odata.etag** property.| No| No
|**odata_id**|RedfishCS_char| String pointer to **@odata.id** property.| Yes| No
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Actions**|RedfishCertificate_V1_3_0_Actions_CS| Structure points to **Actions** property.| No| No
|**CertificateString**|RedfishCS_char| String pointer to **CertificateString** property.| No| Yes
|**CertificateType**|RedfishCS_char| String pointer to **CertificateType** property.| No| Yes
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Fingerprint**|RedfishCS_char| String pointer to **Fingerprint** property.| No| Yes
|**FingerprintHashAlgorithm**|RedfishCS_char| String pointer to **FingerprintHashAlgorithm** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Issuer**|RedfishCertificate_V1_3_0_Identifier_CS| Structure points to **Issuer** property.| No| No
|**KeyUsage**|RedfishCS_char| String pointer to **KeyUsage** property.| No| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**SerialNumber**|RedfishCS_char| String pointer to **SerialNumber** property.| No| Yes
|**SignatureAlgorithm**|RedfishCS_char| String pointer to **SignatureAlgorithm** property.| No| Yes
|**Subject**|RedfishCertificate_V1_3_0_Identifier_CS| Structure points to **Subject** property.| No| No
|**UefiSignatureOwner**|RedfishCS_char| String pointer to **UefiSignatureOwner** property.| No| Yes
|**ValidNotAfter**|RedfishCS_char| String pointer to **ValidNotAfter** property.| No| Yes
|**ValidNotBefore**|RedfishCS_char| String pointer to **ValidNotBefore** property.| No| Yes
## Redfish Certificate V1_3_0 to C Structure Function
    RedfishCS_status
    Json_Certificate_V1_3_0_To_CS (RedfishCS_char *JsonRawText, RedfishCertificate_V1_3_0_Certificate_CS **ReturnedCS);

## C Structure to Redfish Certificate V1_3_0 JSON Function
    RedfishCS_status
    CS_To_Certificate_V1_3_0_JSON (RedfishCertificate_V1_3_0_Certificate_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish Certificate V1_3_0 C Structure Function
    RedfishCS_status
    DestroyCertificate_V1_3_0_CS (RedfishCertificate_V1_3_0_Certificate_CS *CSPtr);

