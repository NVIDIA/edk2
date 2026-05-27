/** @file
  Unit tests for ImageInfoFvLib.

  The fixture builds a synthesised firmware-volume buffer in malloc'd
  memory, points the PcdStubLib at it, and exercises GetImageName()
  against every documented scenario in the Phase 1b plan.

  Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Library/GoogleTestLib.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
  #include <Uefi.h>
  #include <Library/BaseLib.h>
  #include <Library/ImageInfoLib.h>
  #include <IndustryStandard/PeImage.h>
  #include <Pi/PiFirmwareFile.h>
  #include <Pi/PiFirmwareVolume.h>

  VOID
  PcdStubSetFvBaseAddress (
    UINT64  Address
    );
}

using namespace testing;

//
// Round up to the next multiple of Alignment (power of two).  Matches
// the macro the library under test uses.
//
#define ROUND_UP_8(x)  (((x) + 7) & ~((UINTN)7))
#define ROUND_UP_4(x)  (((x) + 3) & ~((UINTN)3))

//////////////////////////////////////////////////////////////////////////////
class ImageInfoFvLibTest : public Test {
public:

protected:
  //
  // A generously sized buffer for the synthesised FV.  All tests stay
  // well below this limit.
  //
  static constexpr UINTN FV_BUFFER_SIZE = 0x4000;

  UINT8 *FvBuffer;
  UINTN FvSize;        // High-water mark of bytes written.

  void
  SetUp (
    ) override
  {
    FvBuffer = (UINT8 *)malloc (FV_BUFFER_SIZE);
    ASSERT_NE (FvBuffer, nullptr);
    memset (FvBuffer, 0, FV_BUFFER_SIZE);
    FvSize = 0;
    PcdStubSetFvBaseAddress ((UINT64)(UINTN)FvBuffer);
  }

  void
  TearDown (
    ) override
  {
    if (FvBuffer != nullptr) {
      free (FvBuffer);
      FvBuffer = nullptr;
    }

    PcdStubSetFvBaseAddress (0);
  }

  /**
    Stamp an EFI_FIRMWARE_VOLUME_HEADER at the start of the buffer.
    HeaderLength is set to sizeof(EFI_FIRMWARE_VOLUME_HEADER); the
    library walks files starting at that offset.
  **/
  void
  BuildFvHeader (
    UINT64  FvLength,
    UINT32  Signature = EFI_FVH_SIGNATURE
    )
  {
    EFI_FIRMWARE_VOLUME_HEADER  *FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)FvBuffer;

    FvHeader->FvLength     = FvLength;
    FvHeader->Signature    = Signature;
    FvHeader->HeaderLength = (UINT16)sizeof (EFI_FIRMWARE_VOLUME_HEADER);
    FvSize                 = sizeof (EFI_FIRMWARE_VOLUME_HEADER);
  }

  /**
    Append an FFS file header followed by raw payload bytes.

    The header checksum is computed over (Header - State - File-checksum)
    and patched into IntegrityCheck.Checksum.Header so the file passes
    the library's checksum gate.

    @param  Type       FFS file type (EFI_FV_FILETYPE_*).
    @param  State      FFS file state byte (EFI_FILE_DATA_VALID for a
                       valid file).
    @param  Attributes FFS attributes byte; OR in FFS_ATTRIB_LARGE_FILE
                       to drive the skip path.
    @param  Payload    Bytes to copy in immediately after the FFS
                       header (typically section headers + body).
    @param  PayloadLen Length of Payload in bytes.

    @return The offset (relative to FvBuffer) at which this file's
            header starts.
  **/
  UINTN
  AddFfsFile (
    EFI_FV_FILETYPE          Type,
    EFI_FFS_FILE_STATE       State,
    EFI_FFS_FILE_ATTRIBUTES  Attributes,
    const UINT8              *Payload,
    UINT32                   PayloadLen
    )
  {
    UINTN                FileStart = FvSize;
    EFI_FFS_FILE_HEADER  *Hdr      = (EFI_FFS_FILE_HEADER *)(FvBuffer + FileStart);
    UINT32               TotalLen  = sizeof (EFI_FFS_FILE_HEADER) + PayloadLen;
    UINT8                Sum;

    memset (Hdr, 0, sizeof (EFI_FFS_FILE_HEADER));
    Hdr->Type                         = Type;
    Hdr->Attributes                   = Attributes;
    Hdr->Size[0]                      = (UINT8)(TotalLen & 0xFF);
    Hdr->Size[1]                      = (UINT8)((TotalLen >> 8) & 0xFF);
    Hdr->Size[2]                      = (UINT8)((TotalLen >> 16) & 0xFF);
    Hdr->IntegrityCheck.Checksum.File = FFS_FIXED_CHECKSUM;

    //
    // Compute header checksum so the entire (header - State - File-checksum)
    // sums to zero.  CalculateSum8 is in BaseLib; we replicate it inline
    // to avoid pulling in a BaseLib dependency for the test harness.
    //
    Sum = 0;
    for (UINTN i = 0; i < sizeof (EFI_FFS_FILE_HEADER); i++) {
      Sum = (UINT8)(Sum + ((UINT8 *)Hdr)[i]);
    }

    Sum                                 = (UINT8)(Sum - Hdr->State);
    Sum                                 = (UINT8)(Sum - Hdr->IntegrityCheck.Checksum.File);
    Hdr->IntegrityCheck.Checksum.Header = (UINT8)(0 - Sum);

    //
    // Setting State last keeps the checksum calculation simple
    // (State is excluded from the sum).
    //
    Hdr->State = State;

    if (PayloadLen > 0) {
      memcpy (FvBuffer + FileStart + sizeof (EFI_FFS_FILE_HEADER), Payload, PayloadLen);
    }

    //
    // Advance to next 8-byte boundary for the next file.
    //
    FvSize = ROUND_UP_8 (FileStart + TotalLen);

    return FileStart;
  }

  /**
    Build a section header + payload into the supplied buffer.
    Sections are 4-byte aligned within an FFS file.

    @param  Buffer     Destination buffer (caller-owned).
    @param  Offset     Offset into Buffer to start writing.
    @param  Type       Section type (EFI_SECTION_PE32, _TE, _RAW, ...).
    @param  Payload    Payload bytes (may be NULL if PayloadLen == 0).
    @param  PayloadLen Payload byte count.

    @return New offset (4-byte aligned past this section).
  **/
  static UINTN
  WriteSection (
    UINT8             *Buffer,
    UINTN             Offset,
    EFI_SECTION_TYPE  Type,
    const UINT8       *Payload,
    UINT32            PayloadLen
    )
  {
    EFI_COMMON_SECTION_HEADER  *Sec     = (EFI_COMMON_SECTION_HEADER *)(Buffer + Offset);
    UINT32                     TotalLen = (UINT32)sizeof (EFI_COMMON_SECTION_HEADER) + PayloadLen;

    Sec->Size[0] = (UINT8)(TotalLen & 0xFF);
    Sec->Size[1] = (UINT8)((TotalLen >> 8) & 0xFF);
    Sec->Size[2] = (UINT8)((TotalLen >> 16) & 0xFF);
    Sec->Type    = Type;

    if (PayloadLen > 0) {
      memcpy (Buffer + Offset + sizeof (EFI_COMMON_SECTION_HEADER), Payload, PayloadLen);
    }

    return ROUND_UP_4 (Offset + TotalLen);
  }
};

