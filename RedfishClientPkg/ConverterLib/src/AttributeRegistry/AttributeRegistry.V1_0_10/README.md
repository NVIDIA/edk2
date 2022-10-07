# Definition of AttributeRegistry.V1_0_10 and functions<br><br>

## AttributeValue
    typedef struct _RedfishAttributeRegistry_V1_0_10_AttributeValue_CS {
        RedfishCS_char *ValueDisplayName;
        RedfishCS_char *ValueName;
    } RedfishAttributeRegistry_V1_0_10_AttributeValue_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ValueDisplayName**|RedfishCS_char| String pointer to **ValueDisplayName** property.| No| Yes
|**ValueName**|RedfishCS_char| String pointer to **ValueName** property.| No| Yes


## Attributes
    typedef struct _RedfishAttributeRegistry_V1_0_10_Attributes_CS {
        RedfishCS_char *AttributeName;
        RedfishCS_Vague *CurrentValue;
        RedfishCS_Vague *DefaultValue;
        RedfishCS_char *DisplayName;
        RedfishCS_int64 *DisplayOrder;
        RedfishCS_bool *GrayOut;
        RedfishCS_char *HelpText;
        RedfishCS_bool *Hidden;
        RedfishCS_bool *Immutable;
        RedfishCS_bool *IsSystemUniqueProperty;
        RedfishCS_int64 *LowerBound;
        RedfishCS_int64 *MaxLength;
        RedfishCS_char *MenuPath;
        RedfishCS_int64 *MinLength;
        RedfishCS_bool *ReadOnly;
        RedfishCS_int64 *ScalarIncrement;
        RedfishCS_char *Type;
        RedfishCS_int64 *UpperBound;
        RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS *Value;
        RedfishCS_char *ValueExpression;
        RedfishCS_char *WarningText;
        RedfishCS_bool *WriteOnly;
    } RedfishAttributeRegistry_V1_0_10_Attributes_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**AttributeName**|RedfishCS_char| String pointer to **AttributeName** property.| No| Yes
|**CurrentValue**|RedfishCS_Vague| RedfishCS_Vague structure to **CurrentValue** property.| No| Yes
|**DefaultValue**|RedfishCS_Vague| RedfishCS_Vague structure to **DefaultValue** property.| No| Yes
|**DisplayName**|RedfishCS_char| String pointer to **DisplayName** property.| No| Yes
|**DisplayOrder**|RedfishCS_int64| 64-bit long long interger pointer to **DisplayOrder** property.| No| Yes
|**GrayOut**|RedfishCS_bool| Boolean pointer to **GrayOut** property.| No| Yes
|**HelpText**|RedfishCS_char| String pointer to **HelpText** property.| No| Yes
|**Hidden**|RedfishCS_bool| Boolean pointer to **Hidden** property.| No| Yes
|**Immutable**|RedfishCS_bool| Boolean pointer to **Immutable** property.| No| Yes
|**IsSystemUniqueProperty**|RedfishCS_bool| Boolean pointer to **IsSystemUniqueProperty** property.| No| Yes
|**LowerBound**|RedfishCS_int64| 64-bit long long interger pointer to **LowerBound** property.| No| Yes
|**MaxLength**|RedfishCS_int64| 64-bit long long interger pointer to **MaxLength** property.| No| Yes
|**MenuPath**|RedfishCS_char| String pointer to **MenuPath** property.| No| Yes
|**MinLength**|RedfishCS_int64| 64-bit long long interger pointer to **MinLength** property.| No| Yes
|**ReadOnly**|RedfishCS_bool| Boolean pointer to **ReadOnly** property.| No| Yes
|**ScalarIncrement**|RedfishCS_int64| 64-bit long long interger pointer to **ScalarIncrement** property.| No| Yes
|**Type**|RedfishCS_char| String pointer to **Type** property.| No| Yes
|**UpperBound**|RedfishCS_int64| 64-bit long long interger pointer to **UpperBound** property.| No| Yes
|**Value**|RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS| Structure array points to one or more than one **RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS** structures for property **Value**.| No| No
|**ValueExpression**|RedfishCS_char| String pointer to **ValueExpression** property.| No| Yes
|**WarningText**|RedfishCS_char| String pointer to **WarningText** property.| No| Yes
|**WriteOnly**|RedfishCS_bool| Boolean pointer to **WriteOnly** property.| No| Yes


