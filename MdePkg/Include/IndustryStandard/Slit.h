/** @file
  ACPI System Locality Information Table (SLIT)

  https://developer.arm.com/documentation/ddi0598/latest

  Copyright (c) 2022 NVIDIA ????????

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __SLIT_H__
#define __SLIT_H__

#pragma pack(1)

///
/// System Locality Information Table (SLIT)
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  UINT64                         NumLocalities;
} EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE;

///
/// SLIT Revision (as defined in ACPI 6.4 spec.)
///
#define EFI_ACPI_6_4_SYSTEM_LOCALITY_INFORMATION_TABLE_REVISION  0x01

///
/// Relative Distance Info Node Structure
///
/*
typedef struct {
  UINT8         *Distance;
} EFI_ACPI_6_4_REL_DISTANCE_INFO_STRUCTURE;
*/
#pragma pack()

#endif
