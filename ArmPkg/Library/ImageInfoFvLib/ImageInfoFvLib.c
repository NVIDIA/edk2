/** @file

  Firmware-volume instance of ImageInfoLib.

  Walks the FV at PcdFvBaseAddress and resolves an address to the PE32
  or TE image that contains it.  Intended for pre-DXE contexts
  (SEC / PEI / PrePi) where the EFI System Table is not yet published
  and Phase 1a's loaded-image walker is unavailable.

  The walker only inspects FFS files with the standard header layout.
  Files with FFS_ATTRIB_LARGE_FILE set, and sections with the
  extended-size sentinel (Size == 0xFFFFFF), are skipped per the
  Phase 1b design — they are not used by the platforms this library
  serves.

  Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ImageInfoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeCoffGetEntryPointLib.h>

#include <IndustryStandard/PeImage.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>

//
// Round (ActualSize) up to the next multiple of (Alignment).  Alignment
// must be a power of two.  Mirrors the GET_OCCUPIED_SIZE macro used by
// DebugAgentSymbolsBaseLib.
//
#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  ((ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))

/**
  Calculate the 8-bit zero-sum checksum of an FFS file header.

  The State and IntegrityCheck.Checksum.File fields are excluded from
  the calculation, per the PI spec.

  @param[in]  FileHeader  Pointer to the FFS file header.

  @retval  0 if the header is well-formed.
**/
STATIC
UINT8
CalculateHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FileHeader
  )
{
  UINT8  Sum;

  Sum = CalculateSum8 ((CONST VOID *)FileHeader, sizeof (EFI_FFS_FILE_HEADER));
  Sum = (UINT8)(Sum - FileHeader->State);
  Sum = (UINT8)(Sum - FileHeader->IntegrityCheck.Checksum.File);
  return Sum;
}

/**
  Return the highest bit set in the FFS file state, interpreted against
  the FV's erase polarity.  Mirrors the helper in
  DebugAgentSymbolsBaseLib.

  @param[in]  ErasePolarity  0 or 1, as defined by
                             EFI_FVB2_ERASE_POLARITY in the FV
                             Attributes field.
  @param[in]  FfsHeader      Pointer to the FFS file header.

  @retval  The highest set bit in the (polarity-adjusted) state field.
**/
STATIC
EFI_FFS_FILE_STATE
GetFileState (
  IN UINT8                ErasePolarity,
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  )
{
  EFI_FFS_FILE_STATE  FileState;
  EFI_FFS_FILE_STATE  HighestBit;

  FileState = FfsHeader->State;

  if (ErasePolarity != 0) {
    FileState = (EFI_FFS_FILE_STATE) ~FileState;
  }

  HighestBit = 0x80;
  while ((HighestBit != 0) && ((HighestBit & FileState) == 0)) {
    HighestBit >>= 1;
  }

  return HighestBit;
}

