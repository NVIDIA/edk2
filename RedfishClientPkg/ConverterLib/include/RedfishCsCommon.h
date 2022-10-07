/** @file

  (C) Copyright 2018-2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright Notice:
  Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
**/
#ifndef REDFISH_CS_COMMON_H_
#define REDFISH_CS_COMMON_H_

#include "RedfishDataTypeDef.h"

RedfishCS_Link * InitializeLinkHead (RedfishCS_Link *LinkHead);
RedfishCS_Link * InsertHeadLink (RedfishCS_Link *ListHead, RedfishCS_Link *Entry);
RedfishCS_Link * InsertTailLink (RedfishCS_Link *ListHead, RedfishCS_Link *Entry);
RedfishCS_Link * GetFirstLink (const RedfishCS_Link *List);
RedfishCS_Link * GetLastLink (const RedfishCS_Link *List);
RedfishCS_Link * GetNextLink (const RedfishCS_Link *List, const RedfishCS_Link *Node);
RedfishCS_Link * GetPreviousLink ( const RedfishCS_Link *List, const RedfishCS_Link *Node);
RedfishCS_Link *RemoveLink ( const RedfishCS_Link *Link);
RedfishCS_bool IsLinkEmpty (const RedfishCS_Link *LinkHead);
RedfishCS_bool IsLinkAtEnd (const RedfishCS_Link *LinkHead, const RedfishCS_Link *ThisLink);
RedfishCS_status recordCsRootMemory (void *memCs);
RedfishCS_status allocateRecordCsMemory (RedfishCS_void *rootCs, RedfishCS_uint32 size, RedfishCS_void **Dst);
RedfishCS_status allocateRecordCsZeroMemory (RedfishCS_void *rootCs, RedfishCS_uint32 size, RedfishCS_void **Dst);
RedfishCS_status allocateArrayRecordCsMemory(RedfishCS_void *rootCs, RedfishCS_uint32 ArrayInstanceSize, RedfishCS_uint64 ArraySize, RedfishCS_void **Dst);
RedfishCS_status allocateDuplicateStr (void *Cs, char *Str, void **DstBuffer);
RedfishCS_status DestoryCsMemory (RedfishCS_void *rootCs);

typedef struct _RedfishCS_char_Array RedfishCS_char_Array;
typedef struct _RedfishCS_int64_Array RedfishCS_int64_Array;
typedef struct _RedfishCS_bool_Array RedfishCS_bool_Array;
typedef struct _RedfishCS_Link_Array RedfishCS_Link_Array;
typedef struct _RedfishCS_EmptyProp_KeyValue RedfishCS_EmptyProp_KeyValue;

typedef enum {
  RedfishCS_Type_CS = 1,
  RedfishCS_Type_CS_EmptyProp,
  RedfishCS_Type_JSON,
  RedfishCS_Type_Uri
} RedfishCS_Type;

typedef struct _RedfishCS_Header {
  RedfishCS_Link     LinkEntry;
  RedfishCS_Type     ResourceType;
  RedfishCS_char     *KeyName;
  RedfishCS_char     *ThisUri;
} RedfishCS_Header;

typedef struct _RedfishCS_Type_Uri_Data {
  RedfishCS_Header  Header;
  RedfishCS_char    *Uri;
} RedfishCS_Type_Uri_Data;

typedef struct _RedfishCS_Type_CS_Data {
  RedfishCS_Header  Header;
  //
  // Followed by C structure of resource.
  //
} RedfishCS_Type_CS_Data;

typedef struct _RedfishCS_Type_JSON_Data {
  RedfishCS_Header  Header;
  RedfishCS_char  *JsonText;
} RedfishCS_Type_JSON_Data;

typedef struct _RedfishCS_Number {
  RedfishCS_uint16 Value;
  RedfishCS_uint16 MaxValue;
  RedfishCS_uint16 MinValue;
} RedfishCS_Number;

typedef struct _RedfishCS_char_Array  {
  RedfishCS_char_Array *Next;
  RedfishCS_char *ArrayValue;
} RedfishCS_char_Array;

typedef struct _RedfishCS_int64_Array  {
  RedfishCS_int64_Array *Next;
  RedfishCS_int64 *ArrayValue;
} RedfishCS_int64_Array;

typedef struct _RedfishCS_bool_Array  {
  RedfishCS_bool_Array *Next;
  RedfishCS_bool *ArrayValue;
} RedfishCS_bool_Array;

typedef struct _RedfishCS_Link_Array  {
  RedfishCS_Link_Array *Next;
  RedfishCS_Link *ArrayValue;
} RedfishCS_Link_Array;

typedef enum {
  RedfishCS_Vague_DataType_String = 1,
  RedfishCS_Vague_DataType_Int64,
  RedfishCS_Vague_DataType_Bool
} RedfishCS_Vague_DataType;

typedef union {
  RedfishCS_char  *CharPtr;
  RedfishCS_bool  *BoolPtr;
  RedfishCS_int64 *Int64Ptr;
} RedfishCS_Vague_Ptr;

typedef struct _RedfishCS_Vague  {
  RedfishCS_Vague_DataType DataType;
  RedfishCS_Vague_Ptr      DataValue;
} RedfishCS_Vague;

typedef struct _RedfishCS_EmptyProp_KeyValue  {
  RedfishCS_EmptyProp_KeyValue *NextKeyValuePtr;
  RedfishCS_char  *KeyNamePtr;
  RedfishCS_Vague *Value;
} RedfishCS_EmptyProp_KeyValue;

typedef struct _RedfishCS_Type_EmptyProp_CS_Data {
  RedfishCS_Header Header;
  RedfishCS_uint32 NunmOfProperties;
  RedfishCS_EmptyProp_KeyValue *KeyValuePtr;
} RedfishCS_Type_EmptyProp_CS_Data;

#endif
