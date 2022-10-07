/** @file
  Common header file for RedfishETagDxe driver.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_ETAG_DXE_H_
#define REDFISH_ETAG_DXE_H_

#include <Uefi.h>
#include <RedfishBase.h>

//
// Libraries
//
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/EdkIIRedfishETagProtocol.h>

#include <Guid/VariableFormat.h>

#define ETAG_VARIABLE_NAME    L"RedfishETag"
#define ETAG_DEBUG_ENABLED    0x00

//
// Definition of REDFISH_ETAG_RECORD
//
typedef struct {
  LIST_ENTRY  List;
  CHAR8       *Uri;
  CHAR8       *ETag;
  UINTN       Size;
} REDFISH_ETAG_RECORD;

#define REDFISH_ETAG_RECORD_FROM_LIST(a)  BASE_CR (a, REDFISH_ETAG_RECORD, List)

//
// Definition of REDFISH_ETAG_LIST
//
typedef struct {
  LIST_ENTRY    Listheader;
  UINTN         TotalSize;
  UINTN         Count;
} REDFISH_ETAG_LIST;

//
// Definition of REDFISH_ETAG_PRIVATE_DATA
//
typedef struct {
  EFI_HANDLE                  ImageHandle;
  REDFISH_ETAG_LIST           ETagList;
  EDKII_REDFISH_ETAG_PROTOCOL Protocol;
  EFI_STRING                  VariableName;
  EFI_EVENT                   Event;
} REDFISH_ETAG_PRIVATE_DATA;

#define REDFISH_ETAG_PRIVATE_FROM_THIS(a)  BASE_CR (a, REDFISH_ETAG_PRIVATE_DATA, Protocol)

#endif
