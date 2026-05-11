/** @file SmmuDxe.c

    This file contains functions for the SMMU driver.

    This driver consumes platform SMMU configuration via SmmuConfigLib to configure the SMMU hardware.
    Initializes the SmmuV3 hardware to enable stage 2 translation and dma remapping.
    Implements the IoMmu protocol to provide a generic interface for mapping host memory to device memory.

    Copyright (c) Microsoft Corporation.
    Copyright (c) 2026, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmuConfigLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include "IoMmu.h"
#include "SmmuV3.h"

// Global IOMMU/SMMU instance
IOMMU_CONFIG  *mIoMmu;

/**
  Initialize a page table. Only initializes the root page table.
  UpdateMapping() will allocate entries on the fly as needed.

  To support concatenated page tables, allocate the max amount of pages we are allowed to concatenate, 16.

  @retval A pointer to the initialized page table, or NULL on failure.
**/
STATIC
PAGE_TABLE *
PageTableInit (
  VOID
  )
{
  PAGE_TABLE  *PageTable;

  // To support concatenated page tables, allocate the max amount of pages we are allowed to concatenate, 16.
  // Arm DDI0487L_a_a-profile_architecture_reference_manual section D8.2.2 states:
  // Align the base address of the first translation table to the sum of the size of the memory occupied by the concatenated translation tables.
  PageTable = (PAGE_TABLE *)AllocateAlignedPages (
                              PAGE_TABLE_ROOT_CONCATENATED_PAGES_MAX,
                              EFI_PAGE_SIZE * PAGE_TABLE_ROOT_CONCATENATED_PAGES_MAX
                              );

  if (PageTable == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate page table\n", __func__));
    return NULL;
  }

  ZeroMem (PageTable, EFI_PAGE_SIZE * PAGE_TABLE_ROOT_CONCATENATED_PAGES_MAX);

  return PageTable;
}

/**
  Recursivley deinitialize and free a page table for all previously
  allocated entries, given its level and pointer.

  @param [in]  Level      The level of the page table to deinitialize.
  @param [in]  PageTable  The page table to deinitialize.
**/
STATIC
VOID
PageTableDeInit (
  IN UINT8       Level,
  IN PAGE_TABLE  *PageTable
  )
{
  UINTN             Index;
  PAGE_TABLE_ENTRY  Entry;
  PAGE_TABLE        *PageTableAddress;

  if ((Level >= PAGE_TABLE_DEPTH) || (PageTable == NULL)) {
    return;
  }

  for (Index = 0; Index < PAGE_TABLE_SIZE; Index++) {
    Entry            = PageTable->Entries[Index];
    PageTableAddress = (PAGE_TABLE *)((UINTN)Entry & ~PAGE_TABLE_BLOCK_OFFSET);

    if (Entry != 0) {
      PageTableDeInit (Level + 1, PageTableAddress);
    }
  }

  // For root level, use larger size to free for supporting a concatenated page table root.
  if (Level == 0) {
    FreePages (PageTable, PAGE_TABLE_ROOT_CONCATENATED_PAGES_MAX);
  } else {
    FreePages (PageTable, 1);
  }
}

