/** @file SmmuConfigLib.h

    Library class interface for providing SMMU configuration data to SmmuDxe.

    Platforms implement this library to supply SMMUv3 node information,
    disabled-SMMU lists, RMR memory ranges, and optionally raw IORT data
    for ACPI table installation.

    The default implementation (SmmuConfigHobLib) reads from the
    gSmmuConfigHobGuid HOB and parses the embedded IORT binary.
    Platform-specific implementations may source data from DTB,
    Configuration Manager, or other mechanisms.

    Copyright (c) 2026, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef SMMU_CONFIG_LIB_H_
#define SMMU_CONFIG_LIB_H_

#include <Uefi/UefiBaseType.h>

#pragma pack(push, 1)

typedef struct _SMMU_PLATFORM_NODE {
  UINT64    SmmuBase;
  UINT32    Flags;
  UINT32    StreamTableEntryMax;
} SMMU_PLATFORM_NODE;

typedef struct _SMMU_PLATFORM_RMR {
  UINT64    SmmuBase;
  UINT64    BaseAddress;
  UINT64    Length;
} SMMU_PLATFORM_RMR;

#pragma pack(pop)

/**
  Retrieve SMMUv3 node information from the platform.

  The caller must free the returned Nodes array with FreePool().

  @param [out] Nodes      Pointer to receive allocated array of SMMU_PLATFORM_NODE.
  @param [out] NodeCount  Number of entries in the returned array.

  @retval EFI_SUCCESS            Data returned successfully.
  @retval EFI_NOT_FOUND          No SMMUv3 nodes found.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation failure.
  @retval EFI_INVALID_PARAMETER  A parameter is NULL.
**/
EFI_STATUS
EFIAPI
SmmuConfigGetNodes (
  OUT SMMU_PLATFORM_NODE  **Nodes,
  OUT UINT32              *NodeCount
  );

/**
  Retrieve the list of SMMU base addresses that should be disabled.

  The caller must free the returned DisabledBases array with FreePool()
  when DisabledCount > 0.

  @param [out] DisabledBases  Pointer to receive allocated array of UINT64 base addresses.
  @param [out] DisabledCount  Number of entries in the returned array.

  @retval EFI_SUCCESS            Data returned successfully (count may be 0).
  @retval EFI_INVALID_PARAMETER  A parameter is NULL.
**/
EFI_STATUS
EFIAPI
SmmuConfigGetDisabledList (
  OUT UINT64  **DisabledBases,
  OUT UINT32  *DisabledCount
  );

/**
  Retrieve RMR (Reserved Memory Region) entries for SMMUs.

  The caller must free the returned RmrEntries array with FreePool()
  when RmrCount > 0.

  @param [out] RmrEntries  Pointer to receive allocated array of SMMU_PLATFORM_RMR.
  @param [out] RmrCount    Number of entries in the returned array.

  @retval EFI_SUCCESS            Data returned successfully (count may be 0).
  @retval EFI_INVALID_PARAMETER  A parameter is NULL.
**/
EFI_STATUS
EFIAPI
SmmuConfigGetRmrInfo (
  OUT SMMU_PLATFORM_RMR  **RmrEntries,
  OUT UINT32             *RmrCount
  );

/**
  Retrieve raw IORT table data for ACPI installation.

  If the platform does not provide IORT data (e.g. Configuration Manager
  handles IORT installation), this function returns EFI_NOT_FOUND.

  The returned IortData pointer references library-managed memory and
  must NOT be freed by the caller.

  @param [out] IortData  Pointer to receive the IORT table data.
  @param [out] IortSize  Size of the IORT table in bytes.

  @retval EFI_SUCCESS            IORT data returned successfully.
  @retval EFI_NOT_FOUND          Platform does not provide IORT data.
  @retval EFI_INVALID_PARAMETER  A parameter is NULL.
**/
EFI_STATUS
EFIAPI
SmmuConfigGetIortData (
  OUT VOID    **IortData,
  OUT UINT32  *IortSize
  );

#endif