//////////////////////////////////////////////////////////////////////////////
// Invalid / empty FV
//

TEST_F (ImageInfoFvLibTest, InvalidSignature_ReturnsNullAndZeroes) {
  UINTN  ImageBase = 0xDEADBEEF;
  UINTN  HdrSize   = 0xDEADBEEF;

  BuildFvHeader (FV_BUFFER_SIZE, 0xBADBAD00);

  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + 0x100, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

TEST_F (ImageInfoFvLibTest, EmptyFv_HeaderOnly_ReturnsNull) {
  UINTN  ImageBase = 0xDEADBEEF;
  UINTN  HdrSize   = 0xDEADBEEF;

  BuildFvHeader (sizeof (EFI_FIRMWARE_VOLUME_HEADER));

  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + 0x100, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

TEST_F (ImageInfoFvLibTest, AddressBeforeAndAfterFile_ReturnsNull) {
  UINT8  SectionBuf[64];
  UINTN  SecOff      = 0;
  UINT8  Payload[16] = { 0 };

  memset (SectionBuf, 0, sizeof (SectionBuf));
  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  AddFfsFile (
    EFI_FV_FILETYPE_PEIM,
    EFI_FILE_DATA_VALID,
    0,
    SectionBuf,
    (UINT32)SecOff
    );

  //
  // Address strictly before the FV header is outside any section
  // even though it shares an FV-relative offset of 0 — the library
  // walks files within the FV bounds.  A bare-byte address before
  // FvBuffer is well below the section payload pointer and should
  // not match.
  //
  EXPECT_EQ (GetImageName (0x1, nullptr, nullptr), nullptr);

  //
  // Address far past the FV end — no match.
  //
  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + FV_BUFFER_SIZE + 0x1000, nullptr, nullptr), nullptr);
}

