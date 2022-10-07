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

#ifndef RedfishAttributeregistry_V1_1_10_CSTRUCT_H_
#define RedfishAttributeregistry_V1_1_10_CSTRUCT_H_

#include "RedfishCsCommon.h"

typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Menus_Array_CS RedfishAttributeRegistry_V1_1_10_Menus_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Actions_CS RedfishAttributeRegistry_V1_1_10_Actions_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeValue_CS RedfishAttributeRegistry_V1_1_10_AttributeValue_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Attributes_CS RedfishAttributeRegistry_V1_1_10_Attributes_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Dependencies_CS RedfishAttributeRegistry_V1_1_10_Dependencies_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Dependency_CS RedfishAttributeRegistry_V1_1_10_Dependency_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_MapFrom_CS RedfishAttributeRegistry_V1_1_10_MapFrom_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_Menus_CS RedfishAttributeRegistry_V1_1_10_Menus_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_OemActions_CS RedfishAttributeRegistry_V1_1_10_OemActions_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_RegistryEntries_CS RedfishAttributeRegistry_V1_1_10_RegistryEntries_CS;
typedef struct _RedfishAttributeRegistry_V1_1_10_SupportedSystems_CS RedfishAttributeRegistry_V1_1_10_SupportedSystems_CS;
#ifndef RedfishResource_Oem_CS_
typedef struct _RedfishResource_Oem_CS RedfishResource_Oem_CS;
#endif

//
// The available actions for this resource.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_Actions_CS {
    RedfishAttributeRegistry_V1_1_10_OemActions_CS    *Oem;    // The available OEM-specific
                                                              // actions for this resource.
} RedfishAttributeRegistry_V1_1_10_Actions_CS;

//
// A possible value for an enumeration attribute.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeValue_CS {
    RedfishCS_char    *ValueDisplayName;    // A user-readable display string
                                           // of the value for the attribute
                                           // in the defined language.
    RedfishCS_char    *ValueName;          // The unique value name for the
                                           // attribute.
} RedfishAttributeRegistry_V1_1_10_AttributeValue_CS;

//
// An attribute and its possible values and other metadata.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_Attributes_CS {
    RedfishCS_char                                              *AttributeName;            // The unique name for the
                                                                                           // attribute.
    RedfishCS_Vague                                             *CurrentValue;             // The placeholder of the current
                                                                                           // value for the attribute.
    RedfishCS_Vague                                             *DefaultValue;             // The default value for the
                                                                                           // attribute.
    RedfishCS_char                                              *DisplayName;              // The user-readable display
                                                                                           // string for the attribute in
                                                                                           // the defined language.
    RedfishCS_int64                                             *DisplayOrder;             // The ascending order, as a
                                                                                           // number, in which this
                                                                                           // attribute appears relative to
                                                                                           // other attributes.
    RedfishCS_bool                                              *GrayOut;                  // An indication of whether this
                                                                                           // attribute is grayed out.  A
                                                                                           // grayed-out attribute is not
                                                                                           // active and is grayed out in
                                                                                           // user interfaces but the
                                                                                           // attribute value can be
                                                                                           // modified.
    RedfishCS_char                                              *HelpText;                 // The help text for the
                                                                                           // attribute.
    RedfishCS_bool                                              *Hidden;                   // An indication of whether this
                                                                                           // attribute is hidden in user
                                                                                           // interfaces.
    RedfishCS_bool                                              *Immutable;                // An indication of whether this
                                                                                           // attribute is immutable.
                                                                                           // Immutable attributes shall not
                                                                                           // be modified and typically
                                                                                           // reflect a hardware state.
    RedfishCS_bool                                              *IsSystemUniqueProperty;    // An indication of whether this
                                                                                           // attribute is unique for this
                                                                                           // system and should not be
                                                                                           // replicated.
    RedfishCS_int64                                             *LowerBound;               // The lower limit for an integer
                                                                                           // attribute.
    RedfishCS_int64                                             *MaxLength;                // The maximum character length
                                                                                           // of a string attribute.
    RedfishCS_char                                              *MenuPath;                 // The path that describes the
                                                                                           // menu hierarchy of this
                                                                                           // attribute.
    RedfishCS_int64                                             *MinLength;                // The minimum character length
                                                                                           // of the string attribute.
    RedfishCS_bool                                              *ReadOnly;                 // An indication of whether this
                                                                                           // attribute is read-only.  A
                                                                                           // read-only attribute cannot be
                                                                                           // modified, and should be grayed
                                                                                           // out in user interfaces.
    RedfishCS_int64                                             *ScalarIncrement;          // The amount to increment or
                                                                                           // decrement an integer attribute
                                                                                           // each time a user requests a
                                                                                           // value change.  The `0` value
                                                                                           // indicates a free-form numeric
                                                                                           // user-input attribute.
    RedfishCS_char                                              *Type;                     // The attribute type.
    RedfishCS_int64                                             *UpperBound;               // The upper limit for an integer
                                                                                           // attribute.
    RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS    *Value;                    // An array of the possible
                                                                                           // values for enumerated
                                                                                           // attribute values.
    RedfishCS_char                                              *ValueExpression;          // A valid regular expression,
                                                                                           // according to the Perl regular
                                                                                           // expression dialect, that
                                                                                           // validates the attribute value.
                                                                                           // Applies to only string and
                                                                                           // integer attributes.
    RedfishCS_char                                              *WarningText;              // The warning text for the
                                                                                           // attribute.
    RedfishCS_bool                                              *WriteOnly;                // An indication of whether this
                                                                                           // attribute is write-only.  A
                                                                                           // write-only attribute reverts
                                                                                           // to its initial value after
                                                                                           // settings are applied.
} RedfishAttributeRegistry_V1_1_10_Attributes_CS;

