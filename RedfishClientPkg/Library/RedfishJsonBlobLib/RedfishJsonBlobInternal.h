/** @file
  Redfish JSON Blob library internal header file.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_JSON_BLOB_INTERNAL_H_
#define REDFISH_JSON_BLOB_INTERNAL_H_

#include <Uefi.h>

#include <Guid/VariableFormat.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/RedfishJsonBlobLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/RedfishDebugLib.h>

#define REDFISH_JSON_BLOB_DEBUG          DEBUG_VERBOSE
#define REDFISH_JSON_BLOB_SIGNATURE      SIGNATURE_32 ('R', 'F', 'J', 'B')
#define REDFISH_JSON_BLOB_VARIABLE_ATTR  VARIABLE_ATTRIBUTE_NV_BS
///
/// Definition of REDFISH_JSON_BLOB_INFO
///
typedef struct {
  UINT32                            Signature;
  EFI_GUID                          VariableGuid;
  EFI_STRING                        VariableName;
  REDFISH_JSON_BLOB_TO_STRUCTURE    ToStructFunc;
  REDFISH_JSON_BLOB_TO_JSON         ToJsonFunc;
  VOID                              *Context;
  BOOLEAN                           IsEmpty;
} REDFISH_JSON_BLOB_DATA;

#endif