/**
  Allocate an event queue for SMMUv3.

  @param [in]   SmmuInfo       Pointer to the SMMU_INFO structure.
  @param [out]  QueueLog2Size  Pointer to store the log2 size of the queue.
  @param [out]  EventQueueBase Pointer to store the base address of the allocated event queue.

  @retval EFI_SUCCESS          The event queue was allocated successfully.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  Allocation failed due to insufficient resources.
**/
STATIC
EFI_STATUS
SmmuV3AllocateEventQueue (
  IN  SMMU_INFO  *SmmuInfo,
  OUT UINT32     *QueueLog2Size,
  OUT VOID       **EventQueueBase
  )
{
  UINT32       QueueSize;
  SMMUV3_IDR1  Idr1;
  UINT32       Pages;

  if ((SmmuInfo == NULL) || (QueueLog2Size == NULL) || (EventQueueBase == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Idr1.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR1);

  *QueueLog2Size  = MIN (Idr1.EventQs, SMMUV3_EVENT_QUEUE_LOG2ENTRIES);
  QueueSize       = SMMUV3_EVENT_QUEUE_SIZE_FROM_LOG2 (*QueueLog2Size);
  Pages           = EFI_SIZE_TO_PAGES (QueueSize);
  *EventQueueBase = AllocatePages (Pages);

  if (*EventQueueBase == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Allocation failed\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (*EventQueueBase, EFI_PAGES_TO_SIZE (Pages));
  return EFI_SUCCESS;
}

/**
  Allocate a command queue for SMMUv3.

  @param [in]   SmmuInfo       Pointer to the SMMU_INFO structure.
  @param [out]  QueueLog2Size  Pointer to store the log2 size of the queue.
  @param [out]  CmdQueueBase   Pointer to store the base address of the allocated command queue.

  @retval EFI_SUCCESS          The command queue was allocated successfully.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  Allocation failed due to insufficient resources.
**/
STATIC
EFI_STATUS
SmmuV3AllocateCommandQueue (
  IN  SMMU_INFO  *SmmuInfo,
  OUT UINT32     *QueueLog2Size,
  OUT VOID       **CmdQueueBase
  )
{
  UINT32       QueueSize;
  SMMUV3_IDR1  Idr1;
  UINT32       Pages;

  if ((SmmuInfo == NULL) || (QueueLog2Size == NULL) || (CmdQueueBase == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Idr1.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR1);

  *QueueLog2Size = MIN (Idr1.CmdQs, SMMUV3_COMMAND_QUEUE_LOG2ENTRIES);
  QueueSize      = SMMUV3_COMMAND_QUEUE_SIZE_FROM_LOG2 (*QueueLog2Size);
  Pages          = EFI_SIZE_TO_PAGES (QueueSize);
  *CmdQueueBase  = AllocatePages (Pages);

  if (*CmdQueueBase == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Allocation failed\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (*CmdQueueBase, EFI_PAGES_TO_SIZE (Pages));
  return EFI_SUCCESS;
}

/**
  Free a previously allocated queue.

  @param [in]  QueuePtr      Pointer to the queue to free.
  @param [in]  Log2Size      Log2 of the number of entries in the queue.
  @param [in]  IsEventQueue  TRUE for event queue (32-byte entries),
                             FALSE for command queue (16-byte entries).
**/
STATIC
VOID
SmmuV3FreeQueue (
  IN VOID     *QueuePtr,
  IN UINT32   Log2Size,
  IN BOOLEAN  IsEventQueue
  )
{
  UINT32  Size;

  if (QueuePtr == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid parameters. QueuePtr == NULL\n", __func__));
  } else {
    Size = IsEventQueue
             ? SMMUV3_EVENT_QUEUE_SIZE_FROM_LOG2 (Log2Size)
             : SMMUV3_COMMAND_QUEUE_SIZE_FROM_LOG2 (Log2Size);
    FreePages ((VOID *)QueuePtr, EFI_SIZE_TO_PAGES (Size));
  }
}

/**
  Build the default stream table entry for SMMUv3.

  @param [in]  SmmuInfo       Pointer to the SMMU_INFO structure.
  @param [out] StreamEntry    Pointer to the stream table entry.

  @retval EFI_SUCCESS         Success.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
**/
STATIC
EFI_STATUS
SmmuV3BuildStreamTableEntry (
  IN SMMU_INFO                   *SmmuInfo,
  OUT SMMUV3_STREAM_TABLE_ENTRY  *StreamEntry
  )
{
  EFI_STATUS   Status;
  UINT32       InputSize;
  SMMUV3_IDR0  Idr0;
  SMMUV3_IDR1  Idr1;
  SMMUV3_IDR5  Idr5;
  UINT8        IortCohac;
  UINT32       CCA;
  UINT8        CPM;
  UINT8        DACS;
  UINT64       S2Sl0;

  if ((SmmuInfo == NULL) || (StreamEntry == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  IortCohac = SmmuInfo->Flags & EFI_ACPI_IORT_SMMUv3_FLAG_COHAC_OVERRIDE; // Cohac override flag
  CCA       = SMMUV3_STREAM_TABLE_ENTRY_CCA;
  CPM       = SMMUV3_STREAM_TABLE_ENTRY_CPM;
  DACS      = SMMUV3_STREAM_TABLE_ENTRY_DACS;

  ZeroMem ((VOID *)StreamEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));

  Idr0.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR0);
  Idr1.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR1);
  Idr5.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR5);

  StreamEntry->Config = SMMUV3_STREAM_TABLE_ENTRY_CONFIG_STAGE_2_TRANSLATE_STAGE_1_BYPASS;
  StreamEntry->Eats   = SMMUV3_STREAM_TABLE_ENTRY_EATS_NOT_SUPPORTED;
  StreamEntry->S2Vmid = SMMUV3_STREAM_TABLE_ENTRY_S2VMID;             // Choose a non-zero value
  StreamEntry->S2Tg   = SMMUV3_STREAM_TABLE_ENTRY_S2TG_4KB;
  StreamEntry->S2Aa64 = 1;                                                                                // AArch64 S2 translation tables
  StreamEntry->S2Ttb  = (UINT64)(UINTN)SmmuInfo->PageTableRoot >> SMMUV3_STREAM_TABLE_ENTRY_S2TTB_OFFSET; // Page table root address
  if ((Idr0.S1p == 1) && (Idr0.S2p == 1)) {
    StreamEntry->S2Ptw = SMMUV3_STREAM_TABLE_ENTRY_S2PTW;
  }

  //
  // Set the maximum output address width. Per SMMUv3.2 spec (sections 5.2 and
  // 3.4.1), the maximum input address width with AArch64 format is given by
  // SMMU_IDR5.OAS field and capped at:
  // - 48 bits in SMMUv3.0,
  // - 52 bits in SMMUv3.1+. However, an address greater than 48 bits can
  //   only be output from stage 2 when a 64KB translation granule is in use
  //   for that translation table, which is not currently supported (only 4KB
  //   granules).
  //
  //  Thus the maximum input address width is restricted to 48-bits even if
  //  it is advertised to be larger.
  //
  SmmuInfo->OutputAddressWidth = SmmuV3DecodeAddressWidth (Idr5.Oas);

  if (SmmuInfo->OutputAddressWidth < SMMUV3_STREAM_TABLE_ENTRY_OUTPUT_ADDRESS_MAX) {
    StreamEntry->S2Ps = SmmuV3EncodeAddressWidth (SmmuInfo->OutputAddressWidth);
  } else {
    DEBUG ((DEBUG_INFO, "%a: Advertised OutputAddressWidth >= 48. Capping the width to 48 per the SMMU spec.\n", __func__));
    StreamEntry->S2Ps            = SmmuV3EncodeAddressWidth (SMMUV3_STREAM_TABLE_ENTRY_OUTPUT_ADDRESS_MAX);
    SmmuInfo->OutputAddressWidth = SMMUV3_STREAM_TABLE_ENTRY_OUTPUT_ADDRESS_MAX;
  }

  Status = SmmuV3SetTranslationStartingLevel (SmmuInfo, SmmuInfo->OutputAddressWidth, &S2Sl0);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to set translation starting level\n", __func__));
    return Status;
  }

  // S2SL0      Meaning
  // <https://developer.arm.com/documentation/ddi0595/2021-03/AArch64-Registers/VTCR-EL2--Virtualization-Translation-Control-Register?lang=en#fieldset_0-7_6-1>
  // Starting level of the stage 2 translation lookup, controlled by VTCR_EL2. The meaning of this field depends on the value of VTCR_EL2.TG0.
  // 0x2:
  // If VTCR_EL2.TG0 is 0b00 (4KB granule):
  // If FEAT_LPA2 is not implemented, start at level 0.
  // If FEAT_LPA2 is implemented and VTCR_EL2.SL2 is 0b0, start at level 0.
  // If FEAT_LPA2 is implemented, the combination of VTCR_EL2.SL0 == 10 and VTCR_EL2.SL2 == 1 is reserved.
  // If VTCR_EL2.TG0 is 0b10 (16KB granule) or 0b01 (64KB granule), start at level 1.
  //
  StreamEntry->S2Sl0 = S2Sl0;

  InputSize           = SmmuInfo->OutputAddressWidth;
  StreamEntry->S2T0Sz = 64 - InputSize;

  /**
    If Platform configures cohac ovveride, coherent translation table walks,
    then update the attributes as:
    - Inner/Outer cacheability -> Write-back-cacheable (WBC),
              Read-Allocate (RA), Write-Allocate (WA)
    - Shareability -> Inner-shareable.

    Otherwise, the default attributes (set above) apply:
    - Inner/Outer cacheability -> Non-cacheable (0x0),
    - Shareability -> Non-shareable (0x0).
  **/
  if (IortCohac != 0) {
    StreamEntry->S2Ir0 = ARM64_RGNCACHEATTR_WRITEBACK_WRITEALLOCATE;
    StreamEntry->S2Or0 = ARM64_RGNCACHEATTR_WRITEBACK_WRITEALLOCATE;
    StreamEntry->S2Sh0 = ARM64_SHATTR_INNER_SHAREABLE;
  } else {
    StreamEntry->S2Ir0 = ARM64_RGNCACHEATTR_NONCACHEABLE;
    StreamEntry->S2Or0 = ARM64_RGNCACHEATTR_NONCACHEABLE;
    StreamEntry->S2Sh0 = ARM64_SHATTR_OUTER_SHAREABLE;
  }

  StreamEntry->S2Rs = SMMUV3_STREAM_TABLE_ENTRY_S2RS_RECORD_FAULTS;   // record faults

  if (Idr1.AttrTypesOvr != 0) {
    StreamEntry->ShCfg = SMMUV3_STREAM_TABLE_ENTRY_SHCFG_INCOMING_SHAREABILITY; // incoming shareability attribute
  }

  // If the device requires memory attribute overrides, then hard-code it to
  // Inner+Outer write-back cached and Inner-shareable (IWB-OWB-ISH) as
  // given by the IORT spec.
  if ((Idr1.AttrTypesOvr != 0) && ((CCA == 1) && (CPM == 1) && (DACS == 0))) {
    StreamEntry->Mtcfg   = SMMUV3_STREAM_TABLE_ENTRY_MTCFG;
    StreamEntry->MemAttr = SMMUV3_STREAM_TABLE_ENTRY_MEMATTR_INNER_OUTTER_WRITEBACK_CACHED; // Inner+Outer write-back cached
    StreamEntry->ShCfg   = SMMUV3_STREAM_TABLE_ENTRY_SHCFG_INNER_SHAREABLE;                 // Inner shareable
  }

  StreamEntry->Valid = SMMUV3_STREAM_TABLE_ENTRY_VALID;

  return Status;
}

/**
  Allocate a linear or 2-Level stream table for SMMUv3.

  For allocating a 2-level or linear stream table, the stream table alignment
  requirements per SMMUv3 spec:
  - For 2-level table, the table needs to be aligned to the larger of L1
    table size or 64 bytes.
  - For linear table, the table needs to be aligned to its size.

  @param [in]  SmmuInfo             Pointer to the SMMU_INFO structure.
  @param [in]  TwoLevelStreamTable  Flag to indicate if a two-level stream table is used.
  @param [out] Log2Size             Pointer to store the log2 size of the stream table.
  @param [out] Size                 Pointer to store the size of the stream table.

  @retval Pointer to the allocated stream table, or NULL on failure.
**/
STATIC
VOID *
SmmuV3AllocateStreamTable (
  IN SMMU_INFO  *SmmuInfo,
  IN BOOLEAN    TwoLevelStreamTable,
  OUT UINT32    *Log2Size,
  OUT UINT32    *Size
  )
{
  UINT32  MaxStreamId;
  UINT32  SidMsb;
  UINT32  L1Bits;
  UINT32  Alignment;
  UINTN   Pages;
  VOID    *AllocatedAddress;

  if ((SmmuInfo == NULL) || (Log2Size == NULL) || (Size == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return NULL;
  }

  // The max stream id is calculated as the output base + the number of stream ids
  MaxStreamId = SmmuInfo->StreamTableEntryMax;
  if (TwoLevelStreamTable && (MaxStreamId < (EFI_PAGE_SIZE / sizeof (SMMUV3_STREAM_TABLE_ENTRY)))) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid MaxStreamId for 2-Level table%u\n", __func__, MaxStreamId));
    return NULL;
  }

  SidMsb    = HighBitSet32 (MaxStreamId);
  *Log2Size = SidMsb + 1;
  *Size     = SMMUV3_LINEAR_STREAM_TABLE_SIZE_FROM_LOG2 (*Log2Size);
  if (TwoLevelStreamTable) {
    L1Bits = *Log2Size - SMMUV3_STR_TAB_BASE_CFG_SPLIT; // L1 table log2 size
    *Size  = SMMUV3_L1_STREAM_TABLE_SIZE_FROM_LOG2 (L1Bits);
  }

  *Size     = ALIGN_VALUE (*Size, EFI_PAGE_SIZE);
  Alignment = *Size;        // Aligned to the size of the table, linear stream table
  Pages     = EFI_SIZE_TO_PAGES (*Size);

  AllocatedAddress = AllocateAlignedPages (Pages, Alignment);
  if (AllocatedAddress == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Allocation failed for stream table\n", __func__));
    return NULL;
  }

  ZeroMem (AllocatedAddress, *Size);
  return AllocatedAddress;
}

/**
  Free the allocated stream table for SMMUv3.

  @param [in] StreamTablePtr  Pointer to the stream table entry.
  @param [in] Size            Size of the stream table.
**/
STATIC
VOID
SmmuV3FreeStreamTable (
  IN VOID    *StreamTablePtr,
  IN UINT32  Size
  )
{
  UINTN  Pages;

  if ((StreamTablePtr == NULL) || (Size == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return;
  }

  Pages = EFI_SIZE_TO_PAGES (Size);
  FreeAlignedPages ((VOID *)StreamTablePtr, Pages);
}

/**
  Overwrite a single STE with a bypass configuration and invalidate the
  cached copy in the SMMU.

  Handles both linear and 2-level stream tables.  For 2-level tables
  where all L1 entries point to the same shared L2 page, a private L2
  page is allocated so that only the target StreamId is affected.

  @param [in] SmmuInfo   Pointer to the SMMU_INFO for the owning SMMU.
  @param [in] StreamId   Hardware stream ID to set to bypass mode.

  @retval EFI_SUCCESS            STE written and invalidated.
  @retval EFI_INVALID_PARAMETER  SmmuInfo is NULL or StreamId out of range.
  @retval EFI_OUT_OF_RESOURCES   L2 page allocation failed.
  @retval Others                 Command queue error.
**/
EFI_STATUS
SmmuV3WriteBypassSte (
  IN SMMU_INFO  *SmmuInfo,
  IN UINT32     StreamId
  )
{
  SMMUV3_STREAM_TABLE_ENTRY          *Ste;
  SMMUV3_STREAM_TABLE_ENTRY          BypassEntry;
  SMMUV3_STREAM_TABLE_ENTRY          OldEntry;
  SMMUV3_CMD_GENERIC                 Command;
  EFI_STATUS                         Status;
  BOOLEAN                            TwoLevel;
  BOOLEAN                            NewL2Allocated;
  SMMUV3_L1_STREAM_TABLE_DESCRIPTOR  *L1Table;
  SMMUV3_STREAM_TABLE_ENTRY          *L2Page;
  SMMUV3_STREAM_TABLE_ENTRY          *SharedL2Page;
  UINT32                             L1Index;
  UINT32                             L2Index;

  if ((SmmuInfo == NULL) || (StreamId > SmmuInfo->StreamTableEntryMax)) {
    return EFI_INVALID_PARAMETER;
  }

  TwoLevel = (SmmuInfo->StreamTableEntryMax >= (EFI_PAGE_SIZE / sizeof (SMMUV3_STREAM_TABLE_ENTRY)));

  ZeroMem (&BypassEntry, sizeof (BypassEntry));
  BypassEntry.Valid  = 1;
  BypassEntry.Config = StreamEntryConfigS1BypassS2Bypass;

  if (TwoLevel) {
    L1Table = (SMMUV3_L1_STREAM_TABLE_DESCRIPTOR *)SmmuInfo->StreamTable;
    L1Index = StreamId >> SMMUV3_STR_TAB_BASE_CFG_SPLIT;
    L2Index = StreamId & ((1U << SMMUV3_STR_TAB_BASE_CFG_SPLIT) - 1);

    if (L1Index >= (1U << (SmmuInfo->StreamTableLog2Size - SMMUV3_STR_TAB_BASE_CFG_SPLIT))) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: L1Index=%u exceeds L1 table size for StreamId=0x%x\n",
        __func__,
        L1Index,
        StreamId
        ));
      return EFI_INVALID_PARAMETER;
    }

    SharedL2Page = (SMMUV3_STREAM_TABLE_ENTRY *)(UINTN)(L1Table[L1Index].L2Ptr << SMMUV3_STR_TAB_BASE_L2_PTR_OFFSET);

    if (SharedL2Page == SmmuInfo->SharedL2Page) {
      L2Page = (SMMUV3_STREAM_TABLE_ENTRY *)AllocatePages (1);
      if (L2Page == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewL2Allocated = TRUE;
      CopyMem (L2Page, SharedL2Page, EFI_PAGE_SIZE);
    } else {
      NewL2Allocated = FALSE;
      L2Page         = SharedL2Page;
    }

    CopyMem (&OldEntry, &L2Page[L2Index], sizeof (SMMUV3_STREAM_TABLE_ENTRY));
    CopyMem (&L2Page[L2Index], &BypassEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));

    L1Table[L1Index].L2Ptr = (UINT64)(UINTN)L2Page >> SMMUV3_STR_TAB_BASE_L2_PTR_OFFSET;
  } else {
    NewL2Allocated = FALSE;
    Ste            = (SMMUV3_STREAM_TABLE_ENTRY *)SmmuInfo->StreamTable;
    CopyMem (&OldEntry, &Ste[StreamId], sizeof (SMMUV3_STREAM_TABLE_ENTRY));
    CopyMem (&Ste[StreamId], &BypassEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));
  }

  ArmDataSynchronizationBarrier ();

  SMMUV3_BUILD_CMD_CFGI_STE (&Command, StreamId, 1);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CFGI_STE failed for StreamId %u\n", __func__, StreamId));
    goto RestoreL1;
  }

  SMMUV3_BUILD_CMD_SYNC_NO_INTERRUPT (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CMD_SYNC failed\n", __func__));
    goto RestoreL1;
  }

  DEBUG ((
    DEBUG_INFO,
    "%a: SMMU 0x%llx StreamId %u set to BYPASS\n",
    __func__,
    SmmuInfo->SmmuBase,
    StreamId
    ));

  return EFI_SUCCESS;