//
// The dependency of attributes on this component.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_Dependencies_CS {
    RedfishAttributeRegistry_V1_1_10_Dependency_CS    *Dependency;      // The dependency expression for
                                                                        // one or more attributes in this
                                                                        // attribute registry.
    RedfishCS_char                                    *DependencyFor;    // The AttributeName of the
                                                                        // attribute whose change
                                                                        // triggers the evaluation of
                                                                        // this dependency expression.
    RedfishCS_char                                    *Type;            // The type of the dependency
                                                                        // structure.
} RedfishAttributeRegistry_V1_1_10_Dependencies_CS;

//
// The dependency expression for one or more attributes in this attribute
// registry.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_Dependency_CS {
    RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS    *MapFrom;          // An array of the map-from
                                                                            // conditions for a mapping
                                                                            // dependency.
    RedfishCS_char                                       *MapToAttribute;    // The AttributeName of the
                                                                            // attribute that is affected by
                                                                            // this dependency expression.
    RedfishCS_char                                       *MapToProperty;    // The metadata property for the
                                                                            // attribute that contains the
                                                                            // map-from condition that
                                                                            // evaluates this dependency
                                                                            // expression.
    RedfishCS_Vague                                      *MapToValue;       // The value that the map-to
                                                                            // property changes to if the
                                                                            // dependency expression
                                                                            // evaluates to `true`.
} RedfishAttributeRegistry_V1_1_10_Dependency_CS;

//
// A map-from condition for mapping dependency.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_MapFrom_CS {
    RedfishCS_char     *MapFromAttribute;    // The attribute to use to
                                            // evaluate this dependency
                                            // expression.
    RedfishCS_char     *MapFromCondition;    // The condition to use to
                                            // evaluate this dependency
                                            // expression.
    RedfishCS_char     *MapFromProperty;    // The metadata property for the
                                            // attribute that the
                                            // MapFromAttribute property
                                            // specifies to use to evaluate
                                            // this dependency expression.
    RedfishCS_Vague    *MapFromValue;       // The value to use to evaluate
                                            // this dependency expression.
    RedfishCS_char     *MapTerms;           // The logical term that combines
                                            // two or more map-from
                                            // conditions in this dependency
                                            // expression.  For example,
                                            // `AND` for logical AND, or `OR`
                                            // for logical OR.
} RedfishAttributeRegistry_V1_1_10_MapFrom_CS;

