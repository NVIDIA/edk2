/** @file
  Callbacks for set variables

Copyright (c) 2023, NVIDIA Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _VARIABLE_SET_CALLBACKS_H_
#define _VARIABLE_SET_CALLBACKS_H_

#include <Library/VarCheckLib.h>

EFI_STATUS
EFIAPI
VarPreSetCallback (
  IN CHAR16                    *VariableName,
  IN EFI_GUID                  *VendorGuid,
  IN UINT32                    Attributes,
  IN UINTN                     DataSize,
  IN VOID                      *Data,
  IN VAR_CHECK_REQUEST_SOURCE  RequestSource
  );

EFI_STATUS
EFIAPI
VarPostSetCallback (
  IN CHAR16                    *VariableName,
  IN EFI_GUID                  *VendorGuid,
  IN UINT32                    Attributes,
  IN UINTN                     DataSize,
  IN VOID                      *Data,
  IN VAR_CHECK_REQUEST_SOURCE  RequestSource,
  IN EFI_STATUS                SetVarStatus
  );

#endif // _VARIABLE_SET_CALLBACKS_H_
