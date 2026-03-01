/** @file

  Utilities to query images already loaded in memory.  Primarily for debugging
  purposes.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2011 - 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/DebugImageInfoTable.h>

/**
  Retrieves a pointer to the system configuration table from the EFI System
  Table based on a specified GUID.

  Simplified version of EfiGetSystemConfigurationTable() in UefiLib and copied
  here to avoid a circular library dependency.

  @param[in]  TableGuid   The pointer to table's GUID type.
  @param[out] Table       The pointer to the table associated with TableGuid in
                          the EFI System Table.

  @retval EFI_SUCCESS     A configuration table matching TableGuid was found.
  @retval EFI_NOT_FOUND   A configuration table matching TableGuid could not be
                          found.

**/
EFI_STATUS
ImageInfoGetSystemConfigurationTable (
  IN  EFI_GUID  *TableGuid,
  OUT VOID      **Table
  )
{
  UINTN  Index;

  if (gST == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // After ExitBootServices, gST->BootServices is set to NULL by the firmware.
  // The EFI Debug Image Info Table lives in boot-services memory and may have
  // been reclaimed by the OS.  Return gracefully rather than risk a fault.
  //
  if (gST->BootServices == NULL) {
    return EFI_NOT_FOUND;
  }

  if ((TableGuid == NULL) || (Table == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (TableGuid, &(gST->ConfigurationTable[Index].VendorGuid))) {
      *Table = gST->ConfigurationTable[Index].VendorTable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Use the EFI Debug Image Table to lookup the Address and find which PE/COFF
  image it came from.  Return the base address of the image.

  @param[in]  Address     Address to lookup

  @retval                 Address of the PE/COFF image's base, or 0 if no image
                          is at Address.

**/
UINTN
GetLoadedImageBase (
  IN  UINTN  Address
  )
{
  EFI_STATUS                         Status;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER  *DebugTableHeader;
  EFI_DEBUG_IMAGE_INFO               *DebugTable;
  UINTN                              Entry;

  Status = ImageInfoGetSystemConfigurationTable (&gEfiDebugImageInfoTableGuid, (VOID **)&DebugTableHeader);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Unable to retrieve DebugImageInfoTable: %r\n", __func__, Status));
    return 0;
  }

  DebugTable = DebugTableHeader->EfiDebugImageInfoTable;
  if (DebugTable == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: DebugImageInfoTable was empty\n", __func__));
    return 0;
  }

  for (Entry = 0; Entry < DebugTableHeader->TableSize; Entry++, DebugTable++) {
    if (DebugTable->NormalImage != NULL) {
      if ((DebugTable->NormalImage->ImageInfoType == EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL) &&
          (DebugTable->NormalImage->LoadedImageProtocolInstance != NULL))
      {
        if ((Address >= (UINTN)DebugTable->NormalImage->LoadedImageProtocolInstance->ImageBase) &&
            (Address < ((UINTN)DebugTable->NormalImage->LoadedImageProtocolInstance->ImageBase + DebugTable->NormalImage->LoadedImageProtocolInstance->ImageSize)))
        {
          return (UINTN)DebugTable->NormalImage->LoadedImageProtocolInstance->ImageBase;
        }
      }
    }
  }

  return 0;
}

/**
  Use the EFI Debug Image Table to lookup the Address and find which PE/COFF
  image it came from. As long as the PE/COFF image contains a debug directory
  entry, a string can be returned.

  @param[in]  Address              Address to lookup.
  @param[out] ImageBase            Return the address of the PE/COFF image's base,
                                   if one was found at Address.
  @param[out] PeCoffSizeOfHeaders  Return the size of the PE/COFF header for the image,
                                   if one was found at Address.

  @retval NULL                     Address is not in a loaded PE/COFF image.
  @retval                          Path and file name of PE/COFF image.

**/
CHAR8 *
EFIAPI
GetImageName (
  IN  UINTN  Address,
  OUT UINTN  *ImageBase,
  OUT UINTN  *PeCoffSizeOfHeaders
  )
{
  UINTN  Base;

  if (ImageBase != NULL) {
    *ImageBase = 0;
  }

  if (PeCoffSizeOfHeaders != NULL) {
    *PeCoffSizeOfHeaders = 0;
  }

  Base = GetLoadedImageBase (Address);
  if (Base == 0) {
    return NULL;
  }

  if (ImageBase != NULL) {
    *ImageBase = Base;
  }

  if (PeCoffSizeOfHeaders != NULL) {
    *PeCoffSizeOfHeaders = PeCoffGetSizeOfHeaders ((VOID *)Base);
  }

  return PeCoffLoaderGetPdbPointer ((VOID *)Base);
}