//
// An attribute's menu and its hierarchy.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_Menus_CS {
    RedfishCS_char     *DisplayName;    // The user-readable display
                                        // string of this menu in the
                                        // defined language.
    RedfishCS_int64    *DisplayOrder;    // The ascending order, as a
                                        // number, in which this menu
                                        // appears relative to other
                                        // menus.
    RedfishCS_bool     *GrayOut;        // An indication of whether this
                                        // menu is grayed out.  A grayed-
                                        // only menu is not accessible in
                                        // user interfaces.
    RedfishCS_char     *MenuName;       // The unique name string of this
                                        // menu.
    RedfishCS_char     *MenuPath;       // The path to the menu names
                                        // that describes this menu
                                        // hierarchy relative to other
                                        // menus.
    RedfishCS_bool     *ReadOnly;       // An indication of whether this
                                        // menu is read-only.  A read-
                                        // only menu, its properties, and
                                        // sub-menus are not accessible
                                        // in user interfaces.
} RedfishAttributeRegistry_V1_1_10_Menus_CS;

//
// The available OEM-specific actions for this resource.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_OemActions_CS {
    RedfishCS_Link    Prop;
} RedfishAttributeRegistry_V1_1_10_OemActions_CS;

//
// The list of all attributes and metadata for this component.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_RegistryEntries_CS {
    RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS      *Attributes;     // An array of attributes and
                                                                               // their possible values in the
                                                                               // attribute registry.
    RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS    *Dependencies;    // An array of dependencies of
                                                                               // attributes on this component.
    RedfishAttributeRegistry_V1_1_10_Menus_Array_CS           *Menus;          // An array for the attributes
                                                                               // menus and their hierarchy in
                                                                               // the attribute registry.
} RedfishAttributeRegistry_V1_1_10_RegistryEntries_CS;

//
// A system that this attribute registry supports.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_SupportedSystems_CS {
    RedfishCS_char    *FirmwareVersion;    // Firmware version.
    RedfishCS_char    *ProductName;       // The product name of the
                                          // computer system to which this
                                          // attribute registry applies.
    RedfishCS_char    *SystemId;          // The ID of the systems to which
                                          // this attribute registry
                                          // applies.
} RedfishAttributeRegistry_V1_1_10_SupportedSystems_CS;

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
// The AttributeRegistry schema contains a set of key-value pairs that represent
// the structure of an attribute registry.  It includes mechanisms for building
// user interfaces, or menus, allowing consistent navigation of the contents.  The
// attribute registry is specific to an implementation or product.  The attributes
// and property names are not standardized.
//
typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS {
    RedfishCS_Header                                              Header;
    RedfishCS_char                                                *odata_type;     
    RedfishAttributeRegistry_V1_1_10_Actions_CS                   *Actions;            // The available actions for this
                                                                                       // resource.
    RedfishCS_char                                                *Description;    
    RedfishCS_char                                                *Id;             
    RedfishCS_char                                                *Language;           // The RFC5646-conformant
                                                                                       // language code for the
                                                                                       // attribute registry.
    RedfishCS_char                                                *Name;           
    RedfishResource_Oem_CS                                        *Oem;                // The OEM extension property.
    RedfishCS_char                                                *OwningEntity;       // The organization or company
                                                                                       // that publishes this attribute
                                                                                       // registry.
    RedfishAttributeRegistry_V1_1_10_RegistryEntries_CS           *RegistryEntries;    // The list of all attributes and
                                                                                       // their metadata for this
                                                                                       // component.
    RedfishCS_char                                                *RegistryVersion;    // The attribute registry
                                                                                       // version.
    RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS    *SupportedSystems;    // An array of systems that this
                                                                                       // attribute registry supports.
} RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_AttributeValue_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_AttributeValue_Array_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_MapFrom_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_MapFrom_Array_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_Attributes_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_Attributes_Array_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_Dependencies_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_Dependencies_Array_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_Menus_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_Menus_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_Menus_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_Menus_Array_CS;

typedef struct _RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS  {
    RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS    *Next;
    RedfishAttributeRegistry_V1_1_10_SupportedSystems_CS    *ArrayValue;
} RedfishAttributeRegistry_V1_1_10_SupportedSystems_Array_CS;

RedfishCS_status
Json_AttributeRegistry_V1_1_10_To_CS (char *JsonRawText, RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS **ReturnedCS);

RedfishCS_status
CS_To_AttributeRegistry_V1_1_10_JSON (RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS *CSPtr, char **JsonText);

RedfishCS_status
DestroyAttributeRegistry_V1_1_10_CS (RedfishAttributeRegistry_V1_1_10_AttributeRegistry_CS *CSPtr);

RedfishCS_status
DestroyAttributeRegistry_V1_1_10_Json (RedfishCS_char *JsonText);

#endif
