/*
 This is the wrapper of open source jansson header file for
 using JsonLib library under RedfishPkg.

 (C) Copyright 2021 Hewlett Packard Enterprise Development LP<BR>

    SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef REDFISH_CLIENT_JANSSON_H_
#define REDFISH_CLIENT_JANSSON_H_

#include <Uefi.h>
#include <Library/JsonLib.h>

typedef EDKII_JSON_VALUE  json_t;
typedef EDKII_JSON_INT_T  json_int_t;
typedef EDKII_JSON_TYPE   json_type;

#define JSON_INDENT(n)      EDKII_JSON_INDENT(n)
#define JSON_ENSURE_ASCII   EDKII_JSON_ENSURE_ASCII
#define JSON_OBJECT         EdkiiJsonTypeObject
#define JSON_ARRAY          EdkiiJsonTypeArray
#define JSON_STRING         EdkiiJsonTypeString
#define JSON_INTEGER        EdkiiJsonTypeInteger
#define JSON_REAL           EdkiiJsonTypeReal
#define JSON_TRUE           EdkiiJsonTypeTrue
#define JSON_FALSE          EdkiiJsonTypeFalse
#define JSON_NULL           EdkiiJsonTypeNull

#define json_object_foreach_safe(object, n, key, value) EDKII_JSON_OBJECT_FOREACH_SAFE(object, n, key, value)

//
// Macros that directly map functions to JsonLib
//
#define json_object_get(json_t, key)                JsonObjectGetValue(json_t, key)
#define json_object_size(json_t)                    JsonObjectSize(json_t)
#define json_array_get(json_t, index)               JsonArrayGetValue(json_t, index)
#define json_array_size(json_t)                     JsonArrayCount(json_t)
#define json_string_value(json_t)                   JsonValueGetAsciiString(json_t)
#define json_dumps(json_t, flags)                   JsonDumpString(json_t, flags)
#define json_loads(string, flags, error)            JsonLoadString(string, flags, error)
#define json_loadb(buffer, buflen, flags, error)    JsonLoadBuffer(buffer, buflen, flags, error)
#define json_array_append_new(json_t_array, json_t) JsonArrayAppendValue(json_t_array, json_t)
#define json_object_set_new(json_t, key, value)     JsonObjectSetValue(json_t, key, value)
#define json_decref(json_t)                         JsonDecreaseReference(json_t)
#define json_integer_value(json_t)                  JsonValueGetInteger(json_t)
#define json_is_object(json_t)                      JsonValueIsObject(json_t)
#define json_is_array(json_t)                       JsonValueIsArray(json_t)
#define json_is_string(json_t)                      JsonValueIsString(json_t)
#define json_is_integer(json_t)                     JsonValueIsInteger(json_t)
#define json_is_number(json_t)                      JsonValueIsNumber(json_t)
#define json_is_boolean(json_t)                     JsonValueIsBoolean(json_t)
#define json_is_true(json_t)                        JsonValueIsTrue(json_t)
#define json_is_false(json_t)                       JsonValueIsFalse(json_t)
#define json_is_null(json_t)                        JsonValueIsNull(json_t)
#define json_incref(json_t)                         JsonIncreaseReference(json_t)
#define json_object_set(json_t, key, value)         JsonObjectSetValue(json_t, key, value)
#define json_array_append(json_t, value)            JsonArrayAppendValue(json_t, value)
#define json_object_iter(json_t)                    JsonObjectIterator(json_t)
#define json_object_iter_next(json_t, iter)         JsonObjectIteratorNext(json_t, iter)
#define json_object_iter_value(iter)                JsonObjectIteratorValue(iter)
#define json_get_type(json_t)                       JsonGetType(json_t)

#define json_object()                               JsonValueInitObject()
#define json_array()                                JsonValueInitArray()
#define json_string(string)                         JsonValueInitAsciiString(string)
#define json_integer(value)                         JsonValueInitInteger(value)
#define json_boolean(value)                         JsonValueInitBoolean(value)
#define json_null()                                 JsonValueInitNull()
#endif
