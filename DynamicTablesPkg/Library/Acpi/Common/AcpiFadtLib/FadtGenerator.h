/** @file
  FADT Table Generator

  Copyright (c) 2017 - 2023, Arm Limited. All rights reserved.
  Copyright (c) 2026, NVIDIA CORPORATION. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.6 Specification, May 13, 2025

**/

#ifndef FADT_GENERATOR_H_
#define FADT_GENERATOR_H_

#include <IndustryStandard/Acpi.h>

/** This macro defines the valid mask for the FADT flag option
    if HW_REDUCED_ACPI flag in the table is set.

  Invalid bits are: 1, 2, 3, 7, 8, 13, 14, 16, 17 and
    22-31 (reserved).

  Valid bits are:
    EFI_ACPI_6_6_WBINVD                               BIT0
    EFI_ACPI_6_6_PWR_BUTTON                           BIT4
    EFI_ACPI_6_6_SLP_BUTTON                           BIT5
    EFI_ACPI_6_6_FIX_RTC                              BIT6
    EFI_ACPI_6_6_DCK_CAP                              BIT9
    EFI_ACPI_6_6_RESET_REG_SUP                        BIT10
    EFI_ACPI_6_6_SEALED_CASE                          BIT11
    EFI_ACPI_6_6_HEADLESS                             BIT12
    EFI_ACPI_6_6_USE_PLATFORM_CLOCK                   BIT15
    EFI_ACPI_6_6_FORCE_APIC_CLUSTER_MODEL             BIT18
    EFI_ACPI_6_6_FORCE_APIC_PHYSICAL_DESTINATION_MODE BIT19
    EFI_ACPI_6_6_HW_REDUCED_ACPI                      BIT20
    EFI_ACPI_6_6_LOW_POWER_S0_IDLE_CAPABLE            BIT21
*/
#define VALID_HARDWARE_REDUCED_FLAG_MASK  (                   \
          EFI_ACPI_6_6_WBINVD                               | \
          EFI_ACPI_6_6_PWR_BUTTON                           | \
          EFI_ACPI_6_6_SLP_BUTTON                           | \
          EFI_ACPI_6_6_FIX_RTC                              | \
          EFI_ACPI_6_6_DCK_CAP                              | \
          EFI_ACPI_6_6_RESET_REG_SUP                        | \
          EFI_ACPI_6_6_SEALED_CASE                          | \
          EFI_ACPI_6_6_HEADLESS                             | \
          EFI_ACPI_6_6_USE_PLATFORM_CLOCK                   | \
          EFI_ACPI_6_6_FORCE_APIC_CLUSTER_MODEL             | \
          EFI_ACPI_6_6_FORCE_APIC_PHYSICAL_DESTINATION_MODE | \
          EFI_ACPI_6_6_HW_REDUCED_ACPI                      | \
          EFI_ACPI_6_6_LOW_POWER_S0_IDLE_CAPABLE)

/** This macro defines the valid mask for the FADT flag option
    if HW_REDUCED_ACPI flag in the table is not set.

  Invalid bits are: Bit 20 and 22-31 (reserved).

  Valid bits are:
    EFI_ACPI_6_6_WBINVD                                BIT0
    EFI_ACPI_6_6_WBINVD_FLUSH                          BIT1
    EFI_ACPI_6_6_PROC_C1                               BIT2
    EFI_ACPI_6_6_P_LVL2_UP                             BIT3
    EFI_ACPI_6_6_PWR_BUTTON                            BIT4
    EFI_ACPI_6_6_SLP_BUTTON                            BIT5
    EFI_ACPI_6_6_FIX_RTC                               BIT6
    EFI_ACPI_6_6_RTC_S4                                BIT7
    EFI_ACPI_6_6_TMR_VAL_EXT                           BIT8
    EFI_ACPI_6_6_DCK_CAP                               BIT9
    EFI_ACPI_6_6_RESET_REG_SUP                         BIT10
    EFI_ACPI_6_6_SEALED_CASE                           BIT11
    EFI_ACPI_6_6_HEADLESS                              BIT12
    EFI_ACPI_6_6_CPU_SW_SLP                            BIT13
    EFI_ACPI_6_6_PCI_EXP_WAK                           BIT14
    EFI_ACPI_6_6_USE_PLATFORM_CLOCK                    BIT15
    EFI_ACPI_6_6_S4_RTC_STS_VALID                      BIT16
    EFI_ACPI_6_6_REMOTE_POWER_ON_CAPABLE               BIT17
    EFI_ACPI_6_6_FORCE_APIC_CLUSTER_MODEL              BIT18
    EFI_ACPI_6_6_FORCE_APIC_PHYSICAL_DESTINATION_MODE  BIT19
    EFI_ACPI_6_6_LOW_POWER_S0_IDLE_CAPABLE             BIT21
*/
#define VALID_NON_HARDWARE_REDUCED_FLAG_MASK  (               \
          EFI_ACPI_6_6_WBINVD                               | \
          EFI_ACPI_6_6_WBINVD_FLUSH                         | \
          EFI_ACPI_6_6_PROC_C1                              | \
          EFI_ACPI_6_6_P_LVL2_UP                            | \
          EFI_ACPI_6_6_PWR_BUTTON                           | \
          EFI_ACPI_6_6_SLP_BUTTON                           | \
          EFI_ACPI_6_6_FIX_RTC                              | \
          EFI_ACPI_6_6_RTC_S4                               | \
          EFI_ACPI_6_6_TMR_VAL_EXT                          | \
          EFI_ACPI_6_6_DCK_CAP                              | \
          EFI_ACPI_6_6_RESET_REG_SUP                        | \
          EFI_ACPI_6_6_SEALED_CASE                          | \
          EFI_ACPI_6_6_HEADLESS                             | \
          EFI_ACPI_6_6_CPU_SW_SLP                           | \
          EFI_ACPI_6_6_PCI_EXP_WAK                          | \
          EFI_ACPI_6_6_USE_PLATFORM_CLOCK                   | \
          EFI_ACPI_6_6_S4_RTC_STS_VALID                     | \
          EFI_ACPI_6_6_REMOTE_POWER_ON_CAPABLE              | \
          EFI_ACPI_6_6_FORCE_APIC_CLUSTER_MODEL             | \
          EFI_ACPI_6_6_FORCE_APIC_PHYSICAL_DESTINATION_MODE | \
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
EFI_STATUS
EFIAPI
FadtArchUpdate (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  *CONST  CfgMgrProtocol,
  IN   OUT EFI_ACPI_6_6_FIXED_ACPI_DESCRIPTION_TABLE      *Fadt
  );

#endif // FADT_GENERATOR_H_
