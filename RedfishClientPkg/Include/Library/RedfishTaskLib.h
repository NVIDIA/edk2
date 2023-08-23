/** @file
  This file defines the Redfish task library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_TASK_LIB_H_
#define REDFISH_TASK_LIB_H_

#include <Uefi.h>
#include <Library/RedfishLib.h>
#include <Protocol/EdkIIRedfishTaskProtocol.h>

/**

  This function update task result to BMC task service. There is
  no standard way defined in Redfish specification that allows
  BIOS to update task state and status. Platform implement
  this function to update task result to BMC by following
  BMC defined interface.

  @param[in]  RedfishService  Instance to Redfish service.
  @param[in]  TaskUri         The URI of task to update result.
  @param[in]  TaskResult      The task state and task status to update.

  @retval     EFI_SUCCESS         Task state and status is updated to BMC.
  @retval     Others              Errors occur.

**/
EFI_STATUS
RedfishTaskUpdate (
  IN REDFISH_SERVICE      RedfishService,
  IN EFI_STRING           TaskUri,
  IN REDFISH_TASK_RESULT  TaskResult
  );

#endif
