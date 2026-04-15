# SmmuDxe Driver - Detailed Documentation

## Table of Contents

- [1. Overview](#1-overview)
- [2. Architecture](#2-architecture)
- [3. File Descriptions](#3-file-descriptions)
- [4. Data Structures](#4-data-structures)
- [5. Driver Initialization Flow (SmmuDxe.c)](#5-driver-initialization-flow-smmudxec)
- [6. SMMUv3 Hardware Utilities (SmmuV3Util.c)](#6-smmuv3-hardware-utilities-smmuv3utilc)
- [7. IOMMU Protocol Implementation (IoMmu.c)](#7-iommu-protocol-implementation-iommuc)
- [8. Page Table Management](#8-page-table-management)
- [9. Stream Table Configuration](#9-stream-table-configuration)
- [10. Command and Event Queues](#10-command-and-event-queues)
- [11. TLB Invalidation](#11-tlb-invalidation)
- [12. IORT Parsing](#12-iort-parsing)
- [13. Reserved Memory Region (RMR) Support](#13-reserved-memory-region-rmr-support)
- [14. ExitBootServices Handling](#14-exitbootservices-handling)
- [15. Bounce Buffer Mechanism](#15-bounce-buffer-mechanism)
- [16. Memory Protection Model](#16-memory-protection-model)
- [17. Concurrency and Synchronization](#17-concurrency-and-synchronization)
- [18. Platform Integration Guide](#18-platform-integration-guide)
- [19. Limitations and Future Enhancements](#19-limitations-and-future-enhancements)
- [20. Reference Specifications](#20-reference-specifications)

---

## 1. Overview

SmmuDxe is a UEFI DXE (Driver Execution Environment) driver that configures Arm SMMUv3
(System Memory Management Unit version 3) hardware to provide DMA remapping and memory protection.
The driver implements the EDK II IOMMU protocol (`EDKII_IOMMU_PROTOCOL`), giving PCI device drivers
a uniform interface for mapping host memory to device-accessible addresses.

**Key capabilities:**

- Configures SMMUv3 hardware for Stage 2 address translation
- Maintains identity-mapped page tables (up to 4 levels) with 4KB granularity
- Supports linear and 2-level stream tables
- Handles Reserved Memory Regions (RMR) from the IORT
- Provides bounce buffering for alignment and address-range constraints
- Installs the IORT ACPI table for OS-level SMMU awareness
- Cleanly transitions SMMU state at ExitBootServices

**Build module type:** `DXE_DRIVER`
**Entry point:** `InitializeSmmuDxe`
**GUID:** `BE506866-85F2-45EC-AC01-9BBF39D4CA78`

---

## 2. Architecture

### Component Integration

```text
PCI Device Driver
      |
      v
PCI I/O Protocol  (calls Map / Unmap / SetAttribute)
      |
      v
EDKII_IOMMU_PROTOCOL   (IoMmu.c  -- identity-mapped page table updates)
      |
      v
SMMUv3 Hardware Layer   (SmmuV3Util.c -- register I/O, queues, TLB invalidation)
      |
      v
SMMUv3 Hardware         (stream table, page table, command queue, event queue)
```

### Protocol Dependencies

| Protocol | Role |
|----------|------|
| `gEfiAcpiTableProtocolGuid` | **Consumed** -- used to install the IORT ACPI table |
| `gEdkiiIoMmuProtocolGuid`   | **Produced** -- the IOMMU protocol exposed to PCI subsystem |

### HOB Dependency

| GUID | Purpose |
|------|---------|
| `gSmmuConfigHobGuid` | Carries platform-supplied `SMMU_CONFIG` structure containing IORT data and disabled-SMMU list |

---

## 3. File Descriptions

| File | Purpose |
|------|---------|
| `SmmuDxe.inf` | EDK II module information file -- defines sources, dependencies, GUIDs, protocols, library classes, and the DEPEX |
| `SmmuDxe.c` | Driver entry point and top-level orchestration: HOB consumption, IORT installation, page table / stream table / queue allocation, per-SMMU configuration, ExitBootServices callback |
| `SmmuV3Util.c` | Low-level SMMUv3 utilities: register read/write, interrupt control, translation enable/disable, global abort/bypass, polling, command queue management, TLB invalidation, event queue consumption, error logging, IORT parsing, RMR mapping |
| `IoMmu.c` | IOMMU protocol implementation: `Map`, `Unmap`, `SetAttribute`, `AllocateBuffer`, `FreeBuffer`, and the underlying page table update logic |
| `SmmuV3.h` | Header for SMMUv3 constants, macros (page table index calculation, queue size computation, queue empty/full checks), data structures (`SMMU_INFO`, `IOMMU_CONFIG`, `RMR_NODE_INFO`), and function prototypes for SmmuV3Util.c |
| `IoMmu.h` | Header for page table bit definitions, `PAGE_TABLE` structure, and prototypes for `UpdatePageTable` and `IoMmuInit` |
| `README.md` | This documentation file |

---

## 4. Data Structures

### 4.1 `SMMU_CONFIG` (platform HOB payload)

Defined externally (in `Guid/SmmuConfig.h`). Passed via `gSmmuConfigHobGuid`:

```c
typedef struct _SMMU_CONFIG {
  UINT32    VersionMajor;
  UINT32    VersionMinor;
  UINT32    SmmuDisabledListSize;    // bytes
  UINT32    SmmuDisabledListOffset;  // from start of struct
  UINT32    IortSize;
  UINT32    IortOffset;              // from start of struct
} SMMU_CONFIG;
```

The IORT table data and the disabled-list array are appended contiguously after this header,
addressed via their respective offsets.

### 4.2 `IOMMU_CONFIG`

Top-level runtime structure holding all SMMU instances:

```c
typedef struct _IOMMU_CONFIG {
  UINT32       SmmuCount;
  SMMU_INFO    *SmmuInfo;   // array of SmmuCount elements
} IOMMU_CONFIG;
```

Global instance: `mIoMmu` (declared in `SmmuDxe.c`, extern in `SmmuV3.h`).

### 4.3 `SMMU_INFO`

Per-SMMU instance state:

```c
typedef struct _SMMU_INFO {
  PAGE_TABLE    *PageTableRoot;
  VOID          *StreamTable;
  VOID          *CommandQueue;
  VOID          *EventQueue;
  LIST_ENTRY    RmrNodeList;
  UINT64        SmmuBase;                    // MMIO base
  UINT64        CachedProducer;              // command queue producer tracking
  UINT64        CachedConsumer;              // command queue consumer tracking
  UINT32        StreamTableSize;
  UINT32        StreamTableEntryMax;         // max stream ID
  UINT32        Flags;                       // from IORT SMMUv3 node
  UINT32        CommandQueueSize;
  UINT32        EventQueueSize;
  UINT32        StreamTableLog2Size;
  UINT32        CommandQueueLog2Size;
  UINT32        EventQueueLog2Size;
  UINT32        OutputAddressWidth;          // e.g. 48
  UINT8         TranslationStartingLevel;    // 0 (4-level) or 1 (3-level)
  BOOLEAN       PageTableRootConcatenated;
  BOOLEAN       RangeInvalidationSupported;  // IDR3.RIL
  BOOLEAN       EBSBehaviorAbort;            // TRUE=abort, FALSE=bypass at EBS
  BOOLEAN       Enabled;
} SMMU_INFO;
```

### 4.4 `PAGE_TABLE` and `PAGE_TABLE_ENTRY`

```c
typedef UINT64 PAGE_TABLE_ENTRY;

#define PAGE_TABLE_SIZE  (EFI_PAGE_SIZE / sizeof(PAGE_TABLE_ENTRY))  // 512 entries

typedef struct _PAGE_TABLE {
  PAGE_TABLE_ENTRY  Entries[PAGE_TABLE_SIZE];
} PAGE_TABLE;
```

Each page table occupies exactly one 4KB page. The root may be concatenated (up to 16 pages)
to support address widths between 39 and 43 bits with a starting level of 1.

### 4.5 `IOMMU_MAP_INFO`

Per-mapping tracking structure, passed as the opaque `Mapping` handle:

```c
typedef struct IOMMU_MAP_INFO {
  UINTN                    NumberOfBytes;
  UINT64                   DeviceAddress;
  UINT64                   HostAddress;
  EDKII_IOMMU_OPERATION    Operation;
} IOMMU_MAP_INFO;
```

### 4.6 `RMR_NODE_INFO`

Linked-list node for tracking RMR entries per SMMU:

```c
typedef struct _RMR_NODE_INFO {
  EFI_ACPI_6_0_IO_REMAPPING_RMR_NODE  *RmrNode;
  LIST_ENTRY                          Link;
} RMR_NODE_INFO;
```

---

## 5. Driver Initialization Flow (SmmuDxe.c)

### 5.1 `InitializeSmmuDxe` -- Entry Point

The entry point performs the following steps in order:

1. **Retrieve HOB data** -- `GetSmmuConfigHobData()` locates `gSmmuConfigHobGuid` and returns the
   `SMMU_CONFIG` pointer. Returns `EFI_NOT_FOUND` if absent.

2. **Version check** -- `CheckSmmuConfigStructure()` verifies that the HOB's `VersionMajor` /
   `VersionMinor` match the driver's expected version. No backward compatibility is provided;
   a mismatch returns `EFI_INCOMPATIBLE_VERSION`.

3. **Locate ACPI Table Protocol** -- Required for IORT installation. This is also expressed in
   the module's `[Depex]` section to guarantee availability.

4. **Register ExitBootServices callback** -- `SmmuV3ExitBootServices` is registered via
   `CreateEventEx` with `gEfiEventExitBootServicesGuid`.

5. **Allocate `IOMMU_CONFIG`** -- `IoMmuConfigInit()` allocates and zero-initializes the global
   `mIoMmu` structure.

6. **Parse IORT** -- `SmmuV3ParseIort()` (in SmmuV3Util.c) performs a multi-pass parse of the
   IORT embedded in the HOB:
   - Pass 1: Count SMMUv3 nodes
   - Pass 2: Extract base addresses, flags, initialize RMR lists
   - Pass 3: Calculate maximum stream IDs from Root Complex / Named Component ID mappings
   - Pass 4: Collect RMR node references

7. **Install IORT ACPI table** -- `AddIortTable()` calculates the checksum and calls
   `AcpiTable->InstallAcpiTable`.

8. **Initialize global page table** -- `PageTableInit()` allocates the root page table. To support
   concatenated translation tables, the root is allocated as 16 contiguous pages aligned to
   `16 * EFI_PAGE_SIZE`.

9. **Determine enabled/disabled SMMUs** -- The driver cross-references each SMMU's base address
   against the `SmmuDisabledList` from the HOB. Matching SMMUs have `Enabled` set to `FALSE`.

10. **Configure enabled SMMUs** -- For each enabled SMMU, `SmmuV3Configure()` is called
    (see Section 5.2).

11. **Disable/bypass non-enabled SMMUs** -- For SMMUs in the disabled list, translation is
    disabled and global bypass is set so their traffic passes through unmodified.

12. **Install IOMMU Protocol** -- `IoMmuInit()` calls `InstallMultipleProtocolInterfaces` to
    publish `gEdkiiIoMmuProtocolGuid`.

On any failure, `IoMmuDeInit()` tears down all allocated resources, disables translation on
all SMMUs, and sets them to global abort.

### 5.2 `SmmuV3Configure` -- Per-SMMU Hardware Setup

This is the core hardware configuration function. Steps:

1. **Disable existing translation** -- `SmmuV3DisableTranslation()` clears `CR0.SMMUEN`,
   `CMDQEN`, `EVENTQEN`, and `PRIQEN`.

2. **Disable interrupts and clear stale errors** -- `SmmuV3DisableInterrupts(SmmuBase, TRUE)`.

3. **Allocate stream table** -- `SmmuV3AllocateStreamTable()` allocates either a linear or
   2-level table depending on the maximum stream ID:
   - **2-level** if `StreamTableEntryMax >= PAGE_SIZE / sizeof(STE)` (i.e., >= 64 entries)
   - **Linear** otherwise.

4. **Apply RMR mappings** -- `SmmuV3AddRMRMapping()` walks the RMR node list and maps each
   memory range into the page table with read/write access.

5. **Build template STE** -- `SmmuV3BuildStreamTableEntry()` constructs a stream table entry
   configured for Stage 2 translation with the page table root address, output address width
   (capped at 48 bits), cacheability/shareability attributes (based on COHAC_OVERRIDE flag),
   and fault recording enabled.

6. **Populate stream table** -- All stream table entries are initialized from the template.
   For 2-level tables, a single L2 page is allocated and all L1 descriptors point to it.

7. **Allocate command queue** -- Size is `min(IDR1.CmdQs, 8)` log2 entries.

8. **Allocate event queue** -- Size is `min(IDR1.EventQs, 7)` log2 entries.

9. **Program registers:**
   - `STRTAB_BASE_CFG` -- format (linear/2-level), log2 size, split
   - `STRTAB_BASE` -- stream table physical address, read-allocation hint
   - `CMDQ_BASE` -- command queue physical address, log2 size
   - `CMDQ_PROD` / `CMDQ_CONS` -- zeroed
   - `EVENTQ_BASE` -- event queue physical address, log2 size
   - `EVENTQ_PROD` / `EVENTQ_CONS` -- zeroed (on Page 1 offset `0x10000`)
   - Check `IDR3.RIL` for range-invalidation support

10. **Enable interrupts** -- `SmmuV3EnableInterrupts()` enables GError and Event queue IRQs.

11. **Configure CR1** -- Sets queue inner/outer cacheability and shareability when COHAC is
    overridden.

12. **Configure CR2** -- Sets `E2H=0`, `RecInvSid=1` (record bad stream IDs), and `PTM=1`
    when BTM is supported.

13. **Enable command queue and event queue (CR0 part 1)** -- Sets `CMDQEN` and `EVENTQEN`,
    polls `CR0ACK`.

14. **Invalidate caches** -- Sends:
    - `CMD_CFGI_ALL` (invalidate all cached stream table configuration)
    - `CMD_TLBI_NSNH_ALL` (invalidate all non-secure non-hyp TLB entries)
    - `CMD_TLBI_EL2_ALL` (invalidate all EL2 TLB entries)
    - `CMD_SYNC` (wait for completion)

15. **Enable SMMU (CR0 part 2)** -- Sets `SMMUEN=1`, `EVENTQEN=1`, `CMDQEN=1`,
    `PRIQEN=0`, `VMW=0`, and `ATSCHK=1` (if ATS supported). Polls `CR0ACK`.

16. **Check for global errors** -- Reads `GERROR` register; any non-zero value returns
    `EFI_DEVICE_ERROR`.

17. **Log errors** -- `SmmuV3LogErrors()` drains the event queue and checks `GERROR`.

### 5.3 `PageTableInit` / `PageTableDeInit`

- **Init**: Allocates 16 aligned pages (the maximum for concatenated root tables), zeroed.
- **DeInit**: Recursively walks the page table tree, freeing each allocated page from leaves
  to root. The root level frees 16 pages; all other levels free 1 page.

### 5.4 `AcpiPlatformChecksum` / `AddIortTable`

Standard ACPI checksum calculation (`CalculateCheckSum8`) followed by
`AcpiTable->InstallAcpiTable` to register the IORT.

### 5.5 `IoMmuDeInit`

Teardown function called on initialization failure:

1. For each SMMU: disable translation, set global abort
2. Free page tables (recursive), stream tables, command queues, event queues
3. Free the `SmmuInfo` array and `IOMMU_CONFIG`

---

## 6. SMMUv3 Hardware Utilities (SmmuV3Util.c)

### 6.1 Register Access

| Function | Description |
|----------|-------------|
| `SmmuV3ReadRegister32` | 32-bit MMIO read at `SmmuBase + Register` |
| `SmmuV3ReadRegister64` | 64-bit MMIO read at `SmmuBase + Register` |
| `SmmuV3WriteRegister32` | 32-bit MMIO write |
| `SmmuV3WriteRegister64` | 64-bit MMIO write |

All validate `SmmuBase != 0`.

### 6.2 Address Width Encoding

| Function | Description |
|----------|-------------|
| `SmmuV3DecodeAddressWidth` | Converts `SMMU_ADDRESS_SIZE_TYPE` enum to bit width (32/36/40/42/44/48/52) |
| `SmmuV3EncodeAddressWidth` | Reverse of decode -- bit width to enum |

### 6.3 Translation Starting Level

`SmmuV3SetTranslationStartingLevel` determines:

- **4-level** (starting level 0, `S2SL0 = 0x2`): when `OutputAddressWidth >= 44`
- **3-level** (starting level 1, `S2SL0 = 0x1`): when `OutputAddressWidth < 44`
  - Concatenated root if `OutputAddressWidth > 39` (with starting level 1)

### 6.4 SMMU Control

| Function | Description |
|----------|-------------|
| `SmmuV3DisableInterrupts` | Clears GError, PRI, and EventQ interrupt enables in `IRQ_CTRL`; optionally clears stale `GERROR` bits |
| `SmmuV3EnableInterrupts` | Enables GError and EventQ interrupts via `IRQ_CTRL` |
| `SmmuV3DisableTranslation` | Clears `SMMUEN`, `CMDQEN`, `EVENTQEN`, `PRIQEN` in `CR0`; polls `CR0ACK` |
| `SmmuV3GlobalAbort` | Sets `GBPA.ABORT` and `GBPA.UPDATE`, polls for completion, verifies abort is active |
| `SmmuV3SetGlobalBypass` | Clears `GBPA.ABORT`, sets `GBPA.UPDATE`, polls for completion -- allows all streams to bypass |

### 6.5 Register Polling

`SmmuV3Poll(SmmuBase, SmmuReg, Mask, Value)`:

- Reads the register up to 10 times (0.1ms total, 10us per iteration)
- Returns `EFI_SUCCESS` when `(RegVal & Mask) == Value`
- Returns `EFI_TIMEOUT` if the condition is never met

### 6.6 Command Queue Operations

#### `SmmuV3WriteCommands`

Writes one or more `SMMUV3_CMD_GENERIC` entries to the circular command queue at the given
starting index, wrapping via `QueueMask`. Issues a DSB (data synchronization barrier) after
writes to ensure visibility before the producer index is updated.

#### `SmmuV3CmdQueueUpdateCachedConsumer`

Reads the hardware `CMDQ_CONS` register and updates `SmmuInfo->CachedConsumer` to reflect
the number of commands consumed. Handles wrap-bit transitions by adding `TotalQueueEntries`
when the wrap changes.

#### `SmmuV3SendCommand`

The primary interface for issuing a single command:

1. Raises TPL to `TPL_HIGH_LEVEL` to serialize command queue access
2. Spins until the queue is not full (compares producer vs. consumer with wrap bits)
3. Writes the command via `SmmuV3WriteCommands`
4. Increments `CachedProducer` and updates `CMDQ_PROD`
5. Restores TPL
6. Polls until the command is consumed (`CachedConsumer >= NewProducer`)
7. Checks for errors via `SmmuV3LogErrors` during the poll loop

### 6.7 TLB Invalidation

| Function | Behavior |
|----------|----------|
| `SmmuV3TLBInvalidateAll` | Sends `CMD_TLBI_NSNH_ALL` + `CMD_SYNC` + DSB |
| `SmmuV3TLBInvalidateAddress` | Sends `CMD_TLBI_S2_IPA` (for the given address and S2VMID) + `CMD_SYNC` + DSB |

### 6.8 Event Queue and Error Logging

#### `SmmuV3ConsumeEventQueueForErrors`

Reads the event queue producer/consumer indices from the Page 1 registers, checks if the
queue is empty, and if not, copies the next `SMMUV3_FAULT_RECORD` entry and advances the
consumer index (handling wrap). Issues a DSB before writing the updated consumer.

#### `SmmuV3LogErrors`

Drains the entire event queue by calling `SmmuV3ConsumeEventQueueForErrors` in a loop.
For each fault record:
- Prints the raw fault data
- If the fault type is a translation fault (codes `0x10`-`0x13`), dumps the page table
  entries for the faulting address via `SmmuV3DumpPageTableEntries`

Also checks the `GERROR` register for global errors.

### 6.9 Page Table Dump

`SmmuV3DumpPageTableEntries` walks the page table from the translation starting level to the
leaf, printing the entry value at each level for the given virtual address.

---

## 7. IOMMU Protocol Implementation (IoMmu.c)

### 7.1 Protocol Instance

```c
EDKII_IOMMU_PROTOCOL SmmuIoMmu = {
  EDKII_IOMMU_PROTOCOL_REVISION,
  IoMmuSetAttribute,
  IoMmuMap,
  IoMmuUnmap,
  IoMmuAllocateBuffer,
  IoMmuFreeBuffer,
};
```

Installed on a new handle by `IoMmuInit()`.

### 7.2 `IoMmuMap`

Maps a host buffer for DMA access by a bus master device.

**Parameters:**
- `Operation` -- direction and addressing capability of the DMA
- `HostAddress` -- CPU-visible buffer address
- `NumberOfBytes` -- size of the DMA transfer
- `DeviceAddress` (out) -- address the device should use
- `Mapping` (out) -- opaque handle for `Unmap` / `SetAttribute`

**Logic:**

1. Allocates an `IOMMU_MAP_INFO` tracking structure.
2. Determines if a **bounce buffer** is needed (see Section 15).
3. If bounce buffer is needed: allocates pages (below 4GB if 32-bit DMA), copies host data
   for read operations, and sets `DeviceAddress` to the bounce buffer.
4. If not needed: `DeviceAddress = HostAddress` (identity mapping).
5. Returns `IOMMU_MAP_INFO` as the `Mapping` handle.

Note: `IoMmuMap` does **not** update the page table. Page table updates happen in
`IoMmuSetAttribute`, which is called by the PCI I/O protocol after mapping.

### 7.3 `IoMmuUnmap`

Tears down a previously established mapping.

1. For bounce buffer case: copies data back to host buffer for write operations, frees
   bounce buffer pages.
2. Frees the `IOMMU_MAP_INFO` structure.

Note: Page table invalidation happens in `IoMmuSetAttribute` (called with `IoMmuAccess=0`
before `Unmap`), not in `Unmap` itself.

### 7.4 `IoMmuSetAttribute`

Sets or clears read/write access permissions for a mapping in the page table.

1. Calls `UpdatePageTable` with the device address, byte count, and R/W flags derived from
   `IoMmuAccess`.
2. When `IoMmuAccess == 0`, the `Valid` parameter is `FALSE`, which invalidates the leaf
   page table entries and triggers TLB invalidation across all enabled SMMUs.
3. After the update, logs any SMMU errors from all enabled instances.

### 7.5 `IoMmuAllocateBuffer` / `IoMmuFreeBuffer`

- **AllocateBuffer**: Allocates pages via `gBS->AllocatePages`. If the
  `EDKII_IOMMU_ATTRIBUTE_DUAL_ADDRESS_CYCLE` attribute is not set, allocation is restricted
  to below 4GB.
- **FreeBuffer**: Frees pages via `gBS->FreePages`.

---

## 8. Page Table Management

### 8.1 Structure

The SMMU page table follows the ARMv8-A VMSAv8-64 descriptor format
(see Arm ARM section D8.3.1):

```text
Level 0 (L0) -- 512 entries, each covering 512GB  (used when OutputAddressWidth >= 44)
    |
Level 1 (L1) -- 512 entries, each covering 1GB
    |
Level 2 (L2) -- 512 entries, each covering 2MB
    |
Level 3 (L3) -- 512 entries, each covering 4KB  (leaf -- maps to physical page)
```

For `OutputAddressWidth < 44`, translation starts at Level 1 (3-level paging).
If `OutputAddressWidth > 39` with starting level 1, the root table is **concatenated**
(multiple contiguous pages form the L1 table), allowing the L1 index to exceed 9 bits.

### 8.2 Page Table Entry Bits

| Bit | Name | Value |
|-----|------|-------|
| 0 | Valid | `0x1` |
| 1 | Descriptor (table/page) | `0x1 << 1` |
| 6 | AP[1] -- Read | `0x1 << 6` |
| 7 | AP[2] -- Write | `0x1 << 7` |
| 10 | Access Flag (AF) | `0x1 << 10` |

Intermediate entries (L0-L2): `PhysicalAddr | AF | Descriptor | Valid`
Leaf entries (L3): `PhysicalAddr | R/W flags | AF | Descriptor | Valid`

### 8.3 `UpdateMapping` (single-page)

Walks the page table from the starting level to the leaf (L3):

1. At each non-leaf level, looks up the index for the virtual address.
2. If the entry is empty, allocates a new page table page, sets valid/descriptor/AF bits.
3. Follows the pointer (masking off the lower 12 bits) to the next level.
4. At the leaf level:
   - If `Valid == TRUE`: writes `PhysicalAddress | Flags | AF | Descriptor | Valid`
   - If `Valid == FALSE`: clears only the valid bit (preserves remaining entry data)

### 8.4 `UpdatePageTable` (range)

Iterates page-by-page over the aligned address range `[PhysicalAddress, PhysicalAddress + Bytes)`,
calling `UpdateMapping` for each 4KB page. After all updates:

- If invalidating (`Valid == FALSE`): issues `SmmuV3TLBInvalidateAll` on every enabled SMMU.
- Executes a speculation barrier (`SpeculationBarrier`).

### 8.5 `PAGE_TABLE_INDEX` Macro

Computes the page table index for a given virtual address at a given level:

```c
#define PAGE_TABLE_INDEX(VirtualAddress, Level, OutputAddressWidth, StartLevel, Concatenated)
```

- For non-concatenated tables: standard 9-bit extraction at `bits [12 + 9*(3-Level) +: 9]`
- For concatenated root (Level == StartLevel == 1): extracts `9 + (OAW - 39)` bits to span
  the expanded root

---

## 9. Stream Table Configuration

### 9.1 Linear vs. 2-Level

The driver chooses between stream table formats based on `StreamTableEntryMax`:

- **Linear**: When `StreamTableEntryMax < PAGE_SIZE / sizeof(STE)` (< 64).
  Each entry is a full `SMMUV3_STREAM_TABLE_ENTRY`. Table size = `2^(log2(MaxSID+1))` entries.

- **2-Level**: When `StreamTableEntryMax >= 64`.
  - L1 table: array of `SMMUV3_L1_STREAM_TABLE_DESCRIPTOR`, each pointing to an L2 page.
  - L2 table: a single page of `SMMUV3_STREAM_TABLE_ENTRY` entries, shared by all L1
    descriptors. Split point is fixed at 6 (`SMMUV3_STR_TAB_BASE_CFG_SPLIT`).

### 9.2 Stream Table Entry (STE) Fields

Key fields set by `SmmuV3BuildStreamTableEntry`:

| Field | Value | Meaning |
|-------|-------|---------|
| `Config` | `0x6` | Stage 2 translate, Stage 1 bypass |
| `S2Ttb` | Page table root >> 4 | Stage 2 translation table base |
| `S2Tg` | `0` | 4KB granule |
| `S2Aa64` | `1` | AArch64 format |
| `S2Sl0` | `0x1` or `0x2` | Starting level (3 or 4 level paging) |
| `S2T0Sz` | `64 - OAW` | Input address size |
| `S2Ps` | encoded OAW | Physical address size |
| `S2Ptw` | `1` (if S1+S2 supported) | Protected table walk |
| `S2Ir0/S2Or0` | WB-WA or NC | Inner/outer cacheability (COHAC dependent) |
| `S2Sh0` | ISH or OSH | Shareability (COHAC dependent) |
| `S2Rs` | `2` | Record faults |
| `S2Vmid` | `1` | Non-zero VMID |
| `ShCfg` | Incoming or ISH | Shareability config for overrides |
| `Mtcfg/MemAttr` | Optional IWB-OWB | Memory type override (CCA+CPM+DACS dependent) |
| `Valid` | `1` | Entry is valid |

---

## 10. Command and Event Queues

### 10.1 Command Queue

- Size: `min(IDR1.CmdQs, 8)` log2 entries (max 256 entries)
- Entry size: `sizeof(SMMUV3_CMD_GENERIC)` (16 bytes)
- Circular buffer with producer/consumer indices and wrap bits
- Producer index (`CMDQ_PROD`) is written by software
- Consumer index (`CMDQ_CONS`) is written by hardware

Commands used:
- `CMD_CFGI_ALL` -- invalidate all cached configuration
- `CMD_TLBI_NSNH_ALL` -- invalidate all NS non-hyp TLB entries
- `CMD_TLBI_EL2_ALL` -- invalidate all EL2 TLB entries
- `CMD_TLBI_S2_IPA` -- invalidate by Stage 2 IPA
- `CMD_SYNC` -- synchronization barrier (no interrupt)

### 10.2 Event Queue

- Size: `min(IDR1.EventQs, 7)` log2 entries (max 128 entries)
- Entry size: `sizeof(SMMUV3_FAULT_RECORD)`
- Producer index written by hardware (on Page 1 register space)
- Consumer index written by software (on Page 1 register space)

Used for fault reporting. The driver drains the queue during `SmmuV3LogErrors` and during
the command-send poll loop.

---

## 11. TLB Invalidation

TLB invalidation is triggered in two scenarios:

1. **During SMMU configuration** (`SmmuV3Configure`):
   - Invalidates all entries: `CMD_CFGI_ALL` + `CMD_TLBI_NSNH_ALL` + `CMD_TLBI_EL2_ALL` +
     `CMD_SYNC`

2. **During page table updates** (`UpdatePageTable` with `Valid == FALSE`):
   - Calls `SmmuV3TLBInvalidateAll` on every enabled SMMU
   - This broadcasts the invalidation because the current protocol does not distinguish
     between SMMUs or stream IDs

All TLB invalidation sequences end with a `CMD_SYNC` and a DSB to ensure completion.

---

## 12. IORT Parsing

### 12.1 `SmmuV3ParseIort`

Entry point for IORT parsing. Validates the IORT signature and revision (supports revision 0
and revision 6), then performs four passes:

| Pass | Function | Purpose |
|------|----------|---------|
| 1 | `SmmuV3NodeCount` | Count SMMUv3 nodes in the IORT |
| 2 | `SmmuV3GetNodeInfo` | Extract base address, flags; initialize RMR lists and EBS defaults |
| 3 | `SmmuV3GetMaxStreamIds` | Walk Root Complex and Named Component nodes' ID mappings to find the maximum stream ID targeting each SMMU |
| 4 | `SmmuV3GetRMRNodeInfo` | Walk RMR nodes' ID mappings and link each RMR node to its target SMMU |

### 12.2 Max Stream ID Calculation

For each Root Complex or Named Component node with ID mappings:
- The output reference is resolved to an SMMU node pointer
- `MaxStreamId = max(MaxStreamId, OutputBase + NumIds)` for that SMMU

This value determines the stream table size.

---

## 13. Reserved Memory Region (RMR) Support

RMR nodes in the IORT describe memory ranges that devices may need to access through the
SMMU before the OS reconfigures it (e.g., GPU framebuffers, NIC DMA regions).

### 13.1 Collection

`SmmuV3GetRMRNodeInfo` links each RMR node to the appropriate SMMU via
`SmmuV3AddRmrNodeToList`, which inserts an `RMR_NODE_INFO` into the per-SMMU linked list.

### 13.2 Application

`SmmuV3AddRMRMapping` iterates the RMR list for a given SMMU and for each memory range
descriptor:
- Calls `UpdatePageTable` with read/write access to identity-map the range
- Sets `EBSBehaviorAbort = FALSE` (at ExitBootServices, bypass instead of abort, since
  active DMA may need continued access)
- Removes and frees the list entry after processing

---

## 14. ExitBootServices Handling

### `SmmuV3ExitBootServices`

Registered as an `EVT_NOTIFY_SIGNAL` callback at `TPL_CALLBACK`:

1. Raises TPL to `TPL_NOTIFY`
2. For each enabled SMMU:
   - If `EBSBehaviorAbort == TRUE` (no RMR mappings): calls `SmmuV3GlobalAbort` to block
     all transactions
   - If `EBSBehaviorAbort == FALSE` (has RMR mappings): calls `SmmuV3SetGlobalBypass` to
     allow traffic through without translation
   - Disables translation via `SmmuV3DisableTranslation`
3. Restores TPL and closes the event

The rationale:
- SMMUs with RMR mappings have active device DMA that must not be aborted during the OS
  handoff. Bypass mode allows that traffic to continue.
- SMMUs without RMR mappings can safely abort, as no active DMA is expected.
- In both cases, translation is disabled, deferring to the OS to reconfigure.
- The PCD `PcdDisableBMEonEBS` should be used in conjunction to clear Bus Master Enable on
  PCI devices, preventing new DMA initiations between EBS and OS reconfiguration.

---

## 15. Bounce Buffer Mechanism

### When Bounce Buffers Are Used

A bounce buffer is allocated in `IoMmuMap` when:
- The operation is **not** `BusMasterCommonBuffer` or `BusMasterCommonBuffer64`, AND
- **Either** of these conditions:
  1. `NumberOfBytes` or `HostAddress` is not 4KB-aligned
  2. The operation is 32-bit (`BusMasterRead` / `BusMasterWrite`) and the buffer extends
     beyond 4GB

### Data Copy Direction

| Phase | Read Operations | Write Operations |
|-------|----------------|-----------------|
| Map (alloc bounce buffer) | Host -> Bounce (device needs to read correct data) | No copy |
| Unmap (free bounce buffer) | No copy | Bounce -> Host (processor needs device-written data) |

### Address Constraints

- 32-bit DMA bounce buffers: allocated below `SIZE_4GB - 1`
- 64-bit DMA bounce buffers (alignment only): allocated anywhere

---

## 16. Memory Protection Model

### Access Control

- Page table entries encode Read and Write permissions per 4KB page
- `IoMmuSetAttribute` with `IoMmuAccess = 0` invalidates entries, removing device access
- `IoMmuSetAttribute` with `EDKII_IOMMU_ACCESS_READ | EDKII_IOMMU_ACCESS_WRITE` grants
  access

### Device Isolation

- Each device is identified by its Stream ID, mapped via the stream table
- Currently, all stream table entries share the same page table root (global page table),
  so isolation is at the page-table level rather than per-device

### Error Handling

- Translation faults generate event queue entries with fault codes
- Fault codes `0x10`-`0x13` (translation faults) trigger page table dumps for debugging
- Global errors are detected via the `GERROR` register
- Errors are logged during command queue operations and after `SetAttribute` calls

---

## 17. Concurrency and Synchronization

### TPL Usage

| Operation | TPL Level | Reason |
|-----------|-----------|--------|
| `SmmuV3SendCommand` (queue write + producer update) | `TPL_HIGH_LEVEL` | Serializes command queue writes across all callers |
| `SmmuV3SendCommand` (consumer poll) | `TPL_HIGH_LEVEL` | Ensures consistent view of consumer index |
| `SmmuV3ExitBootServices` | `TPL_NOTIFY` | Prevents re-entrant DMA operations during shutdown |

### Memory Barriers

- `ArmDataSynchronizationBarrier()` (DSB) is used:
  - After writing commands to the queue (before updating producer index)
  - After event queue consumer update
  - Before and after CR0 modifications
  - After TLB invalidation sequences
- `SpeculationBarrier()` is used after page table updates

---

## 18. Platform Integration Guide

### 18.1 HOB Construction

Platforms must build a `gSmmuConfigHobGuid` HOB containing:

1. A `SMMU_CONFIG` header
2. The complete IORT table (at `IortOffset` from the header start)
3. Optionally, a `UINT64[]` array of SMMU base addresses to disable (at
   `SmmuDisabledListOffset`)

```c
SMMU_CONFIG  Config;
Config.VersionMajor          = CURRENT_SMMU_CONFIG_VERSION_MAJOR;
Config.VersionMinor          = CURRENT_SMMU_CONFIG_VERSION_MINOR;
Config.IortOffset            = sizeof(SMMU_CONFIG);
Config.IortSize              = IortTableSize;
Config.SmmuDisabledListOffset = sizeof(SMMU_CONFIG) + IortTableSize;
Config.SmmuDisabledListSize   = NumDisabled * sizeof(UINT64);

// Build the HOB with Config + IORT data + disabled list appended
BuildGuidDataHob(&gSmmuConfigHobGuid, &HobPayload, TotalSize);
```

### 18.2 IORT Requirements

The IORT must contain:
- At least one `EFI_ACPI_IORT_TYPE_SMMUv3` node with correct `Base` and `Flags`
- Root Complex and/or Named Component nodes with ID mappings referencing the SMMU nodes
- Optionally, RMR nodes with memory range descriptors for regions needing pre-OS DMA access

The platform should **not** install the IORT separately; SmmuDxe handles IORT installation.

### 18.3 Pre-DXE Initialization

For continuous protection, the platform firmware (e.g., TF-A) should set the SMMU to abort
mode before DXE, ensuring no unauthorized DMA can occur before SmmuDxe configures the
hardware. SmmuDxe calls `SmmuV3SetGlobalBypass` during configuration to transition from
abort to operational state.

### 18.4 QEMU Support

SMMUv3 is supported on QEMU v9.1.50+ (<https://gitlab.com/qemu-project/qemu>).

---

## 19. Limitations and Future Enhancements

### Current Limitations

1. **Fixed 4KB granule** -- no 16KB or 64KB granule support
2. **48-bit address cap** -- even if hardware reports > 48-bit OAS, the driver caps at 48
   (64KB granule would be needed for > 48 bits)
3. **Stage 2 only** -- Stage 1 translation is bypassed
4. **Identity mapping** -- virtual address == physical address (no arbitrary IOVA remapping)
5. **Global page table** -- all stream IDs share one page table; no per-device isolation
6. **Shared L2 page** -- 2-level stream tables use a single L2 page for all L1 entries
7. **TLB invalidation broadcasts** -- invalidates all SMMUs on every unmap, not just the
   relevant one
8. **No backward compatibility** -- HOB version must match exactly

### Planned Enhancements

1. Multiple translation granule support (16KB, 64KB)
2. Stage 1 + Stage 2 translation
3. Per-device page tables (IoMmu Protocol V2)
4. Optimized TLB invalidation (per-SMMU, per-stream-ID)
5. Bounce buffer reduction by requiring page-aligned DMA buffers from drivers

---

## 20. Reference Specifications

| Document | URL |
|----------|-----|
| SMMUv3 Specification | <https://developer.arm.com/documentation/ihi0070/latest/> |
| SMMUv3 Programming Guide | <https://developer.arm.com/documentation/109242/0100/Programming-the-SMMU> |
| Arm AArch64 Memory Management | <https://developer.arm.com/documentation/101811/0104> |
| Arm Architecture Reference Manual | <https://developer.arm.com/documentation/102105/ka-07> |
| IORT Specification | <https://developer.arm.com/documentation/den0049/latest/> |
| Intel IOMMU for DMA Protection in UEFI | <https://www.intel.com/content/dam/develop/external/us/en/documents/intel-whitepaper-using-iommu-for-dma-protection-in-uefi.pdf> |

---

*Copyright (c) Microsoft Corporation. SPDX-License-Identifier: BSD-2-Clause-Patent*
