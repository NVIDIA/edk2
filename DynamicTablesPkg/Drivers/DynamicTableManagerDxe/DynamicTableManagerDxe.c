/** @file
  Dynamic Table Manager Dxe

  Copyright (c) 2017 - 2019, ARM Limited. All rights reserved.
  Copyright (c) 2022 - 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <DeviceTreeTableGenerator.h>
#include <Library/TableHelperLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include <Protocol/DynamicTableFactoryProtocol.h>

STATIC VOID  *AcpiTableProtocolRegistration;
STATIC VOID  *SmbiosProtocolRegistration;

extern
VOID
SmbiosProtocolReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  );

extern
VOID
AcpiTableProtocolReady (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  );

/** A helper function to build and install an SMBIOS table.

  This is a helper function that invokes the Table generator interface
  for building an SMBIOS table. It uses the SmbiosProtocol to install the
  table, then frees the resources allocated for generating it.

  @param [in]  TableFactoryProtocol Pointer to the Table Factory Protocol
                                    interface.
  @param [in]  CfgMgrProtocol       Pointer to the Configuration Manager
                                    Protocol Interface.
  @param [in]  SmbiosProtocol       Pointer to the SMBIOS protocol.
  @param [in]  SmbiosTableInfo      Pointer to the SMBIOS table Info.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         Required object is not found.
  @retval EFI_BAD_BUFFER_SIZE   Size returned by the Configuration Manager
                                is less than the Object size for the
                                requested object.
**/
EFI_STATUS
EFIAPI
DynamicTableManagerDxeInitialize (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_EVENT  AcpiEvent;
  EFI_EVENT  SmbiosEvent;

  AcpiEvent = EfiCreateProtocolNotifyEvent (
                &gEfiAcpiTableProtocolGuid,
                TPL_CALLBACK,
                AcpiTableProtocolReady,
                NULL,
                &AcpiTableProtocolRegistration
                );
  if (AcpiEvent == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to ACPI create protocol event\r\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  SmbiosEvent = EfiCreateProtocolNotifyEvent (
                  &gEfiSmbiosProtocolGuid,
                  TPL_CALLBACK,
                  SmbiosProtocolReady,
                  NULL,
                  &SmbiosProtocolRegistration
                  );
  if (SmbiosEvent == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to SMBIOS create protocol event\r\n", __FUNCTION__));
    gBS->CloseEvent (AcpiEvent);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}
