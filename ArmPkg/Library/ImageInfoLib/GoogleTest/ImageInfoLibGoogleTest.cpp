/** @file
  Unit tests for ImageInfoLib.

  Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Library/GoogleTestLib.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
  #include <Uefi.h>
  #include <Library/ImageInfoLib.h>
  #include <Library/DebugLib.h>
  #include <Library/BaseMemoryLib.h>
  #include <Library/PeCoffGetEntryPointLib.h>
  #include <Library/UefiBootServicesTableLib.h>
  #include <Guid/DebugImageInfoTable.h>
}

#define LOADED_IMAGE_TEST_GST_TABLES_MAX    5
#define LOADED_IMAGE_TEST_DEBUG_TABLES_MAX  5

// gST is not provided by a stub lib -- ImageInfoLib accesses it directly.
EFI_SYSTEM_TABLE  *gST;

using namespace testing;

//////////////////////////////////////////////////////////////////////////////
class ImageInfoLibTest : public  Test {
public:

protected:
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER *DebugTableHeader;

  void
  SetUp (
    ) override
  {
    gST              = NULL;
    DebugTableHeader = NULL;
  }

  void
  TearDown (
    ) override
  {
    UINT32  Count;

    if (gST != NULL) {
      free (gST->ConfigurationTable);
      free (gST);
      gST = NULL;
    }

    if (DebugTableHeader != NULL) {
      if (DebugTableHeader->EfiDebugImageInfoTable != NULL) {
        for (Count = 0; Count < DebugTableHeader->TableSize; Count++) {
          if (DebugTableHeader->EfiDebugImageInfoTable[Count].NormalImage != NULL) {
            free (DebugTableHeader->EfiDebugImageInfoTable[Count].NormalImage);
          }
        }

        free (DebugTableHeader->EfiDebugImageInfoTable);
      }

      free (DebugTableHeader);
    }
  }

  /** Malloc a gST with no ConfigTable entries */
  void
  StubGST (
    )
  {
    gST                       = (EFI_SYSTEM_TABLE *)malloc (sizeof (EFI_SYSTEM_TABLE));
    gST->NumberOfTableEntries = 0;
    gST->ConfigurationTable   = (EFI_CONFIGURATION_TABLE *)malloc (sizeof (EFI_CONFIGURATION_TABLE) * LOADED_IMAGE_TEST_GST_TABLES_MAX);
    //
    // Set BootServices to a non-NULL sentinel so the library's post-ExitBootServices
    // guard does not treat this stub as a post-ExitBootServices state.
    // The library checks BootServices for NULL but never dereferences it here.
    //
    gST->BootServices = (EFI_BOOT_SERVICES *)gST;
  }

  /** Add a ConfigTable entry to the stub gST */
  void
  AddConfigTable (
    EFI_GUID  VendorGuid,
    VOID      *TablePtr
    )
  {
    ASSERT (gST->NumberOfTableEntries + 1 <= LOADED_IMAGE_TEST_GST_TABLES_MAX);

    gST->ConfigurationTable[gST->NumberOfTableEntries].VendorGuid  = VendorGuid;
    gST->ConfigurationTable[gST->NumberOfTableEntries].VendorTable = TablePtr;
    gST->NumberOfTableEntries++;
  }

  /** Malloc a debug table and return it.  We'll only support one. */
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER *
  StubDebugTableHeader (
    )
  {
    DebugTableHeader = (EFI_DEBUG_IMAGE_INFO_TABLE_HEADER *)malloc (sizeof (EFI_DEBUG_IMAGE_INFO_TABLE_HEADER));
    memset (DebugTableHeader, 0, sizeof (EFI_DEBUG_IMAGE_INFO_TABLE_HEADER));

    return DebugTableHeader;
  }

  /** Add a DebugInfo entry with a null NormalImage */
  void
  AddNullDebugInfo (
    EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader
    )
  {
    EFI_DEBUG_IMAGE_INFO  *InfoTable;

    ASSERT (DebugTableHeader->TableSize + 1 <= LOADED_IMAGE_TEST_DEBUG_TABLES_MAX);

    if (DebugTableHeader->EfiDebugImageInfoTable == NULL) {
      DebugTableHeader->EfiDebugImageInfoTable = (EFI_DEBUG_IMAGE_INFO *)malloc (sizeof (EFI_DEBUG_IMAGE_INFO) * LOADED_IMAGE_TEST_DEBUG_TABLES_MAX);
    }

    InfoTable                                          = DebugTableHeader->EfiDebugImageInfoTable;
    InfoTable[DebugTableHeader->TableSize].NormalImage = NULL;
    DebugTableHeader->TableSize++;
  }

  /** Add a DebugInfo entry */
  void
  AddDebugInfo (
    EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader,
    UINT32                             ImageInfoType,
    EFI_LOADED_IMAGE_PROTOCOL          *LoadedImageProtocolInstance
    )
  {
    EFI_DEBUG_IMAGE_INFO  *InfoTable;

    ASSERT (DebugTableHeader->TableSize + 1 <= LOADED_IMAGE_TEST_DEBUG_TABLES_MAX);

    if (DebugTableHeader->EfiDebugImageInfoTable == NULL) {
      DebugTableHeader->EfiDebugImageInfoTable = (EFI_DEBUG_IMAGE_INFO *)malloc (sizeof (EFI_DEBUG_IMAGE_INFO) * LOADED_IMAGE_TEST_DEBUG_TABLES_MAX);
    }

    InfoTable                                                                       = DebugTableHeader->EfiDebugImageInfoTable;
    InfoTable[DebugTableHeader->TableSize].NormalImage                              = (EFI_DEBUG_IMAGE_INFO_NORMAL *)malloc (sizeof (EFI_DEBUG_IMAGE_INFO_NORMAL));
    InfoTable[DebugTableHeader->TableSize].NormalImage->ImageInfoType               = ImageInfoType;
    InfoTable[DebugTableHeader->TableSize].NormalImage->LoadedImageProtocolInstance = LoadedImageProtocolInstance;
    DebugTableHeader->TableSize++;
  }
};

