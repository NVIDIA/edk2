/** @file SmmuConfigHobLib.c

    Default implementation of SmmuConfigLib that reads SMMU configuration
    from the gSmmuConfigHobGuid HOB, parses the embedded IORT binary, and
    publishes the IORT table when ACPI table services are available.

    This provides backward compatibility for platforms that build a
    SMMU_CONFIG HOB with an embedded IORT table at PrePi/SEC time.

    Copyright (c) 2026, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmuConfigLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/IoRemappingTable.h>
#include <Guid/SmmuConfig.h>
#include <Protocol/AcpiTable.h>

STATIC EFI_EVENT  mAcpiTableNotifyEvent;
STATIC VOID       *mAcpiTableNotifyRegistration;
STATIC BOOLEAN    mIortInstalled;

STATIC
EFI_STATUS
PublishIortTable (
  VOID
  );

/**
  Retrieve and validate the SMMU_CONFIG HOB.

  @param [out] SmmuConfig  Pointer to receive the SMMU_CONFIG structure.

  @retval EFI_SUCCESS               HOB found and version matches.
  @retval EFI_NOT_FOUND             HOB not present.
  @retval EFI_INCOMPATIBLE_VERSION  Version mismatch.
**/
STATIC
EFI_STATUS
GetValidatedSmmuConfig (
  OUT SMMU_CONFIG  **SmmuConfig
  )
{
  VOID   *GuidHob;
  UINTN  HobDataSize;

  GuidHob = GetFirstGuidHob (&gSmmuConfigHobGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }

  HobDataSize = GET_GUID_HOB_DATA_SIZE (GuidHob);
  if (HobDataSize < sizeof (SMMU_CONFIG)) {
    DEBUG ((DEBUG_ERROR, "%a: HOB data size %u too small for SMMU_CONFIG\n", __func__, HobDataSize));
    return EFI_INVALID_PARAMETER;
  }

  *SmmuConfig = (SMMU_CONFIG *)GET_GUID_HOB_DATA (GuidHob);

  if (((*SmmuConfig)->VersionMajor != CURRENT_SMMU_CONFIG_VERSION_MAJOR) ||
      ((*SmmuConfig)->VersionMinor != CURRENT_SMMU_CONFIG_VERSION_MINOR))
  {
    DEBUG ((
      DEBUG_ERROR,
      "%a: SMMU_CONFIG version mismatch. Expected: %u.%u Got: %u.%u\n",
      __func__,
      CURRENT_SMMU_CONFIG_VERSION_MAJOR,
      CURRENT_SMMU_CONFIG_VERSION_MINOR,
      (*SmmuConfig)->VersionMajor,
      (*SmmuConfig)->VersionMinor
      ));
    return EFI_INCOMPATIBLE_VERSION;
  }

  if (((*SmmuConfig)->IortOffset < sizeof (SMMU_CONFIG)) ||
      ((*SmmuConfig)->IortOffset > HobDataSize) ||
      ((*SmmuConfig)->IortSize > HobDataSize - (*SmmuConfig)->IortOffset))
  {
    DEBUG ((
      DEBUG_ERROR,
      "%a: IortOffset/IortSize out of bounds (Offset=%u Size=%u HobData=%u)\n",
      __func__,
      (*SmmuConfig)->IortOffset,
      (*SmmuConfig)->IortSize,
      HobDataSize
      ));
    return EFI_INVALID_PARAMETER;
  }

  if ((*SmmuConfig)->SmmuDisabledListSize > 0) {
    if (((*SmmuConfig)->SmmuDisabledListOffset < sizeof (SMMU_CONFIG)) ||
        ((*SmmuConfig)->SmmuDisabledListOffset > HobDataSize) ||
        ((*SmmuConfig)->SmmuDisabledListSize > HobDataSize - (*SmmuConfig)->SmmuDisabledListOffset))
    {
      DEBUG ((
        DEBUG_ERROR,
        "%a: SmmuDisabledListOffset/Size out of bounds (Offset=%u Size=%u HobData=%u)\n",
        __func__,
        (*SmmuConfig)->SmmuDisabledListOffset,
        (*SmmuConfig)->SmmuDisabledListSize,
        HobDataSize
        ));
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

/**
  Count SMMUv3 nodes in the IORT table.
**/
STATIC
EFI_STATUS
IortCountSmmuNodes (
  IN  EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort,
  OUT UINT32                           *Count
  )
{
  EFI_ACPI_6_0_IO_REMAPPING_NODE  *Node;
  UINT32                          Index;

  *Count = 0;
  Node   = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Iort + Iort->NodeOffset);

  for (Index = 0; Index < Iort->NumNodes; Index++) {
    if (Node->Type == EFI_ACPI_IORT_TYPE_SMMUv3) {
      (*Count)++;
    }

    if (Node->Length < sizeof (EFI_ACPI_6_0_IO_REMAPPING_NODE)) {
      DEBUG ((DEBUG_ERROR, "%a: Node->Length too small\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    if (((UINT8 *)Node + Node->Length) > ((UINT8 *)Iort + Iort->Header.Length)) {
      DEBUG ((DEBUG_ERROR, "%a: Node extends beyond IORT table\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Node + Node->Length);
  }

  return EFI_SUCCESS;
}

/**
  Collect SMMUv3 node info and record SMMU node pointers for
  output-reference matching.
**/
STATIC
EFI_STATUS
IortCollectSmmuNodes (
  IN  EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort,
  OUT SMMU_PLATFORM_NODE               *Nodes,
  OUT VOID                             **SmmuNodePtrs,
  IN  UINT32                           NodeCount
  )
{
  EFI_ACPI_6_0_IO_REMAPPING_NODE        *Node;
  EFI_ACPI_6_0_IO_REMAPPING_SMMU3_NODE  *SmmuNode;
  UINT32                                Index;
  UINT32                                SmmuIndex;

  Node      = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Iort + Iort->NodeOffset);
  SmmuIndex = 0;

  for (Index = 0; Index < Iort->NumNodes; Index++) {
    if (Node->Type == EFI_ACPI_IORT_TYPE_SMMUv3) {
      SmmuNode = (EFI_ACPI_6_0_IO_REMAPPING_SMMU3_NODE *)Node;

      Nodes[SmmuIndex].SmmuBase            = SmmuNode->Base;
      Nodes[SmmuIndex].Flags               = SmmuNode->Flags;
      Nodes[SmmuIndex].StreamTableEntryMax = 0;
      SmmuNodePtrs[SmmuIndex]              = (VOID *)SmmuNode;
      SmmuIndex++;
    }

    if (Node->Length < sizeof (EFI_ACPI_6_0_IO_REMAPPING_NODE)) {
      DEBUG ((DEBUG_ERROR, "%a: Node->Length too small\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    if (((UINT8 *)Node + Node->Length) > ((UINT8 *)Iort + Iort->Header.Length)) {
      DEBUG ((DEBUG_ERROR, "%a: Node extends beyond IORT table\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Node + Node->Length);
  }

  return EFI_SUCCESS;
}

/**
  Walk Root Complex and Named Component nodes to find the maximum
  stream ID mapping to each SMMU via output references.
**/
STATIC
EFI_STATUS
IortGetMaxStreamIds (
  IN     EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort,
  IN     VOID                             **SmmuNodePtrs,
  IN     UINT32                           SmmuNodeCount,
  IN OUT SMMU_PLATFORM_NODE               *Nodes
  )
{
  EFI_ACPI_6_0_IO_REMAPPING_NODE      *Node;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE  *IdMapping;
  VOID                                *OutputNode;
  UINT32                              Index;
  UINT32                              IdIdx;
  UINT32                              SmmuIdx;
  UINT32                              CurMax;

  Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Iort + Iort->NodeOffset);

  for (Index = 0; Index < Iort->NumNodes; Index++) {
    if (((Node->Type == EFI_ACPI_IORT_TYPE_ROOT_COMPLEX) ||
         (Node->Type == EFI_ACPI_IORT_TYPE_NAMED_COMP)) &&
        (Node->NumIdMappings > 0))
    {
      if (((UINT8 *)Node + Node->IdReference + Node->NumIdMappings * sizeof (EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE)) >
          ((UINT8 *)Iort + Iort->Header.Length))
      {
        DEBUG ((DEBUG_ERROR, "%a: IdMapping array extends beyond IORT table\n", __func__));
        return EFI_INVALID_PARAMETER;
      }

      IdMapping = (EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE *)((UINT8 *)Node + Node->IdReference);

      for (IdIdx = 0; IdIdx < Node->NumIdMappings; IdIdx++) {
        if (IdMapping[IdIdx].OutputReference >= Iort->Header.Length) {
          DEBUG ((DEBUG_ERROR, "%a: OutputReference 0x%x beyond IORT\n", __func__, IdMapping[IdIdx].OutputReference));
          return EFI_INVALID_PARAMETER;
        }

        OutputNode = (VOID *)((UINT8 *)Iort + IdMapping[IdIdx].OutputReference);

        for (SmmuIdx = 0; SmmuIdx < SmmuNodeCount; SmmuIdx++) {
          if (OutputNode == SmmuNodePtrs[SmmuIdx]) {
            CurMax = IdMapping[IdIdx].OutputBase + IdMapping[IdIdx].NumIds;
            if (CurMax > Nodes[SmmuIdx].StreamTableEntryMax) {
              Nodes[SmmuIdx].StreamTableEntryMax = CurMax;
            }

            break;
          }
        }
      }
    }

    if (Node->Length < sizeof (EFI_ACPI_6_0_IO_REMAPPING_NODE)) {
      DEBUG ((DEBUG_ERROR, "%a: Node->Length too small\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    if (((UINT8 *)Node + Node->Length) > ((UINT8 *)Iort + Iort->Header.Length)) {
      DEBUG ((DEBUG_ERROR, "%a: Node extends beyond IORT table\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Node + Node->Length);
  }

  return EFI_SUCCESS;
}

/**
  Collect RMR nodes and produce SMMU_PLATFORM_RMR entries.
**/
STATIC
EFI_STATUS
IortCollectRmrEntries (
  IN  EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort,
  IN  VOID                             **SmmuNodePtrs,
  IN  SMMU_PLATFORM_NODE               *SmmuNodes,
  IN  UINT32                           SmmuNodeCount,
  OUT SMMU_PLATFORM_RMR                **RmrEntries,
  OUT UINT32                           *RmrCount
  )
{
  EFI_ACPI_6_0_IO_REMAPPING_NODE            *Node;
  EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE        *IdMapping;
  EFI_ACPI_6_0_IO_REMAPPING_RMR_NODE        *RmrNode;
  EFI_ACPI_6_0_IO_REMAPPING_MEM_RANGE_DESC  *MemDesc;
  VOID                                      *OutputNode;
  UINT32                                    Index;
  UINT32                                    IdIdx;
  UINT32                                    SmmuIdx;
  UINT32                                    DescIdx;
  UINT32                                    TotalRmrs;
  SMMU_PLATFORM_RMR                         *Entries;

  *RmrEntries = NULL;
  *RmrCount   = 0;

  TotalRmrs = 0;
  Node      = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Iort + Iort->NodeOffset);
  for (Index = 0; Index < Iort->NumNodes; Index++) {
    if ((Node->Type == EFI_ACPI_IORT_TYPE_RMR) && (Node->NumIdMappings > 0)) {
      RmrNode = (EFI_ACPI_6_0_IO_REMAPPING_RMR_NODE *)Node;

      if (RmrNode->NumMemRangeDesc > (MAX_UINT32 / Node->NumIdMappings)) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: RMR count overflow: NumMemRangeDesc=%u NumIdMappings=%u\n",
          __func__,
          RmrNode->NumMemRangeDesc,
          Node->NumIdMappings
          ));
        return EFI_INVALID_PARAMETER;
      }

      if (RmrNode->NumMemRangeDesc * Node->NumIdMappings > MAX_UINT32 - TotalRmrs) {
        DEBUG ((DEBUG_ERROR, "%a: TotalRmrs overflow\n", __func__));
        return EFI_INVALID_PARAMETER;
      }

      TotalRmrs += RmrNode->NumMemRangeDesc * Node->NumIdMappings;
    }

    if (Node->Length < sizeof (EFI_ACPI_6_0_IO_REMAPPING_NODE)) {
      DEBUG ((DEBUG_ERROR, "%a: Node->Length too small\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    if (((UINT8 *)Node + Node->Length) > ((UINT8 *)Iort + Iort->Header.Length)) {
      DEBUG ((DEBUG_ERROR, "%a: Node extends beyond IORT table\n", __func__));
      return EFI_INVALID_PARAMETER;
    }

    Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Node + Node->Length);
  }

  if (TotalRmrs == 0) {
    return EFI_SUCCESS;
  }

  if (TotalRmrs > MAX_UINTN / sizeof (SMMU_PLATFORM_RMR)) {
    DEBUG ((DEBUG_ERROR, "%a: Allocation size overflow: TotalRmrs=%u\n", __func__, TotalRmrs));
    return EFI_INVALID_PARAMETER;
  }

  Entries = AllocateZeroPool (TotalRmrs * sizeof (SMMU_PLATFORM_RMR));
  if (Entries == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TotalRmrs = 0;
  Node      = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Iort + Iort->NodeOffset);
  for (Index = 0; Index < Iort->NumNodes; Index++) {
    if ((Node->Type == EFI_ACPI_IORT_TYPE_RMR) && (Node->NumIdMappings > 0)) {
      if (((UINT8 *)Node + Node->IdReference + Node->NumIdMappings * sizeof (EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE)) >
          ((UINT8 *)Iort + Iort->Header.Length))
      {
        DEBUG ((DEBUG_ERROR, "%a: IdMapping array extends beyond IORT table\n", __func__));
        FreePool (Entries);
        return EFI_INVALID_PARAMETER;
      }

      RmrNode   = (EFI_ACPI_6_0_IO_REMAPPING_RMR_NODE *)Node;
      IdMapping = (EFI_ACPI_6_0_IO_REMAPPING_ID_TABLE *)((UINT8 *)Node + Node->IdReference);

      if (((UINT8 *)RmrNode + RmrNode->MemRangeDescRef + RmrNode->NumMemRangeDesc * sizeof (EFI_ACPI_6_0_IO_REMAPPING_MEM_RANGE_DESC)) >
          ((UINT8 *)Iort + Iort->Header.Length))
      {
        DEBUG ((DEBUG_ERROR, "%a: MemRangeDesc array extends beyond IORT table\n", __func__));
        FreePool (Entries);
        return EFI_INVALID_PARAMETER;
      }

      for (IdIdx = 0; IdIdx < Node->NumIdMappings; IdIdx++) {
        if (IdMapping[IdIdx].OutputReference >= Iort->Header.Length) {
          DEBUG ((DEBUG_ERROR, "%a: OutputReference 0x%x beyond IORT\n", __func__, IdMapping[IdIdx].OutputReference));
          FreePool (Entries);
          return EFI_INVALID_PARAMETER;
        }

        OutputNode = (VOID *)((UINT8 *)Iort + IdMapping[IdIdx].OutputReference);

        for (SmmuIdx = 0; SmmuIdx < SmmuNodeCount; SmmuIdx++) {
          if (OutputNode == SmmuNodePtrs[SmmuIdx]) {
            MemDesc = (EFI_ACPI_6_0_IO_REMAPPING_MEM_RANGE_DESC *)
                      ((UINT8 *)RmrNode + RmrNode->MemRangeDescRef);

            for (DescIdx = 0; DescIdx < RmrNode->NumMemRangeDesc; DescIdx++) {
              if ((MemDesc[DescIdx].Base > 0) && (MemDesc[DescIdx].Length > 0)) {
                Entries[TotalRmrs].SmmuBase    = SmmuNodes[SmmuIdx].SmmuBase;
                Entries[TotalRmrs].BaseAddress = MemDesc[DescIdx].Base;
                Entries[TotalRmrs].Length      = MemDesc[DescIdx].Length;
                TotalRmrs++;
              }
            }

            break;
          }
        }
      }
    }

    if (Node->Length < sizeof (EFI_ACPI_6_0_IO_REMAPPING_NODE)) {
      DEBUG ((DEBUG_ERROR, "%a: Node->Length too small\n", __func__));
      FreePool (Entries);
      return EFI_INVALID_PARAMETER;
    }

    if (((UINT8 *)Node + Node->Length) > ((UINT8 *)Iort + Iort->Header.Length)) {
      DEBUG ((DEBUG_ERROR, "%a: Node extends beyond IORT table\n", __func__));
      FreePool (Entries);
      return EFI_INVALID_PARAMETER;
    }

    Node = (EFI_ACPI_6_0_IO_REMAPPING_NODE *)((UINT8 *)Node + Node->Length);
  }

  if (TotalRmrs == 0) {
    FreePool (Entries);
    Entries = NULL;
  }

  *RmrEntries = Entries;
  *RmrCount   = TotalRmrs;
  return EFI_SUCCESS;
}

/**
  Parse the IORT from the SMMU_CONFIG HOB and produce SMMU_PLATFORM_NODE entries.
**/
STATIC
EFI_STATUS
ParseIortForSmmuNodes (
  IN  SMMU_CONFIG         *SmmuConfig,
  OUT SMMU_PLATFORM_NODE  **Nodes,
  OUT UINT32              *NodeCount,
  OUT VOID                ***SmmuNodePtrsOut
  )
{
  EFI_STATUS                       Status;
  EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort;
  UINT32                           Count;
  SMMU_PLATFORM_NODE               *PlatformNodes;
  VOID                             **SmmuNodePtrs;

  Iort = (EFI_ACPI_6_0_IO_REMAPPING_TABLE *)((UINT8 *)SmmuConfig + SmmuConfig->IortOffset);

  if (Iort->Header.Signature != EFI_ACPI_6_0_IO_REMAPPING_TABLE_SIGNATURE) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid IORT signature: 0x%08X\n", __func__, Iort->Header.Signature));
    return EFI_UNSUPPORTED;
  }

  if ((Iort->Header.Revision != EFI_ACPI_IO_REMAPPING_TABLE_REVISION_00) &&
      (Iort->Header.Revision != EFI_ACPI_IO_REMAPPING_TABLE_REVISION_06))
  {
    DEBUG ((DEBUG_ERROR, "%a: Unsupported IORT revision: %d\n", __func__, Iort->Header.Revision));
    return EFI_UNSUPPORTED;
  }

  Status = IortCountSmmuNodes (Iort, &Count);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Count == 0) {
    *Nodes           = NULL;
    *NodeCount       = 0;
    *SmmuNodePtrsOut = NULL;
    return EFI_NOT_FOUND;
  }

  PlatformNodes = AllocateZeroPool (Count * sizeof (SMMU_PLATFORM_NODE));
  if (PlatformNodes == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmmuNodePtrs = AllocateZeroPool (Count * sizeof (VOID *));
  if (SmmuNodePtrs == NULL) {
    FreePool (PlatformNodes);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = IortCollectSmmuNodes (Iort, PlatformNodes, SmmuNodePtrs, Count);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = IortGetMaxStreamIds (Iort, SmmuNodePtrs, Count, PlatformNodes);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  *Nodes           = PlatformNodes;
  *NodeCount       = Count;
  *SmmuNodePtrsOut = SmmuNodePtrs;
  return EFI_SUCCESS;

Error:
  FreePool (PlatformNodes);
  FreePool (SmmuNodePtrs);
  return Status;
}

//
// Module-scope cache so repeated calls return consistent data and
// SmmuNodePtrs remain valid for RMR collection.
//
STATIC SMMU_CONFIG         *mCachedSmmuConfig;
STATIC SMMU_PLATFORM_NODE  *mCachedNodes;
STATIC UINT32              mCachedNodeCount;
STATIC VOID                **mCachedSmmuNodePtrs;
STATIC BOOLEAN             mCacheInitialized;
STATIC EFI_STATUS          mCacheInitStatus = EFI_NOT_READY;

STATIC
EFI_STATUS
EnsureCacheInitialized (
  VOID
  )
{
  if (mCacheInitialized) {
    return mCacheInitStatus;
  }

  mCacheInitialized = TRUE;

  mCacheInitStatus = GetValidatedSmmuConfig (&mCachedSmmuConfig);
  if (EFI_ERROR (mCacheInitStatus)) {
    DEBUG ((DEBUG_ERROR, "%a: GetValidatedSmmuConfig failed: %r\n", __func__, mCacheInitStatus));
    return mCacheInitStatus;
  }

  mCacheInitStatus = ParseIortForSmmuNodes (
                       mCachedSmmuConfig,
                       &mCachedNodes,
                       &mCachedNodeCount,
                       &mCachedSmmuNodePtrs
                       );
  if (EFI_ERROR (mCacheInitStatus)) {
    DEBUG ((DEBUG_ERROR, "%a: ParseIortForSmmuNodes failed: %r\n", __func__, mCacheInitStatus));
  }

  return mCacheInitStatus;
}

EFI_STATUS
EFIAPI
SmmuConfigGetNodes (
  OUT SMMU_PLATFORM_NODE  **Nodes,
  OUT UINT32              *NodeCount
  )
{
  EFI_STATUS          Status;
  SMMU_PLATFORM_NODE  *Copy;

  if ((Nodes == NULL) || (NodeCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EnsureCacheInitialized ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PublishIortTable ();
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    return Status;
  }

  Copy = AllocateCopyPool (mCachedNodeCount * sizeof (SMMU_PLATFORM_NODE), mCachedNodes);
  if (Copy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Nodes     = Copy;
  *NodeCount = mCachedNodeCount;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmuConfigGetDisabledList (
  OUT UINT64  **DisabledBases,
  OUT UINT32  *DisabledCount
  )
{
  EFI_STATUS  Status;
  UINT32      Count;
  UINT64      *List;
  UINT64      *Copy;

  if ((DisabledBases == NULL) || (DisabledCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EnsureCacheInitialized ();
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    return Status;
  }

  if (mCachedSmmuConfig == NULL) {
    *DisabledBases = NULL;
    *DisabledCount = 0;
    return EFI_SUCCESS;
  }

  Count = mCachedSmmuConfig->SmmuDisabledListSize / sizeof (UINT64);
  if (Count == 0) {
    *DisabledBases = NULL;
    *DisabledCount = 0;
    return EFI_SUCCESS;
  }

  List = (UINT64 *)((UINTN)mCachedSmmuConfig + (UINTN)mCachedSmmuConfig->SmmuDisabledListOffset);
  Copy = AllocateCopyPool (Count * sizeof (UINT64), List);
  if (Copy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *DisabledBases = Copy;
  *DisabledCount = Count;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmuConfigGetRmrInfo (
  OUT SMMU_PLATFORM_RMR  **RmrEntries,
  OUT UINT32             *RmrCount
  )
{
  EFI_STATUS                       Status;
  EFI_ACPI_6_0_IO_REMAPPING_TABLE  *Iort;

  if ((RmrEntries == NULL) || (RmrCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EnsureCacheInitialized ();
  if (EFI_ERROR (Status)) {
    *RmrEntries = NULL;
    *RmrCount   = 0;
    return (Status == EFI_NOT_FOUND) ? EFI_SUCCESS : Status;
  }

  Iort = (EFI_ACPI_6_0_IO_REMAPPING_TABLE *)((UINT8 *)mCachedSmmuConfig + mCachedSmmuConfig->IortOffset);

  return IortCollectRmrEntries (
           Iort,
           mCachedSmmuNodePtrs,
           mCachedNodes,
           mCachedNodeCount,
           RmrEntries,
           RmrCount
           );
}

EFI_STATUS
EFIAPI
SmmuConfigGetIortData (
  OUT VOID    **IortData,
  OUT UINT32  *IortSize
  )
{
  EFI_STATUS  Status;

  if ((IortData == NULL) || (IortSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EnsureCacheInitialized ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *IortData = (VOID *)((UINTN)mCachedSmmuConfig + (UINTN)mCachedSmmuConfig->IortOffset);
  *IortSize = mCachedSmmuConfig->IortSize;
  return EFI_SUCCESS;
}

/**
  Calculate and update the checksum of an ACPI table.

  @param [in, out] Buffer  Pointer to the ACPI table buffer.
  @param [in]      Size    Size of the ACPI table buffer.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
**/
STATIC
EFI_STATUS
AcpiPlatformChecksum (
  IN OUT UINT8  *Buffer,
  IN UINTN      Size
  )
{
  UINTN  ChecksumOffset;

  if ((Buffer == NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  Buffer[ChecksumOffset] = 0;
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);

  return EFI_SUCCESS;
}

/**
  Install the HOB-provided IORT table if ACPI table services are available.

  @retval EFI_SUCCESS            IORT table installed.
  @retval EFI_NOT_FOUND          IORT data or ACPI table protocol not found.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation failure.
  @retval EFI_INVALID_PARAMETER  Invalid table data.
  @retval Others                 Error returned by InstallAcpiTable().
**/
STATIC
EFI_STATUS
InstallIortTable (
  VOID
  )
{
  EFI_STATUS               Status;
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable;
  UINTN                    TableHandle;
  VOID                     *IortData;
  VOID                     *IortCopy;
  UINT32                   IortSize;

  if (mIortInstalled) {
    return EFI_SUCCESS;
  }

  Status = SmmuConfigGetIortData (&IortData, &IortSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((IortData == NULL) || (IortSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid,
                  NULL,
                  (VOID **)&AcpiTable
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IortCopy = AllocateCopyPool (IortSize, IortData);
  if (IortCopy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = AcpiPlatformChecksum ((UINT8 *)IortCopy, IortSize);
  if (!EFI_ERROR (Status)) {
    Status = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          IortCopy,
                          IortSize,
                          &TableHandle
                          );
  }

  FreePool (IortCopy);

  if (!EFI_ERROR (Status)) {
    mIortInstalled = TRUE;
  }

  return Status;
}

/**
  Notification callback used when ACPI table protocol is installed after SmmuDxe.

  @param [in] Event    Event that was signaled.
  @param [in] Context  Optional context.
**/
STATIC
VOID
EFIAPI
AcpiTableProtocolNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;

  Status = InstallIortTable ();
  if (Status == EFI_NOT_FOUND) {
    return;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to install IORT table: %r\n", __func__, Status));
  }

  if (mAcpiTableNotifyEvent != NULL) {
    gBS->CloseEvent (mAcpiTableNotifyEvent);
    mAcpiTableNotifyEvent        = NULL;
    mAcpiTableNotifyRegistration = NULL;
  }
}

/**
  Publish the HOB-provided IORT table when possible.

  @retval EFI_SUCCESS            IORT table installed or deferred.
  @retval EFI_NOT_FOUND          No IORT data was found.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation failure.
  @retval EFI_INVALID_PARAMETER  Invalid table data.
  @retval Others                 Error returned by InstallAcpiTable().
**/
STATIC
EFI_STATUS
PublishIortTable (
  VOID
  )
{
  EFI_STATUS  Status;
  VOID        *IortData;
  UINT32      IortSize;

  Status = SmmuConfigGetIortData (&IortData, &IortSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InstallIortTable ();
  if (Status != EFI_NOT_FOUND) {
    return Status;
  }

  if (mAcpiTableNotifyEvent != NULL) {
    return EFI_SUCCESS;
  }

  mAcpiTableNotifyEvent = EfiCreateProtocolNotifyEvent (
                            &gEfiAcpiTableProtocolGuid,
                            TPL_CALLBACK,
                            AcpiTableProtocolNotify,
                            NULL,
                            &mAcpiTableNotifyRegistration
                            );
  if (mAcpiTableNotifyEvent == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmuConfigGetBypassStreamInfo (
  IN  EFI_HANDLE  DeviceHandle,
  OUT UINT64      *SmmuBase,
  OUT UINT32      *StreamId
  )
{
  if ((SmmuBase == NULL) || (StreamId == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_NOT_FOUND;
}
