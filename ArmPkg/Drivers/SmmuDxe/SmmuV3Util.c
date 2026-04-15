/** @file Smmuv3Util.c

    This file contains util functions for the SMMU driver.
    All functions are derived from the SMMU spec: <https://developer.arm.com/documentation/ihi0070/latest/>

    Copyright (c) Microsoft Corporation.
    Copyright (c) 2026, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
    SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include "SmmuV3.h"

/**
  Decode the address width from the given address size type.

  @param [in]  AddressSizeType  The address size type.

  @return The decoded address width. 0 if the address size type is invalid.
**/
UINT32
SmmuV3DecodeAddressWidth (
  IN UINT32  AddressSizeType
  )
{
  UINT32  Length;

  switch (AddressSizeType) {
    case SmmuAddressSize32Bit:
      Length = 32;
      break;
    case SmmuAddressSize36Bit:
      Length = 36;
      break;
    case SmmuAddressSize40Bit:
      Length = 40;
      break;
    case SmmuAddressSize42Bit:
      Length = 42;
      break;
    case SmmuAddressSize44Bit:
      Length = 44;
      break;
    case SmmuAddressSize48Bit:
      Length = 48;
      break;
    case SmmuAddressSize52Bit:
      Length = 52;
      break;
    default:
      DEBUG ((DEBUG_ERROR, "%a: Invalid Address Size Type: 0x%lx\n", __func__, AddressSizeType));
      Length = 0;
      break;
  }

  return Length;
}

/**
  Encode the address width to the corresponding address size type.

  @param [in]  AddressWidth  The address width.

  @return The encoded address size type. 0 if the address width is invalid.
**/
UINT8
SmmuV3EncodeAddressWidth (
  IN UINT32  AddressWidth
  )
{
  UINT8  Encoding;

  switch (AddressWidth) {
    case 32:
      Encoding = SmmuAddressSize32Bit;
      break;
    case 36:
      Encoding = SmmuAddressSize36Bit;
      break;
    case 40:
      Encoding = SmmuAddressSize40Bit;
      break;
    case 42:
      Encoding = SmmuAddressSize42Bit;
      break;
    case 44:
      Encoding = SmmuAddressSize44Bit;
      break;
    case 48:
      Encoding = SmmuAddressSize48Bit;
      break;
    case 52:
      Encoding = SmmuAddressSize52Bit;
      break;
    default:
      DEBUG ((DEBUG_ERROR, "%a: Invalid Address Width: 0x%lx\n", __func__, AddressWidth));
      Encoding = 0;
      break;
  }

  return Encoding;
}

/**
  Set the translation starting level for SMMUv3 page tables.
  Only 3 and 4 level paging are supported.

  @param [in]  SmmuInfo           Pointer to the SMMU_INFO structure.
  @param [in]  OutputAddressWidth  The output address width.
  @param [out] S2Sl0              The starting level for stage 2 translation.

  @retval EFI_SUCCESS              Success.
  @retval EFI_INVALID_PARAMETER    Invalid parameter.
**/
EFI_STATUS
SmmuV3SetTranslationStartingLevel (
  IN SMMU_INFO  *SmmuInfo,
  IN UINT32     OutputAddressWidth,
  OUT UINT64    *S2Sl0
  )
{
  if ((OutputAddressWidth > PAGE_TABLE_OUTPUT_ADDRESS_WIDTH_MAX) || (OutputAddressWidth < PAGE_TABLE_OUTPUT_ADDRESS_WIDTH_MIN)) {
    DEBUG ((DEBUG_ERROR, "%a: OutputAddressWidth %d not supported.\n", __func__, OutputAddressWidth));
    return EFI_INVALID_PARAMETER;
  }

  // Per the Arm ARM VMSA spec, >= 44 bits of address width requires 4 level paging.
  // Otherwise, 3 level paging is used.
  if (OutputAddressWidth >= PAGE_TABLE_4_LEVEL_OUTPUT_ADDRESS_WIDTH_MIN) {
    SmmuInfo->TranslationStartingLevel = 0; // 4 level paging
    *S2Sl0                             = 0x2;
  } else {
    SmmuInfo->TranslationStartingLevel = 1; // 3 level paging
    *S2Sl0                             = 0x1;
    // If the output address width is greater than PAGE_TABLE_CONCATENATED_PAGES_BITS_CUTOFF, the page table root must be concatenated.
    SmmuInfo->PageTableRootConcatenated = (OutputAddressWidth > PAGE_TABLE_CONCATENATED_PAGES_BITS_CUTOFF);
  }

  return EFI_SUCCESS;
}