//////////////////////////////////////////////////////////////////////////////
// ImageInfoGetSystemConfigurationTable tests
//

extern "C" {
  EFI_STATUS
  ImageInfoGetSystemConfigurationTable (
    IN  EFI_GUID  *TableGuid,
    OUT VOID      **Table
    );
}

TEST_F (ImageInfoLibTest, ImageInfoGetSystemConfig_Empty) {
  EFI_GUID    TableGuid = { 56 };
  CHAR8       Table;
  VOID        *TablePtr = &Table;
  EFI_STATUS  Status;

  ImageInfoLibTest::StubGST ();
  Status = ImageInfoGetSystemConfigurationTable (&TableGuid, &TablePtr);

  // Return is "not found"
  EXPECT_EQ (Status, EFI_NOT_FOUND);

  // Inputs are not touched
  EXPECT_EQ (TableGuid.Data1, 56U);
  EXPECT_EQ (TablePtr, (VOID *)&Table);
}

TEST_F (ImageInfoLibTest, ImageInfoGetSystemConfig_NotFound) {
  EFI_GUID    TableGuid    = { 56 };
  EFI_GUID    NotFoundGuid = { 58 };
  CHAR8       Table;
  VOID        *TablePtr = &Table;
  EFI_STATUS  Status;

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (TableGuid, &Table);
  Status = ImageInfoGetSystemConfigurationTable (&NotFoundGuid, &TablePtr);

  // Return is "not found"
  EXPECT_EQ (Status, EFI_NOT_FOUND);

  // Inputs are not touched
  EXPECT_EQ (NotFoundGuid.Data1, 58U);
  EXPECT_EQ (TablePtr, (VOID *)&Table);
}

TEST_F (ImageInfoLibTest, ImageInfoGetSystemConfig_Success) {
  EFI_GUID    TableGuid0 = { 56 };
  EFI_GUID    TableGuid1 = { 58 };
  EFI_GUID    TableGuid2 = { 42 };
  CHAR8       Table0, Table1, Table2;
  VOID        *TablePtr;
  EFI_STATUS  Status;

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (TableGuid0, &Table0);
  ImageInfoLibTest::AddConfigTable (TableGuid1, &Table1);
  ImageInfoLibTest::AddConfigTable (TableGuid2, &Table2);

  Status = ImageInfoGetSystemConfigurationTable (&TableGuid0, &TablePtr);
  EXPECT_EQ (Status, EFI_SUCCESS);
  EXPECT_EQ (TableGuid0.Data1, 56U);
  EXPECT_EQ (TablePtr, (VOID *)&Table0);

  Status = ImageInfoGetSystemConfigurationTable (&TableGuid1, &TablePtr);
  EXPECT_EQ (Status, EFI_SUCCESS);
  EXPECT_EQ (TableGuid1.Data1, 58U);
  EXPECT_EQ (TablePtr, (VOID *)&Table1);

  Status = ImageInfoGetSystemConfigurationTable (&TableGuid2, &TablePtr);
  EXPECT_EQ (Status, EFI_SUCCESS);
  EXPECT_EQ (TableGuid2.Data1, 42U);
  EXPECT_EQ (TablePtr, (VOID *)&Table2);
}