RestoreL1:
  if (TwoLevel) {
    if (NewL2Allocated) {
      L1Table[L1Index].L2Ptr = (UINT64)(UINTN)SharedL2Page >> SMMUV3_STR_TAB_BASE_L2_PTR_OFFSET;
      FreePages (L2Page, 1);
    } else {
      CopyMem (&L2Page[L2Index], &OldEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));
    }
  } else {
    CopyMem (&Ste[StreamId], &OldEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));
  }

  return Status;
}

/**
  Configure the SMMUv3 based on the provided configuration per the SmmuV3 specification.
  Main configuration function for smmu hardware. Creates and enables a stream table, page table,
  event queue, and command queue. Enables stage 2 translation and dma remapping.

  <https://developer.arm.com/documentation/109242/0100/Programming-the-SMMU/Minimum-configuration>
  <https://developer.arm.com/documentation/ihi0070/latest/>

  @param [in] SmmuInfo        Pointer to the SMMU_INFO structure.
  @param [in] PageTableRoot   Pointer to the page table root.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES   Out of resources.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_DEVICE_ERROR       Device error.
  @retval Others                 Failure.
**/
STATIC
EFI_STATUS
SmmuV3Configure (
  IN SMMU_INFO   *SmmuInfo,
  IN PAGE_TABLE  *PageTableRoot
  )
{
  EFI_STATUS                         Status;
  UINT32                             Index;
  UINT32                             StreamTableLog2Size;
  UINT32                             StreamTableSize;
  UINT32                             CommandQueueLog2Size;
  UINT32                             EventQueueLog2Size;
  UINT8                              ReadWriteAllocationHint;
  SMMUV3_STRTAB_BASE                 StrTabBase;
  SMMUV3_STRTAB_BASE_CFG             StrTabBaseCfg;
  VOID                               *StreamTablePtr;
  SMMUV3_STREAM_TABLE_ENTRY          *StreamTableEntryPtr;
  SMMUV3_STREAM_TABLE_ENTRY          *L2StreamTablePtr;
  SMMUV3_L1_STREAM_TABLE_DESCRIPTOR  *L1Table;
  SMMUV3_STREAM_TABLE_ENTRY          TemplateEntry;
  SMMUV3_CMDQ_BASE                   CommandQueueBase;
  SMMUV3_EVENTQ_BASE                 EventQueueBase;
  SMMUV3_CR0                         Cr0;
  SMMUV3_CR1                         Cr1;
  SMMUV3_CR2                         Cr2;
  SMMUV3_IDR0                        Idr0;
  SMMUV3_IDR3                        Idr3;
  SMMUV3_CMD_GENERIC                 Command;
  SMMUV3_GERROR                      GError;
  VOID                               *CommandQueue;
  VOID                               *EventQueue;
  BOOLEAN                            TwoLevelStreamTable;

  if ((SmmuInfo == NULL) || (PageTableRoot == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  L2StreamTablePtr = NULL;

  // Set ReadWriteAllocationHint based on the COHAC_OVERRIDE flag.
  // These hints are applied to the allocated Stream Table, Command Queue, and Event Queue.
  if ((SmmuInfo->Flags & EFI_ACPI_IORT_SMMUv3_FLAG_COHAC_OVERRIDE) != 0) {
    ReadWriteAllocationHint = 0x1;
  } else {
    ReadWriteAllocationHint = 0x0;
  }

  // Disable SMMU before configuring
  Status = SmmuV3DisableTranslation (SmmuInfo->SmmuBase);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error disabling translation\n", __func__));
    goto End;
  }

  Status = SmmuV3DisableInterrupts (SmmuInfo->SmmuBase, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error disabling interrupts\n", __func__));
    goto End;
  }

  TwoLevelStreamTable = (SmmuInfo->StreamTableEntryMax >= (EFI_PAGE_SIZE / sizeof (SMMUV3_STREAM_TABLE_ENTRY)));
  StreamTablePtr      = SmmuV3AllocateStreamTable (SmmuInfo, TwoLevelStreamTable, &StreamTableLog2Size, &StreamTableSize);
  if (StreamTablePtr == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Error allocating stream table\n", __func__));
    Status = EFI_OUT_OF_RESOURCES;
    goto End;
  }

  SmmuInfo->StreamTable         = StreamTablePtr;
  SmmuInfo->StreamTableSize     = StreamTableSize;
  SmmuInfo->StreamTableLog2Size = StreamTableLog2Size;

  SmmuInfo->PageTableRoot = PageTableRoot;
  if (SmmuInfo->PageTableRoot == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Error initializing Page Table\n", __func__));
    Status = EFI_OUT_OF_RESOURCES;
    goto End;
  }

  Status = SmmuV3AddRMRMapping (SmmuInfo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error adding RMR mapping\n", __func__));
    goto End;
  }

  // Load default STE values
  Status = SmmuV3BuildStreamTableEntry (SmmuInfo, &TemplateEntry);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error building stream table entry\n", __func__));
    goto End;
  }

  if (TwoLevelStreamTable) {
    L2StreamTablePtr = (SMMUV3_STREAM_TABLE_ENTRY *)AllocatePages (1);
    if (L2StreamTablePtr == NULL) {
      DEBUG ((DEBUG_ERROR, "%a: Error allocating L2 stream table\n", __func__));
      Status = EFI_OUT_OF_RESOURCES;
      goto End;
    }

    ZeroMem (L2StreamTablePtr, EFI_PAGE_SIZE);

    for (Index = 0; Index < (EFI_PAGE_SIZE / sizeof (SMMUV3_STREAM_TABLE_ENTRY)); Index++) {
      CopyMem (&L2StreamTablePtr[Index], &TemplateEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));
    }

    SmmuInfo->SharedL2Page = L2StreamTablePtr;

    L1Table = (SMMUV3_L1_STREAM_TABLE_DESCRIPTOR *)StreamTablePtr;
    for (Index = 0; Index < (SMMUV3_L1_STREAM_TABLE_SIZE_FROM_LOG2 (StreamTableLog2Size - SMMUV3_STR_TAB_BASE_CFG_SPLIT) / sizeof (UINT64)); Index++) {
      L1Table[Index].L2Ptr = (UINT64)(UINTN)L2StreamTablePtr >> SMMUV3_STR_TAB_BASE_L2_PTR_OFFSET;
      // Per SmmuV3 spec: Span must be within the range of 0 to (SMMU_STRTAB_BASE_CFG.SPLIT + 1)
      // That is it must stay within the bounds of the Stream table split point.
      // Cannot have Span of 0, means invalid L2 table ptr in the L1 table entry.
      L1Table[Index].Span = SMMUV3_STR_TAB_BASE_CFG_SPLIT + 1;
    }
  } else {
    StreamTableEntryPtr = (SMMUV3_STREAM_TABLE_ENTRY *)StreamTablePtr;
    for (Index = 0; Index <= SmmuInfo->StreamTableEntryMax; Index++) {
      CopyMem (&StreamTableEntryPtr[Index], &TemplateEntry, sizeof (SMMUV3_STREAM_TABLE_ENTRY));
    }
  }

  Status = SmmuV3AllocateCommandQueue (SmmuInfo, &CommandQueueLog2Size, &CommandQueue);
  if (EFI_ERROR (Status) || (CommandQueue == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Error allocating SMMU Command Queue\n", __func__));
    goto End;
  }

  Status = SmmuV3AllocateEventQueue (SmmuInfo, &EventQueueLog2Size, &EventQueue);
  if (EFI_ERROR (Status) || (EventQueue == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Error allocating SMMU Event Queue\n", __func__));
    goto End;
  }

  SmmuInfo->CommandQueue         = CommandQueue;
  SmmuInfo->CommandQueueLog2Size = CommandQueueLog2Size;
  SmmuInfo->EventQueue           = EventQueue;
  SmmuInfo->EventQueueLog2Size   = EventQueueLog2Size;

  // Configure Stream Table Base
  StrTabBaseCfg.AsUINT32 = 0;
  StrTabBaseCfg.Fmt      = SMMUV3_STR_TAB_BASE_CFG_FMT_LINEAR; // Linear format
  if (TwoLevelStreamTable) {
    StrTabBaseCfg.Fmt   = SMMUV3_STR_TAB_BASE_CFG_FMT_2LEVEL; // 2-Level format
    StrTabBaseCfg.Split = SMMUV3_STR_TAB_BASE_CFG_SPLIT;
  }

  StrTabBaseCfg.Log2Size = StreamTableLog2Size;

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_STRTAB_BASE_CFG, StrTabBaseCfg.AsUINT32);

  StrTabBase.AsUINT64 = 0;
  StrTabBase.Ra       = ReadWriteAllocationHint;
  StrTabBase.Addr     = ((UINT64)(UINTN)SmmuInfo->StreamTable) >> SMMUV3_STR_TAB_BASE_ADDR_OFFSET;
  SmmuV3WriteRegister64 (SmmuInfo->SmmuBase, SMMU_STRTAB_BASE, StrTabBase.AsUINT64);

  // Configure Command Queue Base
  CommandQueueBase.AsUINT64 = 0;
  CommandQueueBase.Log2Size = SmmuInfo->CommandQueueLog2Size;
  CommandQueueBase.Addr     = ((UINT64)(UINTN)SmmuInfo->CommandQueue) >> SMMUV3_STR_TAB_BASE_CMDQ_OFFSET;
  CommandQueueBase.Ra       = ReadWriteAllocationHint;
  SmmuV3WriteRegister64 (SmmuInfo->SmmuBase, SMMU_CMDQ_BASE, CommandQueueBase.AsUINT64);
  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CMDQ_PROD, 0);
  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CMDQ_CONS, 0);
  SmmuInfo->CachedConsumer = 0;
  SmmuInfo->CachedProducer = 0;

  // Configure Event Queue Base
  EventQueueBase.AsUINT64 = 0;
  EventQueueBase.Log2Size = SmmuInfo->EventQueueLog2Size;
  EventQueueBase.Addr     = ((UINT64)(UINTN)SmmuInfo->EventQueue) >> SMMUV3_STR_TAB_BASE_EVENTQ_OFFSET;
  EventQueueBase.Wa       = ReadWriteAllocationHint;
  SmmuV3WriteRegister64 (SmmuInfo->SmmuBase, SMMU_EVENTQ_BASE, EventQueueBase.AsUINT64);
  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase + SMMUV3_PAGE_1_OFFSET, SMMU_EVENTQ_PROD, 0);
  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase + SMMUV3_PAGE_1_OFFSET, SMMU_EVENTQ_CONS, 0);

  // Check if Range-based invalidation and level hint are supported.
  Idr3.AsUINT32                        = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR3);
  SmmuInfo->RangeInvalidationSupported = (Idr3.Ril != 0);

  // Enable GError and event interrupts
  Status = SmmuV3EnableInterrupts (SmmuInfo->SmmuBase);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error enabling interrupts\n", __func__));
    goto End;
  }

  // Configure CR1
  Cr1.AsUINT32  = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CR1);
  Cr1.AsUINT32 &= ~SMMUV3_CR1_VALID_MASK;
  if ((SmmuInfo->Flags & EFI_ACPI_IORT_SMMUv3_FLAG_COHAC_OVERRIDE) != 0) {
    Cr1.QueueIc = ARM64_RGNCACHEATTR_WRITEBACK_WRITEALLOCATE; // WBC
    Cr1.QueueOc = ARM64_RGNCACHEATTR_WRITEBACK_WRITEALLOCATE; // WBC
    Cr1.QueueSh = ARM64_SHATTR_INNER_SHAREABLE;               // Inner-shareable
  }

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CR1, Cr1.AsUINT32);

  // Configure CR2
  Cr2.AsUINT32  = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CR2);
  Cr2.AsUINT32 &= ~SMMUV3_CR2_VALID_MASK;
  Cr2.E2h       = SMMUV3_CR2_E2H;
  Cr2.RecInvSid = SMMUV3_CR2_REC_INV_SID;   // Record C_BAD_STREAMID for invalid input streams.

  //
  // If broadcast TLB maintenance (BTM) is not enabled, then configure
  // private TLB maintenance (PTM). Per SMMU spec (section 6.3.12), the PTM bit is
  // only valid when BTM is indicated as supported.
  //
  Idr0.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR0);
  if (Idr0.Btm == 1) {
    Cr2.Ptm = SMMUV3_CR2_PTM;     // Private TLB maintenance.
  }

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CR2, Cr2.AsUINT32);

  // Configure CR0 part1
  ArmDataSynchronizationBarrier ();  // DSB

  Cr0.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CR0);
  Cr0.EventQEn = SMMUV3_CR0_EVENTQ_EN;
  Cr0.CmdQEn   = SMMUV3_CR0_CMDQ_EN;

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CR0, Cr0.AsUINT32);
  Status = SmmuV3Poll (SmmuInfo->SmmuBase, SMMU_CR0ACK, 0xC, 0xC);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error polling register: 0x%lx\n", __func__, SmmuInfo->SmmuBase + SMMU_CR0ACK));
    goto End;
  }

  //
  // Invalidate all cached configuration and TLB entries
  //
  SMMUV3_BUILD_CMD_CFGI_ALL (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error sending command.\n", __func__));
    goto End;
  }

  SMMUV3_BUILD_CMD_TLBI_NSNH_ALL (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error sending command.\n", __func__));
    goto End;
  }

  SMMUV3_BUILD_CMD_TLBI_EL2_ALL (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error sending command.\n", __func__));
    goto End;
  }

  // Issue a CMD_SYNC command to guarantee that any previously issued TLB
  // invalidations (CMD_TLBI_*) are completed (SMMUv3.2 spec section 4.6.3).
  SMMUV3_BUILD_CMD_SYNC_NO_INTERRUPT (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error sending command.\n", __func__));
    goto End;
  }

  // Configure CR0 part2
  Cr0.AsUINT32  = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CR0);
  Cr0.AsUINT32  = Cr0.AsUINT32 & ~SMMUV3_CR0_VALID_MASK;
  Cr0.SmmuEn    = SMMUV3_CR0_SMMU_EN;
  Cr0.EventQEn  = SMMUV3_CR0_EVENTQ_EN;
  Cr0.CmdQEn    = SMMUV3_CR0_CMDQ_EN;
  Cr0.PriQEn    = SMMUV3_CR0_PRIQ_EN_DISABLED;
  Cr0.Vmw       = SMMUV3_CR0_VMW_DISABLED; // Disable VMID wildcard matching.
  Idr0.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_IDR0);
  if (Idr0.Ats != 0) {
    Cr0.AtsChk = SMMUV3_CR0_ATS_CHK_DISABLE;     // disable bypass for ATS translated traffic.
  }

  ArmDataSynchronizationBarrier ();  // DSB

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CR0, Cr0.AsUINT32);
  Status = SmmuV3Poll (SmmuInfo->SmmuBase, SMMU_CR0ACK, SMMUV3_CR0_SMMU_EN_MASK, SMMUV3_CR0_SMMU_EN_MASK);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error polling register: 0x%lx\n", __func__, SmmuInfo->SmmuBase + SMMU_CR0ACK));
    goto End;
  }

  ArmDataSynchronizationBarrier ();  // DSB

  GError.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_GERROR);
  if (GError.AsUINT32 != 0) {
    DEBUG ((DEBUG_ERROR, "%a: Globar SMMU Error detected: 0x%lx\n", __func__, GError.AsUINT32));
    Status = EFI_DEVICE_ERROR;
  }

