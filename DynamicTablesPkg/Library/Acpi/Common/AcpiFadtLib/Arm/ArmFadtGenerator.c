/** @file
  ARM FADT Table Helpers

  Copyright (c) 2017 - 2023, Arm Limited. All rights reserved.
  Copyright (c) 2026, NVIDIA CORPORATION. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.6 Specification, May 13, 2025

**/

#include <Library/AcpiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AcpiTable.h>

// Module specific include files.
#include <AcpiTableGenerator.h>
#include <ConfigurationManagerObject.h>
#include <ConfigurationManagerHelper.h>
#include <Library/TableHelperLib.h>
#include <Protocol/ConfigurationManagerProtocol.h>
#include "FadtGenerator.h"

/** ARM Standard FADT Generator

Requirements:
  The following Configuration Manager Object(s) are required by
  this Generator:
  - EArmObjBootArchInfo
*/

/** This macro expands to a function that retrieves the Boot
    Architecture Information from the Configuration Manager.
*/
GET_OBJECT_LIST (
  EObjNameSpaceArm,
  EArmObjBootArchInfo,
  CM_ARM_BOOT_ARCH_INFO
  );

/** This macro defines the FADT flag options for ARM Platforms.
*/
#define FADT_FLAGS  (EFI_ACPI_6_6_HW_REDUCED_ACPI |          \
                     EFI_ACPI_6_6_LOW_POWER_S0_IDLE_CAPABLE)

/** Updates the Architecture specific information in the FADT Table.

  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.
  @param [in, out] Fadt       Pointer to the constructed ACPI Table.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
STATIC
EFI_STATUS
EFIAPI
ArmFadtBootArchInfoUpdate (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN   OUT EFI_ACPI_6_6_FIXED_ACPI_DESCRIPTION_TABLE      *Fadt
  )
{
  EFI_STATUS             Status;
  CM_ARM_BOOT_ARCH_INFO  *BootArchInfo;

  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Fadt != NULL);

  // Get the Boot Architecture flags from the Platform Configuration Manager
  Status = GetEArmObjBootArchInfo (
             CfgMgrProtocol,
             CM_NULL_TOKEN,
             &BootArchInfo,
             NULL
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "ERROR: FADT: Failed to get Boot Architecture flags. Status = %r\n",
      Status
      ));
    return Status;
  }

  DEBUG ((
    DEBUG_INFO,
    "FADT BootArchFlag = 0x%x\n",
    BootArchInfo->BootArchFlags
    ));

  Fadt->ArmBootArch = BootArchInfo->BootArchFlags;

  return Status;
}

/** Updates the Architecture specific information in the FADT Table.

  @param [in]  CfgMgrProtocol Pointer to the Configuration Manager
                              Protocol Interface.
  @param [in, out] Fadt       Pointer to the constructed ACPI Table.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object was not found.
  @retval EFI_BAD_BUFFER_SIZE   The size returned by the Configuration
                                Manager is less than the Object size for the
                                requested object.
**/
EFI_STATUS
EFIAPI
FadtArchUpdate (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN   OUT EFI_ACPI_6_6_FIXED_ACPI_DESCRIPTION_TABLE      *Fadt
  )
{
  UINT32  StrippedFlags;

  ASSERT (CfgMgrProtocol != NULL);
  ASSERT (Fadt != NULL);

  Fadt->Flags |= FADT_FLAGS;

  StrippedFlags = Fadt->Flags & ~(VALID_HARDWARE_REDUCED_FLAG_MASK);
  if (StrippedFlags != 0) {
    DEBUG ((
      DEBUG_WARN,
      "FADT: Arm forces HW_REDUCED_ACPI; stripping non-hardware-reduced "
      "Flags bits = 0x%x\n",
      StrippedFlags
      ));
    Fadt->Flags &= VALID_HARDWARE_REDUCED_FLAG_MASK;
  }

  return ArmFadtBootArchInfoUpdate (CfgMgrProtocol, Fadt);
}
