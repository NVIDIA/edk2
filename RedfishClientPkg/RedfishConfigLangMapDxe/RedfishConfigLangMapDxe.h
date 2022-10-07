/** @file
  Common header file for RedfishConfigLangMapDxe driver.

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_CONFIG_LANG_MAP_DXE_H_
#define REDFISH_CONFIG_LANG_MAP_DXE_H_

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
#include <Library/RedfishEventLib.h>
#include <Protocol/EdkIIRedfishConfigLangMapProtocol.h>

#include <Guid/VariableFormat.h>

#define CONFIG_LANG_MAP_VARIABLE_NAME    L"RedfishConfigLangMap"
#define CONFIG_LANG_MAP_DEBUG_ENABLED    0x00

//
// Definition of REDFISH_CONFIG_LANG_MAP_RECORD
//
typedef struct {
  LIST_ENTRY  List;
  EFI_STRING  Uri;
  EFI_STRING  ConfigLang;
  UINTN       Size;
} REDFISH_CONFIG_LANG_MAP_RECORD;

#define REDFISH_CONFIG_LANG_MAP_RECORD_FROM_LIST(a)  BASE_CR (a, REDFISH_CONFIG_LANG_MAP_RECORD, List)

//
// Definition of REDFISH_CONFIG_LANG_MAP_LIST
//
typedef struct {
  LIST_ENTRY    Listheader;
  UINTN         TotalSize;
  UINTN         Count;
} REDFISH_CONFIG_LANG_MAP_LIST;

//
// Definition of REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA
//
typedef struct {
  EFI_HANDLE                             ImageHandle;
  REDFISH_CONFIG_LANG_MAP_LIST           ConfigLangList;
  EDKII_REDFISH_CONFIG_LANG_MAP_PROTOCOL Protocol;
  EFI_STRING                             VariableName;
  EFI_EVENT                              ExitBootEvent;
  EFI_EVENT                              ProvisionEvent;
} REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA;

#define REDFISH_CONFIG_LANG_MAP_PRIVATE_FROM_THIS(a)  BASE_CR (a, REDFISH_CONFIG_LANG_MAP_PRIVATE_DATA, Protocol)

#endif
