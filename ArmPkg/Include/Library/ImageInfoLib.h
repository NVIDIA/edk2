/** @file

  ImageInfo library

  Utilities to query images already loaded in memory.  Primarily for debugging
  purposes.

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __IMAGE_INFO_LIB_H__
#define __IMAGE_INFO_LIB_H__

#include <Uefi/UefiBaseType.h>

/**
  Given an address, return the name of the PE/COFF image loaded at that
  address.

  Both output arguments are optional.  Each non-NULL pointer is always
  written: set to 0 when no image is found, or to the real value when one
  is found.

  @param[in]  Address              Address to find PE/COFF image for.
  @param[out] ImageBase            Load address of the found image, or 0.
                                   May be NULL.
  @param[out] PeCoffSizeOfHeaders  Size of the PE/COFF header for the found
                                   image, or 0.  May be NULL.

  @retval NULL                     No loaded PE/COFF image contains Address.
  @retval Other                    Path and file name of the PE/COFF image.

**/
CHAR8 *
EFIAPI
GetImageName (
  IN  UINTN  Address,
  OUT UINTN  *ImageBase,
  OUT UINTN  *PeCoffSizeOfHeaders
  );

#endif