## Dependencies
    typedef struct _RedfishAttributeRegistry_V1_0_10_Dependencies_CS {
        RedfishAttributeRegistry_V1_0_10_Dependency_CS *Dependency;
        RedfishCS_char *DependencyFor;
        RedfishCS_char *Type;
    } RedfishAttributeRegistry_V1_0_10_Dependencies_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Dependency**|RedfishAttributeRegistry_V1_0_10_Dependency_CS| Structure points to **Dependency** property.| No| No
|**DependencyFor**|RedfishCS_char| String pointer to **DependencyFor** property.| No| Yes
|**Type**|RedfishCS_char| String pointer to **Type** property.| No| Yes


## Dependency
    typedef struct _RedfishAttributeRegistry_V1_0_10_Dependency_CS {
        RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS *MapFrom;
        RedfishCS_char *MapToAttribute;
        RedfishCS_char *MapToProperty;
        RedfishCS_Vague *MapToValue;
    } RedfishAttributeRegistry_V1_0_10_Dependency_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**MapFrom**|RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS| Structure array points to one or more than one **RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS** structures for property **MapFrom**.| No| No
|**MapToAttribute**|RedfishCS_char| String pointer to **MapToAttribute** property.| No| Yes
|**MapToProperty**|RedfishCS_char| String pointer to **MapToProperty** property.| No| Yes
|**MapToValue**|RedfishCS_Vague| RedfishCS_Vague structure to **MapToValue** property.| No| Yes


## MapFrom
    typedef struct _RedfishAttributeRegistry_V1_0_10_MapFrom_CS {
        RedfishCS_char *MapFromAttribute;
        RedfishCS_char *MapFromCondition;
        RedfishCS_char *MapFromProperty;
        RedfishCS_Vague *MapFromValue;
        RedfishCS_char *MapTerms;
    } RedfishAttributeRegistry_V1_0_10_MapFrom_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**MapFromAttribute**|RedfishCS_char| String pointer to **MapFromAttribute** property.| No| Yes
|**MapFromCondition**|RedfishCS_char| String pointer to **MapFromCondition** property.| No| Yes
|**MapFromProperty**|RedfishCS_char| String pointer to **MapFromProperty** property.| No| Yes
|**MapFromValue**|RedfishCS_Vague| RedfishCS_Vague structure to **MapFromValue** property.| No| Yes
|**MapTerms**|RedfishCS_char| String pointer to **MapTerms** property.| No| Yes


## Menus
    typedef struct _RedfishAttributeRegistry_V1_0_10_Menus_CS {
        RedfishCS_char *DisplayName;
        RedfishCS_int64 *DisplayOrder;
        RedfishCS_bool *GrayOut;
        RedfishCS_char *MenuName;
        RedfishCS_char *MenuPath;
        RedfishCS_bool *ReadOnly;
    } RedfishAttributeRegistry_V1_0_10_Menus_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**DisplayName**|RedfishCS_char| String pointer to **DisplayName** property.| No| Yes
|**DisplayOrder**|RedfishCS_int64| 64-bit long long interger pointer to **DisplayOrder** property.| No| Yes
|**GrayOut**|RedfishCS_bool| Boolean pointer to **GrayOut** property.| No| Yes
|**MenuName**|RedfishCS_char| String pointer to **MenuName** property.| No| Yes
|**MenuPath**|RedfishCS_char| String pointer to **MenuPath** property.| No| Yes
|**ReadOnly**|RedfishCS_bool| Boolean pointer to **ReadOnly** property.| No| Yes


## RegistryEntries
    typedef struct _RedfishAttributeRegistry_V1_0_10_RegistryEntries_CS {
        RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS *Attributes;
        RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS *Dependencies;
        RedfishAttributeRegistry_V1_0_10_Menus_Array_CS *Menus;
    } RedfishAttributeRegistry_V1_0_10_RegistryEntries_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Attributes**|RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS| Structure array points to one or more than one **RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS** structures for property **Attributes**.| No| No
|**Dependencies**|RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS| Structure array points to one or more than one **RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS** structures for property **Dependencies**.| No| No
|**Menus**|RedfishAttributeRegistry_V1_0_10_Menus_Array_CS| Structure array points to one or more than one **RedfishAttributeRegistry_V1_0_10_Menus_Array_CS** structures for property **Menus**.| No| No


## SupportedSystems
    typedef struct _RedfishAttributeRegistry_V1_0_10_SupportedSystems_CS {
        RedfishCS_char *ProductName;
        RedfishCS_char *SystemId;
    } RedfishAttributeRegistry_V1_0_10_SupportedSystems_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**ProductName**|RedfishCS_char| String pointer to **ProductName** property.| No| Yes
|**SystemId**|RedfishCS_char| String pointer to **SystemId** property.| No| Yes


