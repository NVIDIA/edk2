/** @file
  Stub PcdLib for ImageInfoFvLib host-based tests.

  Backs PcdGet64(PcdFvBaseAddress) with a test-controlled UINT64 so the
  fixture can point ImageInfoFvLib at a synthesised in-memory firmware
  volume.  The library under test goes through the PcdLib accessor
  (LibPcdGet64), so the unit test only needs to provide a working
  LibPcdGet64; other PcdLib entry points are stubbed to zero / no-op.

  Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>

//
// Test-controlled backing store.  Set/read via the helpers below.
//
STATIC UINT64  mFvBaseAddress = 0;

VOID
PcdStubSetFvBaseAddress (
  IN UINT64  Address
  )
{
  mFvBaseAddress = Address;
}

UINT64
PcdStubGetFvBaseAddress (
  VOID
  )
{
  return mFvBaseAddress;
}

UINT8
EFIAPI
LibPcdGet8 (
  IN UINTN  TokenNumber
  )
{
  return 0;
}

UINT16
EFIAPI
LibPcdGet16 (
  IN UINTN  TokenNumber
  )
{
  return 0;
}

UINT32
EFIAPI
LibPcdGet32 (
  IN UINTN  TokenNumber
  )
{
  return 0;
}

UINT64
EFIAPI
LibPcdGet64 (
  IN UINTN  TokenNumber
  )
{
  //
  // ImageInfoFvLib only queries PcdFvBaseAddress.  Return the test-
  // controlled value for every UINT64 PCD lookup — the unit under
  // test does not differentiate by token.
  //
  return mFvBaseAddress;
}

VOID *
EFIAPI
LibPcdGetPtr (
  IN UINTN  TokenNumber
  )
{
  return NULL;
}

BOOLEAN
EFIAPI
LibPcdGetBool (
  IN UINTN  TokenNumber
  )
{
  return FALSE;
}

UINTN
EFIAPI
LibPcdGetSize (
  IN UINTN  TokenNumber
  )
{
  return 0;
}