//////////////////////////////////////////////////////////////////////////////
// Single PE32 section
//

TEST_F (ImageInfoFvLibTest, SinglePe32_HitInsidePayload) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64];
  UINTN  FileStart;
  UINTN  SectionPayloadAbs;
  UINTN  ImageBase = 0;
  UINTN  HdrSize   = 0;
  CHAR8  *Name;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0xAB, sizeof (Payload));

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  //
  // Section payload begins after FFS header + common section header.
  //
  SectionPayloadAbs = (UINTN)FvBuffer + FileStart
                      + sizeof (EFI_FFS_FILE_HEADER)
                      + sizeof (EFI_COMMON_SECTION_HEADER);

  Name = GetImageName (SectionPayloadAbs + 8, &ImageBase, &HdrSize);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, SectionPayloadAbs);
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

//////////////////////////////////////////////////////////////////////////////
// Single TE section — virtual-base adjustment
//

TEST_F (ImageInfoFvLibTest, SingleTe_AdjustedVirtualBase) {
  UINT8                SectionBuf[256];
  UINTN                SecOff;
  UINT8                Payload[128];
  EFI_TE_IMAGE_HEADER  *Te;
  UINTN                FileStart;
  UINTN                SectionPayloadAbs;
  UINTN                ImageBase = 0;
  UINTN                HdrSize   = 0;
  CHAR8                *Name;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0, sizeof (Payload));

  //
  // Plant a TE header at the start of the payload.  StrippedSize is
  // the field that drives the virtual-base adjustment:
  //   *ImageBase = SectionData + sizeof(TE_HEADER) - StrippedSize
  // For StrippedSize == 0x40 and sizeof(TE_HEADER) == 0x28 (on the
  // current spec), the adjustment is -0x18.  We do not hard-code the
  // sizeof; we read it back from the same struct the library uses.
  //
  Te               = (EFI_TE_IMAGE_HEADER *)Payload;
  Te->Signature    = EFI_TE_IMAGE_HEADER_SIGNATURE;
  Te->StrippedSize = 0x40;

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_TE, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  SectionPayloadAbs = (UINTN)FvBuffer + FileStart
                      + sizeof (EFI_FFS_FILE_HEADER)
                      + sizeof (EFI_COMMON_SECTION_HEADER);

  Name = GetImageName (SectionPayloadAbs + 0x10, &ImageBase, &HdrSize);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (
    ImageBase,
    SectionPayloadAbs + sizeof (EFI_TE_IMAGE_HEADER) - 0x40
    );
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

//////////////////////////////////////////////////////////////////////////////
// Multiple FFS files — pick the right one
//