/**
  Find a PE32 or TE section in an FFS file whose payload contains
  Address, and write the resolved image base + return the PDB pointer.

  Each non-NULL output is initialized to 0 by the caller before this
  function is invoked; this function only writes the outputs on a hit.

  @param[in]  FfsHeader            FFS file to scan.
  @param[in]  FfsSize              Size of the FFS file, in bytes
                                   (from the Size[3] header field).
  @param[in]  Address              Address to locate.
  @param[out] ImageBase            Image base for the matched section
                                   (set only on hit).  May be NULL.
  @param[out] PeCoffSizeOfHeaders  PE/COFF header size for the matched
                                   image (set only on hit).  May be
                                   NULL.

  @retval  NULL    No matching section in this file.
  @retval  Other   PDB pointer for the matched image.
**/
STATIC
CHAR8 *
ScanFileForAddress (
  IN  EFI_FFS_FILE_HEADER  *FfsHeader,
  IN  UINT32               FfsSize,
  IN  UINTN                Address,
  OUT UINTN                *ImageBase,
  OUT UINTN                *PeCoffSizeOfHeaders
  )
{
  EFI_COMMON_SECTION_HEADER  *Section;
  UINT8                      *SectionData;
  UINT32                     SectionSize;
  UINT32                     SectionPayloadSize;
  UINT32                     OccupiedSize;
  UINTN                      Parsed;
  UINTN                      DataSize;
  UINTN                      Base;
  EFI_TE_IMAGE_HEADER        *TeHeader;

  //
  // Section iteration starts immediately after the FFS header and runs
  // for the file body (Size - sizeof(header)).
  //
  Section  = (EFI_COMMON_SECTION_HEADER *)(FfsHeader + 1);
  DataSize = (UINTN)FfsSize - sizeof (EFI_FFS_FILE_HEADER);
  Parsed   = 0;

  while (Parsed < DataSize) {
    //
    // Reject a trailing fragment smaller than the section header.
    // The size-field read below requires all three bytes to be
    // within the file body.
    //
    if (DataSize - Parsed < sizeof (EFI_COMMON_SECTION_HEADER)) {
      return NULL;
    }

    SectionSize = (UINT32)Section->Size[0]
                  | ((UINT32)Section->Size[1] << 8)
                  | ((UINT32)Section->Size[2] << 16);

    //
    // Extended-size sections (Size == 0xFFFFFF) are out of scope for
    // Phase 1b.  Bail out rather than mis-parse.
    //
    if (SectionSize == 0x00FFFFFF) {
      return NULL;
    }

    if ((SectionSize < sizeof (EFI_COMMON_SECTION_HEADER)) ||
        (SectionSize > DataSize - Parsed))
    {
      //
      // Malformed section header; stop here so we do not walk off the
      // end of the file body.
      //
      return NULL;
    }

    if ((Section->Type == EFI_SECTION_PE32) || (Section->Type == EFI_SECTION_TE)) {
      SectionData        = (UINT8 *)(Section + 1);
      SectionPayloadSize = SectionSize - (UINT32)sizeof (EFI_COMMON_SECTION_HEADER);

      //
      // Range check uses the section payload extent.  The image is
      // XIP in the FV, so the loaded bytes equal the section payload.
      //
      if ((Address >= (UINTN)SectionData) &&
          (Address < ((UINTN)SectionData + SectionPayloadSize)))
      {
        if (Section->Type == EFI_SECTION_PE32) {
          Base = (UINTN)SectionData;
        } else {
          //
          // TE: the *virtual* PE base is what aligns PC - ImageBase
          // with RVAs in the map file / DWARF.  See the design doc.
          //
          // Guard against malformed TE payloads:
          //   - payload smaller than the TE header would let us read
          //     past the section into uninitialised memory;
          //   - a missing TE signature means we are not really looking
          //     at a TE image;
          //   - a StrippedSize smaller than sizeof(EFI_TE_IMAGE_HEADER)
          //     violates the PI spec (StrippedSize is the size of the
          //     original PE header that was removed, which must be at
          //     least a full TE header's worth).
          //
          if (SectionPayloadSize < sizeof (EFI_TE_IMAGE_HEADER)) {
            return NULL;
          }

          TeHeader = (EFI_TE_IMAGE_HEADER *)SectionData;

          if (TeHeader->Signature != EFI_TE_IMAGE_HEADER_SIGNATURE) {
            return NULL;
          }

          if (TeHeader->StrippedSize < sizeof (EFI_TE_IMAGE_HEADER)) {
            return NULL;
          }

          Base = (UINTN)SectionData + sizeof (EFI_TE_IMAGE_HEADER) - TeHeader->StrippedSize;
        }

        if (ImageBase != NULL) {
          *ImageBase = Base;
        }

        if (PeCoffSizeOfHeaders != NULL) {
          //
          // PeCoffGetSizeOfHeaders reads the real PE/COFF headers in
          // memory, so pass the section-data pointer (the in-memory
          // start of the image), not the adjusted virtual TE base.
          //
          *PeCoffSizeOfHeaders = PeCoffGetSizeOfHeaders ((VOID *)SectionData);
        }

        return PeCoffLoaderGetPdbPointer ((VOID *)SectionData);
      }
    }

    //
    // Advance to the next section.  Sections are 4-byte aligned.
    //
    OccupiedSize = GET_OCCUPIED_SIZE (SectionSize, 4);
    if (OccupiedSize == 0) {
      return NULL;
    }

    Parsed += OccupiedSize;
    Section = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)Section + OccupiedSize);
  }

  return NULL;
}