//////////////////////////////////////////////////////////////////////////////
// GetLoadedImageBase tests
//

extern "C" {
  UINTN
  GetLoadedImageBase (
    IN  UINTN  Address
    );
}

TEST_F (ImageInfoLibTest, GetLoadedImageBase_NotFound) {
  UINTN  Result;

  ImageInfoLibTest::StubGST ();
  Result = GetLoadedImageBase (0x1000);
  EXPECT_EQ (Result, 0ULL);
}

TEST_F (ImageInfoLibTest, GetLoadedImageBase_NoDebugInfo) {
  UINTN                              Result;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  Result = GetLoadedImageBase (0x1000);
  EXPECT_EQ (Result, 0ULL);
}

TEST_F (ImageInfoLibTest, GetLoadedImageBase_WithNulls) {
  UINTN                              Result;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddNullDebugInfo (DebugTableHeader);
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, NULL);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  Result = GetLoadedImageBase (0x1000);
  EXPECT_EQ (Result, 0ULL);
}

TEST_F (ImageInfoLibTest, GetLoadedImageBase_Success) {
  UINTN                              Result;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_LOADED_IMAGE_PROTOCOL          Image1 = { };
  EFI_LOADED_IMAGE_PROTOCOL          Image2 = { };
  EFI_LOADED_IMAGE_PROTOCOL          Image3 = { };

  Image1.ImageBase = (VOID *)0x400000;
  Image1.ImageSize = 0x10000;
  Image2.ImageBase = (VOID *)0x500000;
  Image2.ImageSize = 0x100000;
  Image3.ImageBase = (VOID *)0x600000;
  Image3.ImageSize = 0x100000;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddNullDebugInfo (DebugTableHeader);
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image1);
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image2);
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image3);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  // Check an address before any images
  Result = GetLoadedImageBase (0x1000);
  EXPECT_EQ (Result, 0ULL);

  // Check an address at the beginning of the first image.
  Result = GetLoadedImageBase (0x400000);
  EXPECT_EQ (Result, 0x400000ULL);

  // Check an address inside the first image.
  Result = GetLoadedImageBase (0x400042);
  EXPECT_EQ (Result, 0x400000ULL);

  // Check the last valid byte of the first image.
  Result = GetLoadedImageBase (0x40FFFF);
  EXPECT_EQ (Result, 0x400000ULL);

  // Check an address immediately after the first image.
  Result = GetLoadedImageBase (0x410000);
  EXPECT_EQ (Result, 0ULL);

  // Check an address between images.
  Result = GetLoadedImageBase (0x410042);
  EXPECT_EQ (Result, 0ULL);

  // Check an address inside the second image.
  Result = GetLoadedImageBase (0x510042);
  EXPECT_EQ (Result, 0x500000ULL);

  // Check an address at the end of the second image.
  Result = GetLoadedImageBase (0x5fffff);
  EXPECT_EQ (Result, 0x500000ULL);

  // Check an address at inside the third image.
  Result = GetLoadedImageBase (0x600100);
  EXPECT_EQ (Result, 0x600000ULL);

  // Check the last valid byte of the third image.
  Result = GetLoadedImageBase (0x6FFFFF);
  EXPECT_EQ (Result, 0x600000ULL);

  // Check an address immediately after the third image.
  Result = GetLoadedImageBase (0x700000);
  EXPECT_EQ (Result, 0ULL);

  // Check an address after the third image.
  Result = GetLoadedImageBase (0x700001);
  EXPECT_EQ (Result, 0ULL);
}

//////////////////////////////////////////////////////////////////////////////
// GetImageName tests
//
// Tests for the not-found path exercise the output-initialization guarantee
// without touching PE/COFF memory.
//
// Tests for the found path rely on --wrap to intercept PeCoffGetSizeOfHeaders
// and PeCoffLoaderGetPdbPointer so that no real PE/COFF image data is needed.
//

