/** @file
  Google Test mocks for Mm Services Table

  SPDX-FileCopyrightText: Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <GoogleTest/Library/MockMmStTableLib.h>

MOCK_INTERFACE_DEFINITION(MockMmStTableLib);

MOCK_FUNCTION_DEFINITION(MockMmStTableLib, gMmst_MmLocateProtocol, 3, EFIAPI);

static EFI_MM_SYSTEM_TABLE localMmSt = {
  {0},                                // EFI_TABLE_HEADER

  NULL,                              // MmFirmwareVendor
  0,                                 // MmFirmwareRevision
  NULL,                              // MmInstallConfigurationTable
  {                                  // MmIo
    NULL,
    NULL
  },
  NULL,                               // MmAllocatePool
  NULL,                               // MmFreePool
  NULL,                               // MmAllocatePages
  NULL,                               // MmFreePages
  NULL,                               // MmStartupThisAp
  0,                                  // CurrentlyExecutingCpu
  0,                                  // NumberOfCpus
  NULL,                               // CpuSaveStateSize
  NULL,                               // CpuSaveState
  0,                                  // NumberOfTableEntries
  NULL,                               // MmConfigurationTable
  NULL,                               // MmInstallProtocolInterface
  NULL,                               // MmUninstallProtocolInterface
  NULL,                               // MmHandleProtocol
  NULL,                               // MmRegisterProtocolNotify
  NULL,                               // MmLocateHandle
  gMmst_MmLocateProtocol,             // MmLocateProtocol
  NULL,                               // MmiManage
  NULL,                               // MmiHandlerRegister
  NULL,                               // MmiHandlerUnRegister
};

extern "C" {
  EFI_MM_SYSTEM_TABLE* gMmst = &localMmSt;
}