## Oem
    typedef struct _RedfishResource_Oem_CS {
        RedfishCS_Link Prop;
    } RedfishResource_Oem_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Prop**|RedfishCS_Link| Structure link list to OEM defined property| ---| ---


## RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS
    typedef struct _RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS  {
        RedfishCS_Link *Next;
        RedfishAttributeRegistry_V1_0_10_AttributeValue_CS *ArrayValue;
    } RedfishAttributeRegistry_V1_0_10_AttributeValue_Array_CS;



## RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS
    typedef struct _RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS  {
        RedfishCS_Link *Next;
        RedfishAttributeRegistry_V1_0_10_MapFrom_CS *ArrayValue;
    } RedfishAttributeRegistry_V1_0_10_MapFrom_Array_CS;



## RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS
    typedef struct _RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS  {
        RedfishCS_Link *Next;
        RedfishAttributeRegistry_V1_0_10_Attributes_CS *ArrayValue;
    } RedfishAttributeRegistry_V1_0_10_Attributes_Array_CS;



## RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS
    typedef struct _RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS  {
        RedfishCS_Link *Next;
        RedfishAttributeRegistry_V1_0_10_Dependencies_CS *ArrayValue;
    } RedfishAttributeRegistry_V1_0_10_Dependencies_Array_CS;



## RedfishAttributeRegistry_V1_0_10_Menus_Array_CS
    typedef struct _RedfishAttributeRegistry_V1_0_10_Menus_Array_CS  {
        RedfishCS_Link *Next;
        RedfishAttributeRegistry_V1_0_10_Menus_CS *ArrayValue;
    } RedfishAttributeRegistry_V1_0_10_Menus_Array_CS;



## AttributeRegistry
    typedef struct _RedfishAttributeRegistry_V1_0_10_AttributeRegistry_CS {
        RedfishCS_Header     Header;
        RedfishCS_char *odata_type;
        RedfishCS_char *Description;
        RedfishCS_char *Id;
        RedfishCS_char *Language;
        RedfishCS_char *Name;
        RedfishResource_Oem_CS *Oem;
        RedfishCS_char *OwningEntity;
        RedfishAttributeRegistry_V1_0_10_RegistryEntries_CS *RegistryEntries;
        RedfishCS_char *RegistryVersion;
        RedfishAttributeRegistry_V1_0_10_SupportedSystems_CS *SupportedSystems;
    } RedfishAttributeRegistry_V1_0_10_AttributeRegistry_CS;

|Field |C Structure Data Type|Description |Required Property|Read only Property
| ---  | --- | --- | --- | ---
|**Header**|RedfishCS_Header|Redfish C structure header|---|---
|**odata_type**|RedfishCS_char| String pointer to **@odata.type** property.| Yes| No
|**Description**|RedfishCS_char| String pointer to **Description** property.| No| Yes
|**Id**|RedfishCS_char| String pointer to **Id** property.| Yes| Yes
|**Language**|RedfishCS_char| String pointer to **Language** property.| Yes| Yes
|**Name**|RedfishCS_char| String pointer to **Name** property.| Yes| Yes
|**Oem**|RedfishResource_Oem_CS| Structure points to **Oem** property.| No| No
|**OwningEntity**|RedfishCS_char| String pointer to **OwningEntity** property.| Yes| Yes
|**RegistryEntries**|RedfishAttributeRegistry_V1_0_10_RegistryEntries_CS| Structure points to **RegistryEntries** property.| No| No
|**RegistryVersion**|RedfishCS_char| String pointer to **RegistryVersion** property.| Yes| Yes
|**SupportedSystems**|RedfishAttributeRegistry_V1_0_10_SupportedSystems_CS| Structure points to **SupportedSystems** property.| No| No
## Redfish AttributeRegistry V1_0_10 to C Structure Function
    RedfishCS_status
    Json_AttributeRegistry_V1_0_10_To_CS (RedfishCS_char *JsonRawText, RedfishAttributeRegistry_V1_0_10_AttributeRegistry_CS **ReturnedCS);

## C Structure to Redfish AttributeRegistry V1_0_10 JSON Function
    RedfishCS_status
    CS_To_AttributeRegistry_V1_0_10_JSON (RedfishAttributeRegistry_V1_0_10_AttributeRegistry_CS *CSPtr, RedfishCS_char **JsonText);

## Destory Redfish AttributeRegistry V1_0_10 C Structure Function
    RedfishCS_status
    DestroyAttributeRegistry_V1_0_10_CS (RedfishAttributeRegistry_V1_0_10_AttributeRegistry_CS *CSPtr);