End:
  if (EFI_ERROR (Status) && (L2StreamTablePtr != NULL)) {
    FreePages (L2StreamTablePtr, 1);
  }

  // Only logs errors if errors are found
  SmmuV3LogErrors (SmmuInfo);
  return Status;
}

/**
  Free all RMR_NODE_INFO linked-list nodes attached to every entry in a
  SMMU_INFO array.  Only the individually-allocated list nodes are freed;
  the SmmuInfoArray itself is left untouched so the caller can free it
  separately.

  @param [in]  SmmuInfoArray  The SMMU_INFO array whose RmrNodeList chains
                              will be drained.
  @param [in]  NodeCount      Number of entries in SmmuInfoArray.
**/
STATIC
VOID
FreeSmmuInfoRmrNodes (
  IN SMMU_INFO  *SmmuInfoArray,
  IN UINT32     NodeCount
  )
{
  UINT32      Index;
  LIST_ENTRY  *Entry;
  LIST_ENTRY  *Next;

  if (SmmuInfoArray == NULL) {
    return;
  }

  for (Index = 0; Index < NodeCount; Index++) {
    Entry = GetFirstNode (&SmmuInfoArray[Index].RmrNodeList);
    while (!IsNull (&SmmuInfoArray[Index].RmrNodeList, Entry)) {
      Next = GetNextNode (&SmmuInfoArray[Index].RmrNodeList, Entry);
      RemoveEntryList (Entry);
      FreePool (BASE_CR (Entry, RMR_NODE_INFO, Link));
      Entry = Next;
    }
  }
}