/**
  Read a 32-bit value from the specified SMMU register.

  @param [in]  SmmuBase   The base address of the SMMU.
  @param [in]  Register   The offset of the register.

  @return The 32-bit value read from the register. 0 if the SMMU base address is invalid.
**/
UINT32
SmmuV3ReadRegister32 (
  IN UINT64  SmmuBase,
  IN UINT64  Register
  )
{
  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return 0;
  }

  return MmioRead32 (SmmuBase + Register);
}

/**
  Read a 64-bit value from the specified SMMU register.

  @param [in]  SmmuBase   The base address of the SMMU.
  @param [in]  Register   The offset of the register.

  @return The 64-bit value read from the register. 0 if the SMMU base address is invalid.
**/
UINT64
SmmuV3ReadRegister64 (
  IN UINT64  SmmuBase,
  IN UINT64  Register
  )
{
  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return 0;
  }

  return MmioRead64 (SmmuBase + Register);
}

/**
  Write a 32-bit value to the specified SMMU register.

  @param [in]  SmmuBase   The base address of the SMMU.
  @param [in]  Register   The offset of the register.
  @param [in]  Value      The 32-bit value to write.

  @return The 32-bit value written to the register, or 0 if the SMMU base address is invalid.
**/
UINT32
SmmuV3WriteRegister32 (
  IN UINT64  SmmuBase,
  IN UINT64  Register,
  IN UINT32  Value
  )
{
  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return 0;
  }

  return MmioWrite32 (SmmuBase + Register, Value);
}

/**
  Write a 64-bit value to the specified SMMU register.

  @param [in]  SmmuBase   The base address of the SMMU.
  @param [in]  Register   The offset of the register.
  @param [in]  Value      The 64-bit value to write.

  @return The 64-bit value written to the register, or 0 if the SMMU base address is invalid.
**/
UINT64
SmmuV3WriteRegister64 (
  IN UINT64  SmmuBase,
  IN UINT64  Register,
  IN UINT64  Value
  )
{
  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return 0;
  }

  return MmioWrite64 (SmmuBase + Register, Value);
}

/**
  Disable interrupts for the SMMUv3.

  @param [in]  SmmuBase          The base address of the SMMU.
  @param [in]  ClearStaleErrors  Whether to clear stale errors.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
  @retval EFI_TIMEOUT            Timeout.
**/
EFI_STATUS
SmmuV3DisableInterrupts (
  IN UINT64   SmmuBase,
  IN BOOLEAN  ClearStaleErrors
  )
{
  EFI_STATUS       Status;
  SMMUV3_IRQ_CTRL  IrqControl;
  SMMUV3_GERROR    GlobalErrors;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  IrqControl.AsUINT32 = SmmuV3ReadRegister32 (SmmuBase, SMMU_IRQ_CTRL);
  if ((IrqControl.AsUINT32 & SMMUV3_IRQ_CTRL_GLOBAL_PRIQ_EVTQ_EN_MASK) != 0) {
    IrqControl.AsUINT32 &= ~SMMUV3_IRQ_CTRL_GLOBAL_PRIQ_EVTQ_EN_MASK;
    SmmuV3WriteRegister32 (SmmuBase, SMMU_IRQ_CTRL, IrqControl.AsUINT32);
    Status = SmmuV3Poll (SmmuBase, SMMU_IRQ_CTRLACK, SMMUV3_IRQ_CTRL_GLOBAL_PRIQ_EVTQ_EN_MASK, 0);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "%a: Error polling register: 0x%lx\n", __func__, SmmuBase + SMMU_IRQ_CTRLACK));
      return Status;
    }
  }

  if (ClearStaleErrors != FALSE) {
    GlobalErrors.AsUINT32 = SmmuV3ReadRegister32 (SmmuBase, SMMU_GERROR);
    GlobalErrors.AsUINT32 = GlobalErrors.AsUINT32 & SMMUV3_GERROR_VALID_MASK;
    SmmuV3WriteRegister32 (SmmuBase, SMMU_GERROR, GlobalErrors.AsUINT32);
  }

  return EFI_SUCCESS;
}

