/** @file

  (C) Copyright 2018-2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright Notice:
  Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
**/
#ifndef REDFISH_CS_MEMORY_INTERNAL_H_
#define REDFISH_CS_MEMORY_INTERNAL_H_

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

typedef struct _RedfishCS_Internal_memory_link {
  RedfishCS_Link nextLink;
  void           *memoryPtr;
} RedfishCS_Internal_memory_link;

typedef struct _RedfishCS_Internal_memory_root {
  RedfishCS_Link  nextRoot;
  RedfishCS_Link  memBlocks;
  void            *CsPtr;
} RedfishCS_Internal_memory_root;

#endif


