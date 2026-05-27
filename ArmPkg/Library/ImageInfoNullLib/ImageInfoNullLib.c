/** @file

  Null instance of ImageInfoLib.

  Returns NULL for every query.  Intended for host-based unit-test builds
  and minimal firmware configurations that link the ImageInfoLib API but
  do not need to resolve runtime addresses to PE/COFF images.

  Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/ImageInfoLib.h>

/**
  Given an address, return the name of the PE/COFF image loaded at that
  address.  Null implementation: always reports "not found".

  Each non-NULL output pointer is initialized to 0, matching the API
  contract documented in ImageInfoLib.h.

  @param[in]  Address              Address to find PE/COFF image for.
  @param[out] ImageBase            Always written to 0 when non-NULL.
  @param[out] PeCoffSizeOfHeaders  Always written to 0 when non-NULL.

  @retval NULL                     Always.

**/
CHAR8 *
EFIAPI
GetImageName (
  IN  UINTN  Address,
  OUT UINTN  *ImageBase,
  OUT UINTN  *PeCoffSizeOfHeaders
  )
{
  if (ImageBase != NULL) {
    *ImageBase = 0;
  }

  if (PeCoffSizeOfHeaders != NULL) {
    *PeCoffSizeOfHeaders = 0;
  }

  return NULL;
}