TEST_F (ImageInfoFvLibTest, MultipleFiles_ResolvesToCorrect) {
  UINT8  Sec1[256];
  UINT8  Sec2[256];
  UINT8  Sec3[256];
  UINTN  Sec1Off, Sec2Off, Sec3Off;
  UINT8  Pay1[64], Pay2[64], Pay3[64];
  UINTN  F1, F2, F3;
  UINTN  PayloadAbs2;
  UINTN  ImageBase = 0;
  UINTN  HdrSize   = 0;
  CHAR8  *Name;

  memset (Sec1, 0, sizeof (Sec1));
  memset (Sec2, 0, sizeof (Sec2));
  memset (Sec3, 0, sizeof (Sec3));
  memset (Pay1, 0x11, sizeof (Pay1));
  memset (Pay2, 0x22, sizeof (Pay2));
  memset (Pay3, 0x33, sizeof (Pay3));

  Sec1Off = WriteSection (Sec1, 0, EFI_SECTION_PE32, Pay1, sizeof (Pay1));
  Sec2Off = WriteSection (Sec2, 0, EFI_SECTION_PE32, Pay2, sizeof (Pay2));
  Sec3Off = WriteSection (Sec3, 0, EFI_SECTION_PE32, Pay3, sizeof (Pay3));

  BuildFvHeader (FV_BUFFER_SIZE);
  F1 = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, Sec1, (UINT32)Sec1Off);
  F2 = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, Sec2, (UINT32)Sec2Off);
  F3 = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, Sec3, (UINT32)Sec3Off);

  (void)F1;
  (void)F3;

  PayloadAbs2 = (UINTN)FvBuffer + F2
                + sizeof (EFI_FFS_FILE_HEADER)
                + sizeof (EFI_COMMON_SECTION_HEADER);

  Name = GetImageName (PayloadAbs2 + 16, &ImageBase, &HdrSize);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, PayloadAbs2);
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

//////////////////////////////////////////////////////////////////////////////
// Non-PE32/non-TE section is skipped
//

TEST_F (ImageInfoFvLibTest, RawSection_SkippedAndAddressMissed) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64];
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  ImageBase = 0xDEADBEEF;
  UINTN  HdrSize   = 0xDEADBEEF;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0xCC, sizeof (Payload));

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_RAW, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_FREEFORM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_EQ (GetImageName (PayloadAbs + 8, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

//////////////////////////////////////////////////////////////////////////////
// FFS_ATTRIB_LARGE_FILE is skipped
//

TEST_F (ImageInfoFvLibTest, LargeFileAttribute_Skipped) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64];
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  ImageBase = 0xDEADBEEF;
  UINTN  HdrSize   = 0xDEADBEEF;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0, sizeof (Payload));

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                FFS_ATTRIB_LARGE_FILE,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  //
  // Address that would otherwise hit the PE32 payload — but the file
  // is flagged FFS_ATTRIB_LARGE_FILE, so the walker stops without
  // returning a name.
  //
  EXPECT_EQ (GetImageName (PayloadAbs + 8, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

//////////////////////////////////////////////////////////////////////////////
// Output-arg NULL handling matrix — mirrors Phase 1a
//

TEST_F (ImageInfoFvLibTest, NullOutputs_NotFound_NoCrash) {
  BuildFvHeader (sizeof (EFI_FIRMWARE_VOLUME_HEADER));
  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + 0x100, nullptr, nullptr), nullptr);
}

TEST_F (ImageInfoFvLibTest, NullPeCoffSize_NotFound) {
  UINTN  ImageBase = 0xDEADBEEF;

  BuildFvHeader (sizeof (EFI_FIRMWARE_VOLUME_HEADER));
  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + 0x100, &ImageBase, nullptr), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
}

TEST_F (ImageInfoFvLibTest, NullImageBase_NotFound) {
  UINTN  HdrSize = 0xDEADBEEF;

  BuildFvHeader (sizeof (EFI_FIRMWARE_VOLUME_HEADER));
  EXPECT_EQ (GetImageName ((UINTN)FvBuffer + 0x100, nullptr, &HdrSize), nullptr);
  EXPECT_EQ (HdrSize, 0ULL);
}

