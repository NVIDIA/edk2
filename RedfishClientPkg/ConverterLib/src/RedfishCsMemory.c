/** @file

  (C) Copyright 2018-2021 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright Notice:
  Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
**/

#include "RedfishCsMemoryInternal.h"
#include <stdlib.h>
#include <string.h>

RedfishCS_Link CsMemRoot = {&CsMemRoot, &CsMemRoot};

/**
  This function records the memory allocation to
  the C Structure instance that owns the memory block.

  memCs          C Structure instance.
  Return RedfishCS_status

**/
RedfishCS_status recordCsRootMemory (void *memCs)
{
  RedfishCS_Internal_memory_root *memRoot;

  memRoot = malloc (sizeof(RedfishCS_Internal_memory_root));
  if (memRoot == NULL) {
    return RedfishCS_status_insufficient_memory;
  }
  InitializeLinkHead (&memRoot->nextRoot);
  InitializeLinkHead (&memRoot->memBlocks);
  memRoot->CsPtr = memCs;
  InsertTailLink (&CsMemRoot, &memRoot->nextRoot);
  return RedfishCS_status_success;
}
/**
  This function allocates and records the memory allocation
  to the C Structure instance that owns the memory block.

  rootCs          C Structure instance.
  size            The size to allocate.
  Dst             Pointer to retrieve the pointer to memory
                  block.
  Return RedfishCS_status

**/
RedfishCS_status allocateRecordCsMemory (RedfishCS_void *rootCs, RedfishCS_uint32 size, RedfishCS_void **Dst)
{
  RedfishCS_Internal_memory_root *memRoot;
  RedfishCS_Internal_memory_link *memLink;

  if (IsLinkEmpty (&CsMemRoot)) {
    return RedfishCS_status_invalid_parameter;
  }
  memRoot = (RedfishCS_Internal_memory_root *)GetFirstLink (&CsMemRoot);
  while (RedfishCS_boolean_true){
    if (memRoot->CsPtr == rootCs) {
      // Allocation memory and record it.
      memLink = malloc (sizeof(RedfishCS_Internal_memory_link));
      if (memLink == NULL) {
        return RedfishCS_status_insufficient_memory;
      }
      *Dst = malloc (size);
      if (*Dst == NULL) {
        free (memLink);
        return RedfishCS_status_insufficient_memory;
      }
      memset (*Dst, 0, size);
      memset (memLink, 0, sizeof(RedfishCS_Internal_memory_link));
      InitializeLinkHead (&memLink->nextLink);
      memLink->memoryPtr = *Dst;
      InsertTailLink (&memRoot->memBlocks, &memLink->nextLink);
      return RedfishCS_status_success;
    }
    if (IsLinkAtEnd (&CsMemRoot, (RedfishCS_Link *)&memRoot->nextRoot)) {
      break;
    }
    memRoot = (RedfishCS_Internal_memory_root *)GetNextLink (&CsMemRoot, &memRoot->nextRoot);
  };

  return RedfishCS_status_invalid_parameter;
}
/**
  This function allocates, records and zero out the memory
  to the C Structure instance that owns the memory block.

  rootCs          C Structure instance.
  size            The size to allocate.
  Dst             Pointer to retrieve the pointer to memory
                  block.
  Return RedfishCS_status

**/
RedfishCS_status allocateRecordCsZeroMemory (RedfishCS_void *rootCs, RedfishCS_uint32 size, RedfishCS_void **Dst)
{
  RedfishCS_status Status;

  Status = allocateRecordCsMemory (rootCs, size, Dst);
  if (Status != RedfishCS_status_success || *Dst == NULL) {
    return Status;
  }
  memset (*Dst, 0, size);
  return RedfishCS_status_success;
}
/**
  This function destroies all memory allocations belong
  to the C Structure instance.

  rootCs          C Structure instance.

  Return RedfishCS_status

**/
RedfishCS_status DestoryCsMemory (RedfishCS_void *rootCs)
{
  RedfishCS_Internal_memory_root *memRoot;
  RedfishCS_Internal_memory_link *memLink;

  if (IsLinkEmpty (&CsMemRoot)) {
    return RedfishCS_status_invalid_parameter;
  }
  memRoot = (RedfishCS_Internal_memory_root *)GetFirstLink (&CsMemRoot);
  while (RedfishCS_boolean_true){
    if (memRoot->CsPtr == rootCs) {
      if (IsLinkEmpty (&memRoot->memBlocks)) {
        return RedfishCS_status_success;
      }
      while (RedfishCS_boolean_true) {
        memLink = (RedfishCS_Internal_memory_link *)GetLastLink(&memRoot->memBlocks);
        if (memLink->memoryPtr != NULL) {
          free (memLink->memoryPtr);
          RemoveLink (&memLink->nextLink);
          free (memLink);
        }
        if (IsLinkEmpty (&memRoot->memBlocks)) {
          RemoveLink (&memRoot->nextRoot);
          free (memRoot);
          free (rootCs);
          return RedfishCS_status_success;
        }
      }
    }
    if (IsLinkAtEnd (&CsMemRoot, (RedfishCS_Link *)&memRoot->nextRoot)) {
      break;
    }
    memRoot = (RedfishCS_Internal_memory_root *)GetNextLink (&CsMemRoot, &memRoot->nextRoot);
  };
  return RedfishCS_status_invalid_parameter;
}

/**
  This function allocates an array of memory blocks owned
  by the C Structure instance.

  rootCs            C Structure instance.
  ArrayInstanceSize Number of items in array.
  ArraySize         The size of each array.
  Dst               Pointer to retrieve the pointer to memory
                    block.

  Return RedfishCS_status

**/
RedfishCS_status allocateArrayRecordCsMemory(RedfishCS_void *rootCs, RedfishCS_uint32 ArrayInstanceSize, RedfishCS_uint64 ArraySize, RedfishCS_void **Dst)
{
  RedfishCS_uint16 Index;
  RedfishCS_void *ArrayInstance;
  RedfishCS_void *PreArrayInstance;
  RedfishCS_status Status;
  RedfishCS_uint16 SizeOfVoid;

  for (Index = 0; Index < ArraySize; Index ++) {
    Status = allocateRecordCsMemory(rootCs, ArrayInstanceSize, &ArrayInstance);
    if (Status != RedfishCS_status_success) {
      return Status;
    }
    memset (ArrayInstance, 0, ArrayInstanceSize);
    if (Index == 0) {
      *Dst = ArrayInstance;
    } else {
      SizeOfVoid = sizeof (RedfishCS_void *);
      if (SizeOfVoid == sizeof (RedfishCS_uint32)) {
        *((RedfishCS_uint32 *)PreArrayInstance) = (RedfishCS_uint32)(unsigned long long)ArrayInstance; // Next link.
      } else if (SizeOfVoid == sizeof (RedfishCS_uint64)){
        *((RedfishCS_uint64 *)PreArrayInstance) = (RedfishCS_uint64)ArrayInstance; // Next link.
      } else {
        return RedfishCS_status_invalid_parameter;
      }
    }
    PreArrayInstance = ArrayInstance;
  }
  return RedfishCS_status_success;
}