/**
  Enable interrupts for the SMMUv3.

  @param [in]  SmmuBase  The base address of the SMMU.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
  @retval EFI_TIMEOUT            Timeout.
**/
EFI_STATUS
SmmuV3EnableInterrupts (
  IN UINT64  SmmuBase
  )
{
  EFI_STATUS       Status;
  SMMUV3_IRQ_CTRL  IrqControl;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  IrqControl.AsUINT32         = SmmuV3ReadRegister32 (SmmuBase, SMMU_IRQ_CTRL);
  IrqControl.AsUINT32        &= ~SMMUV3_IRQ_CTRL_GLOBAL_PRIQ_EVTQ_EN_MASK;
  IrqControl.GlobalErrorIrqEn = 1;
  IrqControl.EventqIrqEn      = 1;
  SmmuV3WriteRegister32 (SmmuBase, SMMU_IRQ_CTRL, IrqControl.AsUINT32);
  Status = SmmuV3Poll (SmmuBase, SMMU_IRQ_CTRLACK, 0x5, 0x5);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "%a: Error polling register: 0x%lx\n", __func__, SmmuBase + SMMU_IRQ_CTRLACK));
  }

  return Status;
}

/**
  Disable translation for the SMMUv3.

  @param [in]  SmmuBase  The base address of the SMMU.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
  @retval EFI_TIMEOUT            Timeout.
**/
EFI_STATUS
SmmuV3DisableTranslation (
  IN UINT64  SmmuBase
  )
{
  SMMUV3_CR0  Cr0;
  EFI_STATUS  Status;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Cr0.AsUINT32 = SmmuV3ReadRegister32 (SmmuBase, SMMU_CR0);
  if ((Cr0.AsUINT32 & SMMUV3_CR0_SMMU_CMDQ_EVTQ_PRIQ_EN_MASK) != 0) {
    Cr0.AsUINT32 = Cr0.AsUINT32 & ~SMMUV3_CR0_SMMU_CMDQ_EVTQ_PRIQ_EN_MASK;
    SmmuV3WriteRegister32 (SmmuBase, SMMU_CR0, Cr0.AsUINT32);
    Status = SmmuV3Poll (SmmuBase, SMMU_CR0ACK, SMMUV3_CR0_SMMU_CMDQ_EVTQ_PRIQ_EN_MASK, 0);
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "%a: Error polling register: 0x%lx\n", __func__, SmmuBase + SMMU_CR0ACK));
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Set the Smmu in ABORT mode and stop DMA.

  @param [in]  SmmuReg    Base address of the SMMUv3.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
  @retval EFI_TIMEOUT            Timeout.
