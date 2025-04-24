/** @file
  Stub PeCoffGetEntryPointLib for ImageInfoLib host-based tests.

  Returns fixed values so GetImageName() tests can verify output-argument
  initialization and NULL-pointer handling without requiring real PE/COFF
  image data in memory.

  Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>

UINTN
PeCoffGetSizeOfHeaders (
  VOID  *ImageAddress
  )
{
  return 0x200;
}

CHAR8 *
PeCoffLoaderGetPdbPointer (
  VOID  *Pe32Data
  )
{
  return (CHAR8 *)"TestImage.efi";
}
