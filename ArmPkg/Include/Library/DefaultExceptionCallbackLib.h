/** @file

  SPDX-FileCopyrightText: Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef DEFAULT_EXCEPTION_CALLBACK_LIB_H_
#define DEFAULT_EXCEPTION_CALLBACK_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/DebugSupport.h>

/**
  This is the default action to take on an unexpected exception

  @param  ExceptionType    Type of the exception
  @param  SystemContext    Register state at the time of the Exception

**/
VOID
DefaultExceptionCallback (
  IN     EFI_EXCEPTION_TYPE  ExceptionType,
  IN OUT EFI_SYSTEM_CONTEXT  SystemContext
  );

#endif // DEFAULT_EXCEPTION_HANDLER_LIB_H_