**/
EFI_STATUS
SmmuV3GlobalAbort (
  IN  UINT64  SmmuBase
  )
{
  EFI_STATUS  Status;
  UINT32      RegVal;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  // Attribute update has completed when SMMU_(S)_GBPA.Update bit is 0.
  Status = SmmuV3Poll (SmmuBase, SMMU_GBPA, SMMU_GBPA_UPDATE, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // SMMU_(S)_CR0 resets to zero with all streams bypassing the SMMU,
  // so just abort all incoming transactions.
  RegVal = SmmuV3ReadRegister32 (SmmuBase, SMMU_GBPA);

  // Set the SMMU_GBPA.ABORT and SMMU_GBPA.UPDATE.
  RegVal |= (SMMU_GBPA_ABORT | SMMU_GBPA_UPDATE);

  SmmuV3WriteRegister32 (SmmuBase, SMMU_GBPA, RegVal);

  // Attribute update has completed when SMMU_(S)_GBPA.Update bit is 0.
  Status = SmmuV3Poll (SmmuBase, SMMU_GBPA, SMMU_GBPA_UPDATE, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Sanity check to see if abort is set
  Status = SmmuV3Poll (SmmuBase, SMMU_GBPA, SMMU_GBPA_ABORT, SMMU_GBPA_ABORT);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Set all streams to bypass the SMMU.

  @param [in]  SmmuReg    Base address of the SMMUv3.

  @retval EFI_SUCCESS            Success.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3SetGlobalBypass (
  IN UINT64  SmmuBase
  )
{
  EFI_STATUS  Status;
  UINT32      RegVal;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  // Attribute update has completed when SMMU_(S)_GBPA.Update bit is 0.
  Status = SmmuV3Poll (SmmuBase, SMMU_GBPA, SMMU_GBPA_UPDATE, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // SMMU_(S)_CR0 resets to zero with all streams bypassing the SMMU
  RegVal = SmmuV3ReadRegister32 (SmmuBase, SMMU_GBPA);

  // TF-A configures the SMMUv3 to abort all incoming transactions.
  // Clear the SMMU_GBPA.ABORT to allow Non-secure streams to bypass
  // the SMMU.
  RegVal &= ~SMMU_GBPA_ABORT;
  RegVal |= SMMU_GBPA_UPDATE;

  SmmuV3WriteRegister32 (SmmuBase, SMMU_GBPA, RegVal);

  Status = SmmuV3Poll (SmmuBase, SMMU_GBPA, SMMU_GBPA_UPDATE, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_ERROR, "%a: SMMU 0x%llx set to global bypass\n", __func__, SmmuBase));
  return EFI_SUCCESS;
}

/**
  Poll the SMMU register and test the value based on the mask.

  @param [in]  SmmuBase   Base address of the SMMU.
  @param [in]  SmmuReg    The SMMU register to poll.
  @param [in]  Mask       Mask of register bits to monitor.
  @param [in]  Value      Expected value.

  @retval EFI_SUCCESS            Success.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3Poll (
  IN UINT64  SmmuBase,
  IN UINT64  SmmuReg,
  IN UINT32  Mask,
  IN UINT32  Value
  )
{
  UINT32  RegVal;
  UINTN   Count;

  if (SmmuBase == 0) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid SMMU base address\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  // Set 0.1ms timeout value.
  Count = 10;
  do {
    RegVal = SmmuV3ReadRegister32 (SmmuBase, SmmuReg);
    if ((RegVal & Mask) == Value) {
      return EFI_SUCCESS;
    }

    MicroSecondDelay (10);
  } while ((--Count) > 0);

  DEBUG ((
    DEBUG_ERROR,
    "%a: Timeout polling SMMUv3 register @%p Read value 0x%x "
    "expected 0x%x\n",
    __func__,
    SmmuReg,
    RegVal,
    ((Value == 0) ? (RegVal & ~Mask) : (RegVal | Mask))
    ));

  return EFI_TIMEOUT;
}

/**
  Consume the event queue for errors and retrieve the fault record.
  Clears the outputted FaultRecord if the queue is empty.

  @param [in]  SmmuInfo     Pointer to the SMMU_INFO structure.
  @param [out] FaultRecord  Pointer to the fault record structure.
  @param [out] IsEmpty      Flag to indicate if the queue is empty.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3ConsumeEventQueueForErrors (
  IN SMMU_INFO             *SmmuInfo,
  OUT SMMUV3_FAULT_RECORD  *FaultRecord,
  OUT BOOLEAN              *IsEmpty
  )
{
  SMMUV3_EVENTQ_CONS   Consumer;
  UINT32               ConsumerIndex;
  UINT32               ConsumerWrap;
  SMMUV3_FAULT_RECORD  *NextFault;
  SMMUV3_EVENTQ_PROD   Producer;
  UINT32               ProducerIndex;
  UINT32               ProducerWrap;
  BOOLEAN              QueueEmpty;
  UINT32               QueueMask;
  UINT32               TotalQueueEntries;
  UINT32               WrapMask;

  if ((SmmuInfo == NULL) || ((FaultRecord == NULL) || (IsEmpty == NULL))) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  TotalQueueEntries = SMMUV3_COUNT_FROM_LOG2 (SmmuInfo->EventQueueLog2Size);
  WrapMask          = TotalQueueEntries;
  QueueMask         = TotalQueueEntries - 1;

  Producer.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase + SMMUV3_PAGE_1_OFFSET, SMMU_EVENTQ_PROD);
  Consumer.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase + SMMUV3_PAGE_1_OFFSET, SMMU_EVENTQ_CONS);

  ProducerIndex = Producer.WriteIndex & QueueMask;
  ProducerWrap  = Producer.WriteIndex & WrapMask;
  ConsumerIndex = Consumer.ReadIndex & QueueMask;
  ConsumerWrap  = Consumer.ReadIndex & WrapMask;
  QueueEmpty    = SMMUV3_IS_QUEUE_EMPTY (
                    ProducerIndex,
                    ProducerWrap,
                    ConsumerIndex,
                    ConsumerWrap
                    );

  if (QueueEmpty != FALSE) {
    *IsEmpty = TRUE;
    goto End;
  }

  *IsEmpty  = FALSE;
  NextFault = (SMMUV3_FAULT_RECORD *)SmmuInfo->EventQueue + ConsumerIndex;
  CopyMem (FaultRecord, NextFault, SMMUV3_EVENT_QUEUE_ENTRY_SIZE);

  ConsumerIndex += 1;
  if (ConsumerIndex == TotalQueueEntries) {
    ConsumerIndex = 0;
    ConsumerWrap  = ConsumerWrap ^ WrapMask;
  }

  Consumer.ReadIndex = ConsumerIndex | ConsumerWrap;

  ArmDataSynchronizationBarrier ();

  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase + SMMUV3_PAGE_1_OFFSET, SMMU_EVENTQ_CONS, Consumer.AsUINT32);

End:
  return EFI_SUCCESS;
}

/**
  Dump the page table entries for a given virtual address.
  Dumps PTE's for all levels regardless of the starting level chosen for translation.

  @param [in]  SmmuInfo        Pointer to the SMMU_INFO structure.
  @param [in]  VirtualAddress  The virtual address to dump.
  @param [in]  Root            Pointer to the root page table.

  @retval None.
**/
VOID
SmmuV3DumpPageTableEntries (
  IN SMMU_INFO   *SmmuInfo,
  IN UINT64      VirtualAddress,
  IN PAGE_TABLE  *Root
  )
{
  UINTN       Index;
  UINT8       Level;
  PAGE_TABLE  *Current;

  Current = Root;

  for (Level = SmmuInfo->TranslationStartingLevel; Level < PAGE_TABLE_DEPTH; Level++) {
    Index = PAGE_TABLE_INDEX (VirtualAddress, Level, SmmuInfo->OutputAddressWidth, SmmuInfo->TranslationStartingLevel, SmmuInfo->PageTableRootConcatenated);
    if (Current->Entries[Index] == 0) {
      DEBUG ((DEBUG_ERROR, "%a: Invalid entry at level %d, index %d\n", __func__, Level, Index));
      break;
    }

    DEBUG ((DEBUG_INFO, "%a: VirtualAddress = %llx Level = %d Current->Entries[%d] = 0x%llx\n", __func__, VirtualAddress, Level, Index, Current->Entries[Index]));
    Current = (PAGE_TABLE *)((UINTN)Current->Entries[Index] & ~0xFFF);
  }
}

/**
  Log the errors if found from the SMMUv3. Prints Event Queue entries and GError register.
  Does nothing if no errors found.

  @param [in]  SmmuInfo  Pointer to the SMMU_INFO structure.

  @retval EFI_SUCCESS            No SMMU errors found.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
  @retval EFI_DEVICE_ERROR       SMMU error found.
**/
EFI_STATUS
SmmuV3LogErrors (
  IN SMMU_INFO  *SmmuInfo
  )
{
  SMMUV3_GERROR        GError;
  SMMUV3_FAULT_RECORD  FaultRecord;
  UINTN                Index;
  BOOLEAN              IsEmpty;
  EFI_STATUS           Status;

  if (SmmuInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  do {
    // Only consumes one entry at a time, so we loop until empty
    Status = SmmuV3ConsumeEventQueueForErrors (SmmuInfo, &FaultRecord, &IsEmpty);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Error consuming event queue\n", __func__));
      return Status;
    }

    if (IsEmpty == FALSE) {
      Status = EFI_DEVICE_ERROR;
      DEBUG ((DEBUG_ERROR, "%a: %llx FaultRecord:\n", __func__, SmmuInfo->SmmuBase));
      for (Index = 0; Index < sizeof (FaultRecord.Fault) / sizeof (FaultRecord.Fault[0]); Index++) {
        DEBUG ((DEBUG_ERROR, "0x%llx\n", FaultRecord.Fault[Index]));
      }

      // Dump PTE's if translation related fault
      if (((FaultRecord.Fault[0] & 0xFF) == 0x10) || ((FaultRecord.Fault[0] & 0xFF) == 0x11) ||
          ((FaultRecord.Fault[0] & 0xFF) == 0x12) || ((FaultRecord.Fault[0] & 0xFF) == 0x13))
      {
        SmmuV3DumpPageTableEntries (SmmuInfo, FaultRecord.Fault[2], SmmuInfo->PageTableRoot);
      }
    }
  } while (IsEmpty == FALSE);

  GError.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_GERROR);
  if (GError.AsUINT32 != 0) {
    Status = EFI_DEVICE_ERROR;
    DEBUG ((DEBUG_ERROR, "%a: %llx GError: 0x%lx\n", __func__, SmmuInfo->SmmuBase, GError.AsUINT32));
  }

  return Status;
}

/**
  Write commands to the SMMUv3 command queue.

  @param [in]  SmmuInfo       Pointer to the SMMU_INFO structure.
  @param [in]  StartingIndex  The starting index in the command queue.
  @param [in]  CommandCount   The number of commands to write.
  @param [in]  Commands       Pointer to the commands to write.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
STATIC
EFI_STATUS
SmmuV3WriteCommands (
  IN SMMU_INFO           *SmmuInfo,
  IN UINT32              StartingIndex,
  IN UINT32              CommandCount,
  IN SMMUV3_CMD_GENERIC  *Commands
  )
{
  UINT32              Index;
  UINT32              ProducerIndex;
  UINT32              QueueMask;
  UINT32              WrapMask;
  SMMUV3_CMD_GENERIC  *CommandQueue;

  if ((SmmuInfo == NULL) || (Commands == NULL) || (CommandCount == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  WrapMask     = (1UL << SmmuInfo->CommandQueueLog2Size);
  QueueMask    = WrapMask - 1;
  CommandQueue = (SMMUV3_CMD_GENERIC *)SmmuInfo->CommandQueue;
  for (Index = 0; Index < CommandCount; Index += 1) {
    ProducerIndex               = (UINT32)((StartingIndex + Index) & QueueMask);
    CommandQueue[ProducerIndex] = Commands[Index];
  }

  // This DSB ensures that all commands written to the command queue before this point will be visible
  // before we update the producer index register to actually trigger processing of the commands.
  ArmDataSynchronizationBarrier ();

  return EFI_SUCCESS;
}

/**
  Update the cached consumer index for the SMMUv3 command queue.

  @param [in]  SmmuInfo            Pointer to the SMMU_INFO structure.
  @param [in]  QueueMask           The queue mask.
  @param [in]  WrapMask            The wrap mask.
  @param [in]  TotalQueueEntries   The total number of entries in the queue.
  @param [out] ConsumerIndexOut    Pointer to store the consumer index.
  @param [out] ConsumerWrapOut     Pointer to store the consumer wrap.
**/
VOID
SmmuV3CmdQueueUpdateCachedConsumer (
  IN  SMMU_INFO  *SmmuInfo,
  IN  UINT32     QueueMask,
  IN  UINT32     WrapMask,
  IN  UINT32     TotalQueueEntries,
  OUT UINT32     *ConsumerIndexOut,
  OUT UINT32     *ConsumerWrapOut
  )
{
  SMMUV3_CMDQ_CONS  Consumer;
  UINT32            ConsumerIndex;
  UINT32            ConsumerWrap;
  UINT64            CachedConsumerWrap;

  if ((SmmuInfo == NULL) || (ConsumerIndexOut == NULL) || (ConsumerWrapOut == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    ASSERT (FALSE);
    return;
  }

  Consumer.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CMDQ_CONS);
  ConsumerIndex     = Consumer.ReadIndex & QueueMask;
  ConsumerWrap      = Consumer.ReadIndex & WrapMask;
  *ConsumerIndexOut = ConsumerIndex;
  *ConsumerWrapOut  = ConsumerWrap;

  CachedConsumerWrap       = SmmuInfo->CachedConsumer & WrapMask;
  SmmuInfo->CachedConsumer = (SmmuInfo->CachedConsumer & ~QueueMask) | ConsumerIndex;

  if (CachedConsumerWrap != ConsumerWrap) {
    SmmuInfo->CachedConsumer += TotalQueueEntries;
  }
}

/**
  Send a SMMUV3_CMD_GENERIC command to the SMMUv3.

  @param [in]  SmmuInfo  Pointer to the SMMU_INFO structure.
  @param [in]  Command   Pointer to the command to send.

  @retval EFI_SUCCESS            Success.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3SendCommand (
  IN SMMU_INFO           *SmmuInfo,
  IN SMMUV3_CMD_GENERIC  *Command
  )
{
  UINT32            QueueMask;
  UINT32            WrapMask;
  UINT32            TotalQueueEntries;
  UINT32            ProducerIndex;
  UINT32            ConsumerIndex;
  UINT32            ProducerWrap;
  UINT32            ConsumerWrap;
  SMMUV3_CMDQ_PROD  Producer;
  EFI_STATUS        Status;
  EFI_TPL           OldTpl;
  UINT64            NewProducer;

  if ((SmmuInfo == NULL) || (Command == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  TotalQueueEntries = SMMUV3_COUNT_FROM_LOG2 (SmmuInfo->CommandQueueLog2Size);
  WrapMask          = TotalQueueEntries;
  QueueMask         = WrapMask - 1;

  // We need to synchronize the the entire command queue write and producer update with the TPL locks.
  // As a result we don't just lock SmmuV3CmdQueueUpdateCachedConsumer but the entire process of writing the command
  // and updating the producer index.
  OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);

  // Loop until there is space in the command queue
  do {
    Producer.AsUINT32 = SmmuV3ReadRegister32 (SmmuInfo->SmmuBase, SMMU_CMDQ_PROD);
    ProducerWrap      = Producer.WriteIndex & WrapMask;
    ProducerIndex     = Producer.WriteIndex & QueueMask;

    SmmuV3CmdQueueUpdateCachedConsumer (SmmuInfo, QueueMask, WrapMask, TotalQueueEntries, &ConsumerIndex, &ConsumerWrap);
  } while (SMMUV3_IS_QUEUE_FULL (ProducerIndex, ProducerWrap, ConsumerIndex, ConsumerWrap) != FALSE);

  Status = SmmuV3WriteCommands (SmmuInfo, ProducerIndex, 1, Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Error writing command to queue\n", __func__));
    gBS->RestoreTPL (OldTpl);
    return Status;
  }

  SmmuInfo->CachedProducer += 1;
  NewProducer               = SmmuInfo->CachedProducer;
  SmmuV3WriteRegister32 (SmmuInfo->SmmuBase, SMMU_CMDQ_PROD, (UINT32)(NewProducer & (WrapMask | QueueMask)));

  gBS->RestoreTPL (OldTpl);

  // Loop until the command is consumed
  do {
    // SmmuV3CmdQueueUpdateCachedConsumer needs to be within the scope of this lock because we want to make sure we have
    // the synchronized view of the consumer index when checking against the current local producer index.
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
    SmmuV3CmdQueueUpdateCachedConsumer (SmmuInfo, QueueMask, WrapMask, TotalQueueEntries, &ConsumerIndex, &ConsumerWrap);
    // Only prints errors if Event Queue is not empty and GError != 0. Returns EFI_SUCCESS if no errors found.
    Status = SmmuV3LogErrors (SmmuInfo);
    gBS->RestoreTPL (OldTpl);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: Error logged from SMMUv3 during command queue operation.\n", __func__));
      break;
    }
  } while (SmmuInfo->CachedConsumer < NewProducer);

  return Status;
}

/**
  Invalidate all TLB entries in the SMMUv3.

  @param [in]  SmmuInfo  Pointer to the SMMU_INFO structure.

  @retval EFI_SUCCESS            Success.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3TLBInvalidateAll (
  IN SMMU_INFO  *SmmuInfo
  )
{
  SMMUV3_CMD_GENERIC  Command;
  EFI_STATUS          Status;

  if (SmmuInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  // Invalidate TLBI Commands
  SMMUV3_BUILD_CMD_TLBI_NSNH_ALL (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CMD_TLBI_NSNH_ALL failed.\n", __func__));
    return Status;
  }

  // Issue a CMD_SYNC command to guarantee that any previously issued TLB
  // invalidations (CMD_TLBI_*) are completed (SMMUv3.2 spec section 4.6.3).
  SMMUV3_BUILD_CMD_SYNC_NO_INTERRUPT (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CMD_SYNC_NO_INTERRUPT failed.\n", __func__));
    return Status;
  }

  ArmDataSynchronizationBarrier ();

  return Status;
}

/**
  Invalidate TLB entries for specified InputAddress for Stage 2 of SmmuV3.

  @param [in]  SmmuInfo      Pointer to the SMMU_INFO structure.
  @param [in]  InputAddress  The input address to invalidate.

  @retval EFI_SUCCESS            Success.
  @retval EFI_TIMEOUT            Timeout.
  @retval EFI_INVALID_PARAMETER  Invalid Parameters.
**/
EFI_STATUS
SmmuV3TLBInvalidateAddress (
  IN SMMU_INFO  *SmmuInfo,
  IN UINT64     InputAddress
  )
{
  SMMUV3_CMD_GENERIC  Command;
  EFI_STATUS          Status;

  if (SmmuInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  // Invalidate with TLBI_S2_IPA Commands
  SMMUV3_BUILD_CMD_TLBI_S2_IPA (&Command, SMMUV3_STREAM_TABLE_ENTRY_S2VMID, InputAddress);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CMD_TLBI_S2_IPA failed.\n", __func__));
    return Status;
  }

  // Issue a CMD_SYNC command to guarantee that any previously issued TLB
  // invalidations (CMD_TLBI_*) are completed (SMMUv3.2 spec section 4.6.3).
  SMMUV3_BUILD_CMD_SYNC_NO_INTERRUPT (&Command);
  Status = SmmuV3SendCommand (SmmuInfo, &Command);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: CMD_SYNC_NO_INTERRUPT failed.\n", __func__));
    return Status;
  }

  ArmDataSynchronizationBarrier ();

  return Status;
}

/**
 * Add RMR mappings for each SMMU node in the SmmuInfo structure.
 * This function iterates through the RMR nodes and updates the page table
 * for each memory range described in the RMR_NODE_INFO list.
 *
 * @param [in] SmmuInfo  Pointer to the SMMU_INFO structure.
 *
 * @retval EFI_SUCCESS            Success.
 * @retval EFI_INVALID_PARAMETER  Invalid Parameters.
 * @retval Other                  RMR mapping update failure.
 */
EFI_STATUS
SmmuV3AddRMRMapping (
  IN SMMU_INFO  *SmmuInfo
  )
{
  LIST_ENTRY     *Entry;
  RMR_NODE_INFO  *Item;
  EFI_STATUS     Status;

  if (SmmuInfo == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Invalid Parameters\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  Entry = GetFirstNode (&SmmuInfo->RmrNodeList);
  while (!IsNull (&SmmuInfo->RmrNodeList, Entry)) {
    Item  = BASE_CR (Entry, RMR_NODE_INFO, Link);
    Entry = GetNextNode (&SmmuInfo->RmrNodeList, Entry);

    if ((Item != NULL) && (Item->BaseAddress > 0) && (Item->Length > 0)) {
      SmmuInfo->EBSBehaviorAbort = FALSE;
      DEBUG ((
        DEBUG_INFO,
        "%a: Adding RMR mapping for SMMU[0x%llx]: Base=0x%llx, Length=0x%llx\n",
        __func__,
        SmmuInfo->SmmuBase,
        Item->BaseAddress,
        Item->Length
        ));
      Status = UpdatePageTable (
                 SmmuInfo->PageTableRoot,
                 Item->BaseAddress,
                 Item->Length,
                 PAGE_TABLE_READ_WRITE_FROM_IOMMU_ACCESS ((EDKII_IOMMU_ACCESS_READ | EDKII_IOMMU_ACCESS_WRITE)),
                 TRUE
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Failed to update RMR mapping.\n", __func__));
        RemoveEntryList (&Item->Link);
        FreePool (Item);
        goto CleanupList;
      }
    }

    RemoveEntryList (&Item->Link);
    FreePool (Item);
  }

  return EFI_SUCCESS;

CleanupList:
  while (!IsNull (&SmmuInfo->RmrNodeList, Entry)) {
    Item  = BASE_CR (Entry, RMR_NODE_INFO, Link);
    Entry = GetNextNode (&SmmuInfo->RmrNodeList, Entry);
    RemoveEntryList (&Item->Link);
    FreePool (Item);
  }

  return Status;
}