/**
  Populate SMMU_INFO array from SmmuConfigLib platform data.

  Allocates and fills the SMMU_INFO array from SMMU_PLATFORM_NODE
  entries and applies the disabled list and RMR data.

  @param [out]  SmmuInfoOut   Pointer to receive allocated SMMU_INFO array.
  @param [out]  SmmuCountOut  Number of entries.

  @retval EFI_SUCCESS            Data populated successfully.
  @retval EFI_NOT_FOUND          No SMMU nodes found.
  @retval EFI_OUT_OF_RESOURCES   Allocation failure.
**/
STATIC
EFI_STATUS
PopulateSmmuInfoFromPlatform (
  OUT SMMU_INFO  **SmmuInfoOut,
  OUT UINT32     *SmmuCountOut
  )
{
  EFI_STATUS          Status;
  SMMU_PLATFORM_NODE  *PlatformNodes;
  UINT32              NodeCount;
  UINT64              *DisabledBases;
  UINT32              DisabledCount;
  SMMU_PLATFORM_RMR   *RmrEntries;
  UINT32              RmrCount;
  SMMU_INFO           *SmmuInfoArray;
  UINT32              Index;
  UINT32              DisIdx;
  UINT32              RmrIdx;
  RMR_NODE_INFO       *RmrItem;

  Status = SmmuConfigGetNodes (&PlatformNodes, &NodeCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: SmmuConfigGetNodes failed: %r\n", __func__, Status));
    return Status;
  }

  SmmuInfoArray = AllocateZeroPool (NodeCount * sizeof (SMMU_INFO));
  if (SmmuInfoArray == NULL) {
    FreePool (PlatformNodes);
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < NodeCount; Index++) {
    SmmuInfoArray[Index].SmmuBase            = PlatformNodes[Index].SmmuBase;
    SmmuInfoArray[Index].Flags               = PlatformNodes[Index].Flags;
    SmmuInfoArray[Index].StreamTableEntryMax = PlatformNodes[Index].StreamTableEntryMax;
    SmmuInfoArray[Index].EBSBehaviorAbort    = TRUE;
    SmmuInfoArray[Index].Enabled             = TRUE;
    InitializeListHead (&SmmuInfoArray[Index].RmrNodeList);
  }

  FreePool (PlatformNodes);

  Status = SmmuConfigGetDisabledList (&DisabledBases, &DisabledCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: SmmuConfigGetDisabledList failed: %r\n", __func__, Status));
    FreePool (SmmuInfoArray);
    return Status;
  }

  if (DisabledCount > 0) {
    for (Index = 0; Index < NodeCount; Index++) {
      for (DisIdx = 0; DisIdx < DisabledCount; DisIdx++) {
        if (SmmuInfoArray[Index].SmmuBase == DisabledBases[DisIdx]) {
          SmmuInfoArray[Index].Enabled = FALSE;
          DEBUG ((DEBUG_ERROR, "%a: SMMU[%u] Base=0x%llx marked DISABLED\n", __func__, Index, SmmuInfoArray[Index].SmmuBase));
        }
      }
    }

    FreePool (DisabledBases);
  }

  Status = SmmuConfigGetRmrInfo (&RmrEntries, &RmrCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: SmmuConfigGetRmrInfo failed: %r\n", __func__, Status));
    FreePool (SmmuInfoArray);
    return Status;
  }

  DEBUG ((DEBUG_ERROR, "%a: SmmuConfigGetRmrInfo: RmrCount=%u\n", __func__, RmrCount));

  if (RmrCount > 0) {
    for (RmrIdx = 0; RmrIdx < RmrCount; RmrIdx++) {
      for (Index = 0; Index < NodeCount; Index++) {
        if (SmmuInfoArray[Index].SmmuBase == RmrEntries[RmrIdx].SmmuBase) {
          RmrItem = AllocateZeroPool (sizeof (RMR_NODE_INFO));
          if (RmrItem == NULL) {
            DEBUG ((DEBUG_ERROR, "%a: Failed to allocate RMR_NODE_INFO\n", __func__));
            FreePool (RmrEntries);
            FreeSmmuInfoRmrNodes (SmmuInfoArray, NodeCount);
            FreePool (SmmuInfoArray);
            return EFI_OUT_OF_RESOURCES;
          }

          RmrItem->BaseAddress = RmrEntries[RmrIdx].BaseAddress;
          RmrItem->Length      = RmrEntries[RmrIdx].Length;
          InsertTailList (&SmmuInfoArray[Index].RmrNodeList, &RmrItem->Link);
          break;
        }
      }
    }

    FreePool (RmrEntries);
  }

  *SmmuInfoOut  = SmmuInfoArray;
  *SmmuCountOut = NodeCount;
  return EFI_SUCCESS;
}