// Not found — both outputs initialized to 0.
TEST_F (ImageInfoLibTest, GetImageName_NotFound_BothOutputsProvided) {
  UINTN  ImageBase           = 0xDEADBEEF;
  UINTN  PeCoffSizeOfHeaders = 0xDEADBEEF;
  CHAR8  *Name;

  ImageInfoLibTest::StubGST ();
  Name = GetImageName (0x1000, &ImageBase, &PeCoffSizeOfHeaders);

  EXPECT_EQ (Name, nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
  EXPECT_EQ (PeCoffSizeOfHeaders, 0ULL);
}

// Not found — only ImageBase provided; still initialized to 0.
TEST_F (ImageInfoLibTest, GetImageName_NotFound_NullPeCoffSize) {
  UINTN  ImageBase = 0xDEADBEEF;
  CHAR8  *Name;

  ImageInfoLibTest::StubGST ();
  Name = GetImageName (0x1000, &ImageBase, NULL);

  EXPECT_EQ (Name, nullptr);
  EXPECT_EQ (ImageBase, 0ULL);
}

// Not found — only PeCoffSizeOfHeaders provided; still initialized to 0.
TEST_F (ImageInfoLibTest, GetImageName_NotFound_NullImageBase) {
  UINTN  PeCoffSizeOfHeaders = 0xDEADBEEF;
  CHAR8  *Name;

  ImageInfoLibTest::StubGST ();
  Name = GetImageName (0x1000, NULL, &PeCoffSizeOfHeaders);

  EXPECT_EQ (Name, nullptr);
  EXPECT_EQ (PeCoffSizeOfHeaders, 0ULL);
}

// Not found — both outputs NULL; no crash.
TEST_F (ImageInfoLibTest, GetImageName_NotFound_BothNull) {
  ImageInfoLibTest::StubGST ();
  EXPECT_EQ (GetImageName (0x1000, NULL, NULL), nullptr);
}

// Found — both outputs provided.
TEST_F (ImageInfoLibTest, GetImageName_Found_BothOutputsProvided) {
  UINTN                              ImageBase           = 0;
  UINTN                              PeCoffSizeOfHeaders = 0;
  CHAR8                              *Name;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_LOADED_IMAGE_PROTOCOL          Image = { };

  Image.ImageBase = (VOID *)0x400000;
  Image.ImageSize = 0x10000;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  Name = GetImageName (0x400042, &ImageBase, &PeCoffSizeOfHeaders);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, 0x400000ULL);
  EXPECT_EQ (PeCoffSizeOfHeaders, (UINTN)0x200);
}

// Found — ImageBase NULL; PeCoffSizeOfHeaders still set, no crash.
TEST_F (ImageInfoLibTest, GetImageName_Found_NullImageBase) {
  UINTN                              PeCoffSizeOfHeaders = 0;
  CHAR8                              *Name;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_LOADED_IMAGE_PROTOCOL          Image = { };

  Image.ImageBase = (VOID *)0x400000;
  Image.ImageSize = 0x10000;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  Name = GetImageName (0x400042, NULL, &PeCoffSizeOfHeaders);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (PeCoffSizeOfHeaders, (UINTN)0x200);
}

// Found — PeCoffSizeOfHeaders NULL; ImageBase still set, no crash.
TEST_F (ImageInfoLibTest, GetImageName_Found_NullPeCoffSize) {
  UINTN                              ImageBase = 0;
  CHAR8                              *Name;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_LOADED_IMAGE_PROTOCOL          Image = { };

  Image.ImageBase = (VOID *)0x400000;
  Image.ImageSize = 0x10000;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  Name = GetImageName (0x400042, &ImageBase, NULL);

  EXPECT_STREQ (Name, "TestImage.efi");
  EXPECT_EQ (ImageBase, 0x400000ULL);
}

// Found — both outputs NULL; image name still returned, no crash.
TEST_F (ImageInfoLibTest, GetImageName_Found_BothNull) {
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_LOADED_IMAGE_PROTOCOL          Image = { };

  Image.ImageBase = (VOID *)0x400000;
  Image.ImageSize = 0x10000;

  DebugTableHeader = ImageInfoLibTest::StubDebugTableHeader ();
  ImageInfoLibTest::AddDebugInfo (DebugTableHeader, EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL, &Image);

  ImageInfoLibTest::StubGST ();
  ImageInfoLibTest::AddConfigTable (gEfiDebugImageInfoTableGuid, DebugTableHeader);

  EXPECT_STREQ (GetImageName (0x400042, NULL, NULL), "TestImage.efi");
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
