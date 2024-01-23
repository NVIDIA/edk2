/** @file
  Implementation functions and structures for var check services.

Copyright (c) 2015 - 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/VarSetCallbacksLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
VarPreSetCallback (
  IN CHAR16                    *VariableName,
  IN EFI_GUID                  *VendorGuid,
  IN UINT32                    Attributes,
  IN UINTN                     DataSize,
  IN VOID                      *Data,
  IN VAR_CHECK_REQUEST_SOURCE  RequestSource
  )
{
  return EFI_SUCCESS;
}

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
  )
{
  return EFI_SUCCESS;
}