/**
  Initialize the IOMMU_CONFIG structure.


  @retval Pointer to the allocated IOMMU_CONFIG structure, or NULL on failure.
**/
EFI_STATUS
IoMmuConfigInit (
  OUT IOMMU_CONFIG  **IoMmu
  )
{
  *IoMmu = (IOMMU_CONFIG *)AllocateZeroPool (sizeof (IOMMU_CONFIG));
  if (*IoMmu == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to allocate IOMMU_CONFIG structure\n", __func__));
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Deinitialize and free the SMMU_INFO structure and everything inside.
  Also disables SMMU translation and sets global abort.

  @param [in]  Smmu    Pointer to the SMMU_INFO structure to deinitialize.
**/
STATIC
VOID
IoMmuDeInit (
  IN IOMMU_CONFIG  *IoMmu
  )
{
  EFI_STATUS  Status;
  UINT32      SmmuIndex;

  if (IoMmu == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: SMMU_INFO structure is NULL\n", __func__));
    return;
  }

  for (SmmuIndex = 0; SmmuIndex < IoMmu->SmmuCount; SmmuIndex++) {
    if (IoMmu->SmmuInfo[SmmuIndex].CommandQueue != NULL) {
      Status = SmmuV3DisableTranslation (IoMmu->SmmuInfo[SmmuIndex].SmmuBase);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to disable SMMUv3 translation 0x%llx\n", __func__, IoMmu->SmmuInfo[SmmuIndex].SmmuBase));
      }

      Status = SmmuV3GlobalAbort (IoMmu->SmmuInfo[SmmuIndex].SmmuBase);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to global abort SMMUv3 0x%llx\n", __func__, IoMmu->SmmuInfo[SmmuIndex].SmmuBase));
      }
    }

    if (IoMmu->SmmuInfo[SmmuIndex].PageTableRoot != NULL) {
      PAGE_TABLE  *FreedRoot;
      UINT32      PtIdx;

      FreedRoot = IoMmu->SmmuInfo[SmmuIndex].PageTableRoot;
      PageTableDeInit (0, FreedRoot);
      for (PtIdx = 0; PtIdx < IoMmu->SmmuCount; PtIdx++) {
        if (IoMmu->SmmuInfo[PtIdx].PageTableRoot == FreedRoot) {
          IoMmu->SmmuInfo[PtIdx].PageTableRoot = NULL;
        }
      }
    }

    if (IoMmu->SmmuInfo[SmmuIndex].SharedL2Page != NULL) {
      FreePages (IoMmu->SmmuInfo[SmmuIndex].SharedL2Page, 1);
      IoMmu->SmmuInfo[SmmuIndex].SharedL2Page = NULL;
    }

    if (IoMmu->SmmuInfo[SmmuIndex].StreamTable != NULL) {
      SmmuV3FreeStreamTable (IoMmu->SmmuInfo[SmmuIndex].StreamTable, IoMmu->SmmuInfo[SmmuIndex].StreamTableSize);
      IoMmu->SmmuInfo[SmmuIndex].StreamTable = NULL;
    }

    if (IoMmu->SmmuInfo[SmmuIndex].CommandQueue != NULL) {
      SmmuV3FreeQueue (IoMmu->SmmuInfo[SmmuIndex].CommandQueue, IoMmu->SmmuInfo[SmmuIndex].CommandQueueLog2Size, FALSE);
      IoMmu->SmmuInfo[SmmuIndex].CommandQueue = NULL;
    }

    if (IoMmu->SmmuInfo[SmmuIndex].EventQueue != NULL) {
      SmmuV3FreeQueue (IoMmu->SmmuInfo[SmmuIndex].EventQueue, IoMmu->SmmuInfo[SmmuIndex].EventQueueLog2Size, TRUE);
      IoMmu->SmmuInfo[SmmuIndex].EventQueue = NULL;
    }
  }

  IoMmuFreeBypassList ();
  FreeSmmuInfoRmrNodes (IoMmu->SmmuInfo, IoMmu->SmmuCount);
  if (IoMmu->SmmuInfo != NULL) {
    FreePool (IoMmu->SmmuInfo);
  }

  FreePool (IoMmu);
}