/**
  Given an address, return the name of the PE/COFF image loaded at
  that address by walking the FV at PcdFvBaseAddress.

  Each non-NULL output pointer is always written: set to 0 when no
  image is found, or to the real value when one is found.

  @param[in]  Address              Address to find PE/COFF image for.
  @param[out] ImageBase            Load (or virtual, for TE) address
                                   of the found image, or 0.  May be
                                   NULL.
  @param[out] PeCoffSizeOfHeaders  Size of the PE/COFF header for the
                                   found image, or 0.  May be NULL.

  @retval NULL                     No image contains Address.
  @retval Other                    PDB pointer for the matched image.
**/
CHAR8 *
EFIAPI
GetImageName (
  IN  UINTN  Address,
  OUT UINTN  *ImageBase,
  OUT UINTN  *PeCoffSizeOfHeaders
  )
{
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  EFI_FFS_FILE_HEADER         *FfsHeader;
  UINT64                      FvBase;
  UINTN                       FileOffset;
  UINTN                       FvLength;
  UINT32                      FileSize;
  UINT32                      OccupiedSize;
  UINT8                       ErasePolarity;
  EFI_FFS_FILE_STATE          FileState;
  CHAR8                       *Name;

  if (ImageBase != NULL) {
    *ImageBase = 0;
  }

  if (PeCoffSizeOfHeaders != NULL) {
    *PeCoffSizeOfHeaders = 0;
  }

  //
  // Guard the base-address cast.  On AArch32 / IA32 a UINT64 value
  // above MAX_UINTN would silently truncate to zero (or an unrelated
  // address) when cast to UINTN.  PcdFvBaseAddress is a compile-time
  // platform constant, so this is a defensive authoring check rather
  // than an adversarial-input guard — the analogue of the FvLength cap
  // below.
  //
  FvBase = PcdGet64 (PcdFvBaseAddress);
  if (FvBase > MAX_UINTN) {
    return NULL;
  }

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)FvBase;
  if ((FvHeader == NULL) || (FvHeader->Signature != EFI_FVH_SIGNATURE)) {
    return NULL;
  }

  //
  // Cap FvLength at MAX_UINTN.  On AArch32 / IA32 builds UINTN is 32-
  // bit, so a corrupt or otherwise oversized FvLength header field
  // would otherwise wrap the FileOffset / FvLength - FileOffset
  // arithmetic below and let us walk past the end of the FV.  On
  // AArch64 / X64 builds UINTN is already 64-bit and the check is a
  // no-op.
  //
  if (FvHeader->FvLength > MAX_UINTN) {
    return NULL;
  }

  FvLength = (UINTN)FvHeader->FvLength;

  //
  // FV state bits are interpreted against the erase polarity recorded
  // in the FV header attributes.  Compute it once for the whole walk.
  //
  ErasePolarity = ((FvHeader->Attributes & EFI_FVB2_ERASE_POLARITY) != 0) ? 1 : 0;

  //
  // HeaderLength must be at least a full EFI_FIRMWARE_VOLUME_HEADER and
  // cannot exceed FvLength.  A corrupt value below the lower bound
  // would place the first FfsHeader inside the FV header itself; a
  // value above the upper bound would walk past the end of the FV.
  //
  if ((FvHeader->HeaderLength < sizeof (EFI_FIRMWARE_VOLUME_HEADER)) ||
      ((UINTN)FvHeader->HeaderLength > FvLength))
  {
    return NULL;
  }

  FileOffset = FvHeader->HeaderLength;
  FfsHeader  = (EFI_FFS_FILE_HEADER *)((UINT8 *)FvHeader + FileOffset);

  while (FileOffset + sizeof (EFI_FFS_FILE_HEADER) <= FvLength) {
    //
    // Reject large-file FFS entries — they use a different (larger)
    // header layout and are out of scope for Phase 1b.
    //
    if ((FfsHeader->Attributes & FFS_ATTRIB_LARGE_FILE) != 0) {
      //
      // We cannot trust Size[3] for a large file (the spec says it
      // must be zero), so we cannot safely skip past it.  Stop the
      // walk.
      //
      return NULL;
    }

    FileSize = (UINT32)FfsHeader->Size[0]
               | ((UINT32)FfsHeader->Size[1] << 8)
               | ((UINT32)FfsHeader->Size[2] << 16);
    if ((FileSize < sizeof (EFI_FFS_FILE_HEADER)) ||
        (FileSize > FvLength - FileOffset))
    {
      //
      // Malformed size; stop rather than walk off the end.
      //
      return NULL;
    }

    OccupiedSize = GET_OCCUPIED_SIZE (FileSize, 8);
    if (OccupiedSize == 0) {
      //
      // Unreachable with a 3-byte FileSize, but guards against an
      // infinite loop if the size logic ever changes.
      //
      return NULL;
    }

    //
    // Inspect files whose polarity-adjusted state is either
    // DATA_VALID or MARKED_FOR_UPDATE (an in-progress update is still
    // a live image), and whose header checksum is intact.
    //
    FileState = GetFileState (ErasePolarity, FfsHeader);
    if (((FileState == EFI_FILE_DATA_VALID) ||
         (FileState == EFI_FILE_MARKED_FOR_UPDATE)) &&
        (CalculateHeaderChecksum (FfsHeader) == 0))
    {
      Name = ScanFileForAddress (
               FfsHeader,
               FileSize,
               Address,
               ImageBase,
               PeCoffSizeOfHeaders
               );
      if (Name != NULL) {
        return Name;
      }
    }

    FileOffset += OccupiedSize;
    FfsHeader   = (EFI_FFS_FILE_HEADER *)((UINT8 *)FvHeader + FileOffset);
  }

  return NULL;
}