TEST_F (ImageInfoFvLibTest, NullPeCoffSize_Found) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64] = { 0 };
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  ImageBase = 0;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, SectionBuf, (UINT32)SecOff);

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_STREQ (GetImageName (PayloadAbs + 4, &ImageBase, nullptr), "TestImage.efi");
  EXPECT_EQ (ImageBase, PayloadAbs);
}

TEST_F (ImageInfoFvLibTest, NullImageBase_Found) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64] = { 0 };
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  HdrSize = 0;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, SectionBuf, (UINT32)SecOff);

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_STREQ (GetImageName (PayloadAbs + 4, nullptr, &HdrSize), "TestImage.efi");
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

TEST_F (ImageInfoFvLibTest, BothOutputsNull_Found) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64] = { 0 };
  UINTN  FileStart;
  UINTN  PayloadAbs;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (EFI_FV_FILETYPE_PEIM, EFI_FILE_DATA_VALID, 0, SectionBuf, (UINT32)SecOff);

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_STREQ (GetImageName (PayloadAbs + 4, nullptr, nullptr), "TestImage.efi");
}

//////////////////////////////////////////////////////////////////////////////
// Patchset 2 — additional review-driven coverage
//
// Each new TEST_F below targets one of the F2..F4 fixes documented in
// the Phase 1b patchset-2 plan:
//   - F2 TE size guard:    Te_TruncatedHeader_ReturnsNull
//                          Te_BadSignature_ReturnsNull
//   - F3 StrippedSize:     Te_StrippedSizeTooSmall_ReturnsNull
//   - F4 erase polarity:   ErasePolarityOne_DataValid_Resolves
//                          MarkedForUpdate_Resolves
//
// F1a (FvBase > MAX_UINTN cap) and F1b (FvLength > MAX_UINTN cap) are
// only reachable on AArch32 builds, which edk2-nvidia does not target.
// No host tests.
//