/**
  Disable SMMU translation and set SMMU to global bypass during ExitBootServices.

  @param [in] Event    The event that triggered this notification function.
  @param [in] Context  Pointer to the notification function's context.
**/
STATIC
VOID
SmmuV3ExitBootServices (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_STATUS  Status;
  EFI_TPL     OldTpl;
  UINT32      SmmuIndex;

  if (Event == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Event\n", __func__));
    ASSERT (Event != NULL);
    return;
  }

  if ((mIoMmu == NULL) || (mIoMmu->SmmuInfo == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: IOMMU_CONFIG/SMMU_INFO structure is NULL\n", __func__));
    ASSERT (mIoMmu != NULL);
    ASSERT (mIoMmu->SmmuInfo != NULL);
    return;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  for (SmmuIndex = 0; SmmuIndex < mIoMmu->SmmuCount; SmmuIndex++) {
    if (mIoMmu->SmmuInfo[SmmuIndex].Enabled) {
      Status = SmmuV3DisableTranslation (mIoMmu->SmmuInfo[SmmuIndex].SmmuBase);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to disable smmu 0x%llx translation.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
        ASSERT_EFI_ERROR (Status);
      }

      if (mIoMmu->SmmuInfo[SmmuIndex].EBSBehaviorAbort) {
        Status = SmmuV3GlobalAbort (mIoMmu->SmmuInfo[SmmuIndex].SmmuBase);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: Failed to global abort smmu 0x%llx.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
          ASSERT_EFI_ERROR (Status);
        }
      } else {
        Status = SmmuV3SetGlobalBypass (mIoMmu->SmmuInfo[SmmuIndex].SmmuBase);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a: Failed to set smmu 0x%llx global bypass.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
          ASSERT_EFI_ERROR (Status);
        }
      }
    }
  }

  gBS->RestoreTPL (OldTpl);
  gBS->CloseEvent (Event);
}

/**
  Entrypoint for SmmuDxe driver.
  Configures SMMUv3 hardware based on platform configuration data from SmmuConfigLib.
  Uses a linear stream table and stage 2 translation for dma remapping.
  Initializes IoMmu Protocol.

  @param [in] ImageHandle    The firmware allocated handle for the EFI image.
  @param [in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS               The entry point is executed successfully.
  @retval EFI_OUT_OF_RESOURCES      Not enough resources to initialize the driver.
  @retval EFI_NOT_FOUND             The SMMU configuration data is not found.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_TIMEOUT               Timeout.
  @retval EFI_DEVICE_ERROR          Device error.
  @retval Others                    Some error occurs when executing this entry point.
**/
EFI_STATUS
InitializeSmmuDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  UINT32      SmmuIndex;
  PAGE_TABLE  *PageTableRoot;

  Event         = NULL;
  PageTableRoot = NULL;

  Status = IoMmuConfigInit (&mIoMmu);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to initialize IoMmu Config\n", __func__));
    return Status;
  }

  Status = PopulateSmmuInfoFromPlatform (&mIoMmu->SmmuInfo, &mIoMmu->SmmuCount);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to get SMMU configuration from platform\n", __func__));
    goto Error;
  }

  DEBUG ((DEBUG_ERROR, "%a: Found %u SMMU(s) from platform config\n", __func__, mIoMmu->SmmuCount));

  // Global Page Table until TODO: IoMmu Protocol V2 is implemented
  PageTableRoot = PageTableInit ();
  if (PageTableRoot == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to initialize Page Table\n", __func__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  // Configure SMMUv3 hardware
  for (SmmuIndex = 0; SmmuIndex < mIoMmu->SmmuCount; SmmuIndex++) {
    if (mIoMmu->SmmuInfo[SmmuIndex].Enabled) {
      Status = SmmuV3Configure (&mIoMmu->SmmuInfo[SmmuIndex], PageTableRoot);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to configure SMMUv3 hardware\n", __func__));
        goto Error;
      }

      DEBUG ((DEBUG_INFO, "%a: SMMUv3 0x%llx is configured for Stage2 Translation\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
    }
  }

  // Disable any SMMU that is not enabled.
  for (SmmuIndex = 0; SmmuIndex < mIoMmu->SmmuCount; SmmuIndex++) {
    if (mIoMmu->SmmuInfo[SmmuIndex].Enabled == FALSE) {
      Status = SmmuV3DisableTranslation (mIoMmu->SmmuInfo[SmmuIndex].SmmuBase);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to disable smmu 0x%llx translation.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
        ASSERT_EFI_ERROR (Status);
      }

      Status = SmmuV3SetGlobalBypass (mIoMmu->SmmuInfo[SmmuIndex].SmmuBase);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to set smmu 0x%llx global bypass.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
        ASSERT_EFI_ERROR (Status);
      }

      DEBUG ((DEBUG_INFO, "%a: SMMUv3 0x%llx is disabled/global bypass.\n", __func__, mIoMmu->SmmuInfo[SmmuIndex].SmmuBase));
    }
  }

  // Create an event callback to disable SMMUv3 translation and set global abort during ExitBootServices
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SmmuV3ExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create ExitBootServices event\n", __func__));
    goto Error;
  }

  // Initialize IoMmu Protocol
  Status = IoMmuInit ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to install IoMmuProtocol\n", __func__));
    goto Error;
  }

  DEBUG ((DEBUG_INFO, "%a: Status = %llx\n", __func__, Status));

  return Status;

Error:
  DEBUG ((DEBUG_ERROR, "%a: SMMU DMA protection failed to initialize. Status = %llx\n", __func__, Status));
  if (PageTableRoot != NULL) {
    if ((mIoMmu != NULL) && (mIoMmu->SmmuInfo != NULL)) {
      for (SmmuIndex = 0; SmmuIndex < mIoMmu->SmmuCount; SmmuIndex++) {
        if (mIoMmu->SmmuInfo[SmmuIndex].PageTableRoot == PageTableRoot) {
          mIoMmu->SmmuInfo[SmmuIndex].PageTableRoot = NULL;
        }
      }
    }

    PageTableDeInit (0, PageTableRoot);
    PageTableRoot = NULL;
  }

  IoMmuDeInit (mIoMmu);
  mIoMmu = NULL;
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  return Status;
}