//
// F2: TE section whose payload is smaller than EFI_TE_IMAGE_HEADER.
//
TEST_F (ImageInfoFvLibTest, Te_TruncatedHeader_ReturnsNull) {
  UINT8  SectionBuf[64];
  UINTN  SecOff;
  UINT8  Payload[8] = { 0 };       // < sizeof(EFI_TE_IMAGE_HEADER)
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  ImageBase = 0xDEADBEEF;
  UINTN  HdrSize   = 0xDEADBEEF;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_TE, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  //
  // Address inside the truncated TE payload — the size guard must
  // refuse to dereference the TE header.
  //
  EXPECT_EQ (GetImageName (PayloadAbs + 2, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

//
// F2: TE section with valid size but wrong signature.  The library
// must refuse to treat it as a TE image.
//
TEST_F (ImageInfoFvLibTest, Te_BadSignature_ReturnsNull) {
  UINT8                SectionBuf[256];
  UINTN                SecOff;
  UINT8                Payload[128];
  EFI_TE_IMAGE_HEADER  *Te;
  UINTN                FileStart;
  UINTN                PayloadAbs;
  UINTN                ImageBase = 0xDEADBEEF;
  UINTN                HdrSize   = 0xDEADBEEF;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0, sizeof (Payload));

  Te               = (EFI_TE_IMAGE_HEADER *)Payload;
  Te->Signature    = 0xBAD0;   // not EFI_TE_IMAGE_HEADER_SIGNATURE ("VZ")
  Te->StrippedSize = 0x40;

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_TE, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_EQ (GetImageName (PayloadAbs + 0x10, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

//
// F3: TE section whose StrippedSize is smaller than the TE header.
// Without the lower-bound guard the subtraction in the library would
// wrap UINTN and produce a near-MAX_UINTN virtual base.
//
TEST_F (ImageInfoFvLibTest, Te_StrippedSizeTooSmall_ReturnsNull) {
  UINT8                SectionBuf[256];
  UINTN                SecOff;
  UINT8                Payload[128];
  EFI_TE_IMAGE_HEADER  *Te;
  UINTN                FileStart;
  UINTN                PayloadAbs;
  UINTN                ImageBase = 0xDEADBEEF;
  UINTN                HdrSize   = 0xDEADBEEF;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0, sizeof (Payload));

  Te               = (EFI_TE_IMAGE_HEADER *)Payload;
  Te->Signature    = EFI_TE_IMAGE_HEADER_SIGNATURE;
  Te->StrippedSize = 0;   // < sizeof(EFI_TE_IMAGE_HEADER)

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_TE, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                EFI_FILE_DATA_VALID,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  EXPECT_EQ (GetImageName (PayloadAbs + 0x10, &ImageBase, &HdrSize), nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (HdrSize, 0ULL);
}

//
// F4 (a): FV with erase polarity = 1.  A polarity-1 FV stores a
// DATA_VALID file as raw State = ~(HEADER_CONSTRUCTION | HEADER_VALID
// | DATA_VALID) = ~0x07 = 0xF8.  Without the polarity-aware check in
// the library, this file would be skipped on every real platform.
//
TEST_F (ImageInfoFvLibTest, ErasePolarityOne_DataValid_Resolves) {
  UINT8                       SectionBuf[256];
  UINTN                       SecOff;
  UINT8                       Payload[64];
  UINTN                       FileStart;
  UINTN                       PayloadAbs;
  UINTN                       ImageBase = 0;
  UINTN                       HdrSize   = 0;
  CHAR8                       *Name;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0xAB, sizeof (Payload));

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);

  //
  // Flip the FV erase polarity.  EFI_FVB2_ERASE_POLARITY in
  // Attributes selects the polarity-1 interpretation of file state.
  //
  FvHeader              = (EFI_FIRMWARE_VOLUME_HEADER *)FvBuffer;
  FvHeader->Attributes |= EFI_FVB2_ERASE_POLARITY;

  //
  // State 0xF8 == ~(0x07): under polarity 1 the FileState resolves to
  // 0x07, whose highest set bit is 0x04 == EFI_FILE_DATA_VALID.
  //
  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                (EFI_FFS_FILE_STATE)0xF8,
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  Name = GetImageName (PayloadAbs + 4, &ImageBase, &HdrSize);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, PayloadAbs);
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

//
// F4 (b): an in-progress update is still a live image — the walker
// must resolve a file whose state highest-bit is MARKED_FOR_UPDATE
// (0x08).  Using polarity 0, that means raw State has bits
// 0x01|0x02|0x04|0x08 set = 0x0F.
//
TEST_F (ImageInfoFvLibTest, MarkedForUpdate_Resolves) {
  UINT8  SectionBuf[256];
  UINTN  SecOff;
  UINT8  Payload[64];
  UINTN  FileStart;
  UINTN  PayloadAbs;
  UINTN  ImageBase = 0;
  UINTN  HdrSize   = 0;
  CHAR8  *Name;

  memset (SectionBuf, 0, sizeof (SectionBuf));
  memset (Payload, 0xAB, sizeof (Payload));

  SecOff = WriteSection (SectionBuf, 0, EFI_SECTION_PE32, Payload, sizeof (Payload));

  BuildFvHeader (FV_BUFFER_SIZE);

  FileStart = AddFfsFile (
                EFI_FV_FILETYPE_PEIM,
                (EFI_FFS_FILE_STATE)(EFI_FILE_HEADER_CONSTRUCTION
                                     | EFI_FILE_HEADER_VALID
                                     | EFI_FILE_DATA_VALID
                                     | EFI_FILE_MARKED_FOR_UPDATE),
                0,
                SectionBuf,
                (UINT32)SecOff
                );

  PayloadAbs = (UINTN)FvBuffer + FileStart
               + sizeof (EFI_FFS_FILE_HEADER)
               + sizeof (EFI_COMMON_SECTION_HEADER);

  Name = GetImageName (PayloadAbs + 4, &ImageBase, &HdrSize);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, PayloadAbs);
  EXPECT_EQ (HdrSize, (UINTN)0x200);
}

//////////////////////////////////////////////////////////////////////////////
int
main (
  int   argc,
  char  *argv[]
  )
{
  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
