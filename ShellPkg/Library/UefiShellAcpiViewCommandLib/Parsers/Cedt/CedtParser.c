/** @file
  CEDT table parser

  Copyright (c) 2025, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
    - Compute Express Link Specification Revision 3.2
**/

#include <IndustryStandard/Cxl32.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include "AcpiParser.h"

// Local Variables
STATIC CONST UINT8                   *CedtStructureType;
STATIC CONST UINT16                  *CedtStructureLength;
STATIC ACPI_DESCRIPTION_HEADER_INFO  AcpiHdrInfo;

// Defines for structure field offsets and sizes
#define CEDT_STRUCTURE_NAME_BUFFER_SIZE  80
#define CFMWS_FIXED_FIELDS_SIZE          36
#define CXIMS_NIB_OFFSET                 7
#define CXIMS_XORMAP_OFFSET              8
#define CXIMS_MAX_NIB_VALUE              4

/**
  An ACPI_PARSER array describing the CEDT Table.
**/
STATIC CONST ACPI_PARSER  CedtParser[] = {
  PARSE_ACPI_HEADER (&AcpiHdrInfo)
};

/**
  An ACPI_PARSER array describing the CEDT structure header.
**/
STATIC CONST ACPI_PARSER  CedtStructureHeaderParser[] = {
  { L"Type",     1, 0, L"0x%x", NULL, (VOID **)&CedtStructureType,   NULL, NULL },
  { L"Reserved", 1, 1, L"0x%x", NULL, NULL,                          NULL, NULL },
  { L"Length",   2, 2, L"%d",   NULL, (VOID **)&CedtStructureLength, NULL, NULL }
};

/**
  An ACPI_PARSER array describing the CXL Host Bridge Structure (CHBS).
**/
STATIC CONST ACPI_PARSER  CedtChbsParser[] = {
  { L"Type",        1, 0,  L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Reserved",    1, 1,  L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Length",      2, 2,  L"%d",    NULL, NULL, NULL, NULL },
  { L"UID",         4, 4,  L"0x%x",  NULL, NULL, NULL, NULL },
  { L"CXL Version", 4, 8,  L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Reserved",    4, 12, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Base",        8, 16, L"0x%lx", NULL, NULL, NULL, NULL },
  { L"Length",      8, 24, L"0x%lx", NULL, NULL, NULL, NULL }
};

/**
  This function traces the interleave ways field for CFMWS.

  @param [in] Format  Format string for tracing the data.
  @param [in] Ptr     Pointer to the start of the buffer.
  @param [in] Length  Length of the field.
**/
STATIC
VOID
EFIAPI
DumpCfmwsInterleaveWays (
  IN CONST CHAR16  *Format,
  IN UINT8         *Ptr,
  IN UINT32        Length
  )
{
  UINT8  EncodedWays;
  UINT8  ActualWays;

  EncodedWays = *Ptr;

  switch (EncodedWays) {
    case CXL_HDM_1_WAY_INTERLEAVING:
      ActualWays = 1;
      break;
    case CXL_HDM_2_WAY_INTERLEAVING:
      ActualWays = 2;
      break;
    case CXL_HDM_3_WAY_INTERLEAVING:
      ActualWays = 3;
      break;
    case CXL_HDM_4_WAY_INTERLEAVING:
      ActualWays = 4;
      break;
    case CXL_HDM_6_WAY_INTERLEAVING:
      ActualWays = 6;
      break;
    case CXL_HDM_8_WAY_INTERLEAVING:
      ActualWays = 8;
      break;
    case CXL_HDM_12_WAY_INTERLEAVING:
      ActualWays = 12;
      break;
    case CXL_HDM_16_WAY_INTERLEAVING:
      ActualWays = 16;
      break;
    default:
      Print (L"0x%x (Invalid)", EncodedWays);
      IncrementErrorCount ();
      return;
  }

  Print (L"0x%x (%d-way)", EncodedWays, ActualWays);
}

/**
  This function traces the interleave granularity field for CFMWS.

  @param [in] Format  Format string for tracing the data.
  @param [in] Ptr     Pointer to the start of the buffer.
  @param [in] Length  Length of the field.
**/
STATIC
VOID
EFIAPI
DumpCfmwsGranularity (
  IN CONST CHAR16  *Format,
  IN UINT8         *Ptr,
  IN UINT32        Length
  )
{
  UINT32  EncodedGranularity;
  UINT32  ActualGranularity;

  EncodedGranularity = *(UINT32 *)Ptr;

  switch (EncodedGranularity) {
    case CXL_INTERLEAVE_GRANULARITY_256B:
      ActualGranularity = 256;
      break;
    case CXL_INTERLEAVE_GRANULARITY_512B:
      ActualGranularity = 512;
      break;
    case CXL_INTERLEAVE_GRANULARITY_1024B:
      ActualGranularity = 1024;
      break;
    case CXL_INTERLEAVE_GRANULARITY_2048B:
      ActualGranularity = 2048;
      break;
    case CXL_INTERLEAVE_GRANULARITY_4096B:
      ActualGranularity = 4096;
      break;
    case CXL_INTERLEAVE_GRANULARITY_8192B:
      ActualGranularity = 8192;
      break;
    case CXL_INTERLEAVE_GRANULARITY_16384B:
      ActualGranularity = 16384;
      break;
    default:
      Print (L"0x%x (Invalid)", EncodedGranularity);
      IncrementErrorCount ();
      return;
  }

  Print (L"0x%x (%d bytes)", EncodedGranularity, ActualGranularity);
}

/**
  This function traces the Window Restrictions field for CFMWS.

  @param [in] Format  Format string for tracing the data.
  @param [in] Ptr     Pointer to the start of the buffer.
  @param [in] Length  Length of the field.
**/
STATIC
VOID
EFIAPI
DumpCfmwsWindowRestrictions (
  IN CONST CHAR16  *Format,
  IN UINT8         *Ptr,
  IN UINT32        Length
  )
{
  UINT16  Restrictions;

  Restrictions = *(UINT16 *)Ptr;

  Print (L"0x%x\n", Restrictions);

  // Decode each bit
  Print (L"    Bit 0 (Device Coherent):              %d\n", (Restrictions & BIT0) ? 1 : 0);
  Print (L"    Bit 1 (Host-Only Coherent):           %d\n", (Restrictions & BIT1) ? 1 : 0);
  Print (L"    Bit 2 (Volatile):                     %d\n", (Restrictions & BIT2) ? 1 : 0);
  Print (L"    Bit 3 (Persistent):                   %d\n", (Restrictions & BIT3) ? 1 : 0);
  Print (L"    Bit 4 (Fixed Device Configuration):   %d\n", (Restrictions & BIT4) ? 1 : 0);
}

/**
  This function traces the interleave target list for CFMWS.

  @param [in] Format  Format string for tracing the data.
  @param [in] Ptr     Pointer to the start of the buffer.
  @param [in] Length  Length of the field.
**/
STATIC
VOID
EFIAPI
DumpCfmwsTargetList (
  IN CONST CHAR16  *Format,
  IN UINT8         *Ptr,
  IN UINT32        Length
  )
{
  UINT32  Index;
  UINT32  *TargetList;

  Print (L"\n");

  TargetList = (UINT32 *)Ptr;
  for (Index = 0; Index < Length / sizeof (UINT32); Index++) {
    Print (L"    Target[%d]: 0x%x\n", Index, TargetList[Index]);
  }
}

/**
  An ACPI_PARSER array describing the CXL Fixed Memory Window Structure (CFMWS).
**/
STATIC CONST ACPI_PARSER  CedtCfmwsParser[] = {
  { L"Type",                    1, 0,  L"0x%x",  NULL,                        NULL, NULL, NULL },
  { L"Reserved",                1, 1,  L"0x%x",  NULL,                        NULL, NULL, NULL },
  { L"Length",                  2, 2,  L"%d",    NULL,                        NULL, NULL, NULL },
  { L"Reserved",                4, 4,  L"0x%x",  NULL,                        NULL, NULL, NULL },
  { L"Base HPA",                8, 8,  L"0x%lx", NULL,                        NULL, NULL, NULL },
  { L"Window Size",             8, 16, L"0x%lx", NULL,                        NULL, NULL, NULL },
  { L"Encoded Interleave Ways", 1, 24, NULL,     DumpCfmwsInterleaveWays,     NULL, NULL, NULL },
  { L"Interleave Arithmetic",   1, 25, L"0x%x",  NULL,                        NULL, NULL, NULL },
  { L"Reserved",                2, 26, L"0x%x",  NULL,                        NULL, NULL, NULL },
  { L"HBIG",                    4, 28, NULL,     DumpCfmwsGranularity,        NULL, NULL, NULL },
  { L"Window Restrictions",     2, 32, NULL,     DumpCfmwsWindowRestrictions, NULL, NULL, NULL },
  { L"QTG ID",                  2, 34, L"0x%x",  NULL,                        NULL, NULL, NULL }
  // Note: Interleave Target List follows but is variable length
};

/**
  An ACPI_PARSER array describing the CXL XOR Interleave Math Structure (CXIMS).
**/
STATIC CONST ACPI_PARSER  CedtCximsParser[] = {
  { L"Type",     1, 0, L"0x%x", NULL, NULL, NULL, NULL },
  { L"Reserved", 1, 1, L"0x%x", NULL, NULL, NULL, NULL },
  { L"Length",   2, 2, L"%d",   NULL, NULL, NULL, NULL },
  { L"Reserved", 2, 4, L"0x%x", NULL, NULL, NULL, NULL },
  { L"HBIG",     1, 6, L"0x%x", NULL, NULL, NULL, NULL },
  { L"NIB",      1, 7, L"0x%x", NULL, NULL, NULL, NULL }
  // Note: XORMAP List follows but is variable length (up to 4 entries)
};

/**
  An ACPI_PARSER array describing the RCEC Downstream Port Association Structure (RDPAS).
**/
STATIC CONST ACPI_PARSER  CedtRdpasParser[] = {
  { L"Type",           1, 0, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Reserved",       1, 1, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Length",         2, 2, L"%d",    NULL, NULL, NULL, NULL },
  { L"Segment Number", 2, 4, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"BDF",            2, 6, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Protocol Type",  1, 8, L"0x%x",  NULL, NULL, NULL, NULL },
  { L"Base Address",   8, 9, L"0x%lx", NULL, NULL, NULL, NULL }
};

/**
  This function traces the Capabilities field for CSDS.

  @param [in] Format  Format string for tracing the data.
  @param [in] Ptr     Pointer to the start of the buffer.
  @param [in] Length  Length of the field.
**/
STATIC
VOID
EFIAPI
DumpCsdsCapabilities (
  IN CONST CHAR16  *Format,
  IN UINT8         *Ptr,
  IN UINT32        Length
  )
{
  UINT16  Capabilities;

  Capabilities = *(UINT16 *)Ptr;

  Print (L"0x%x\n", Capabilities);

  // Decode each bit (CXL 3.1 and 3.2)
  Print (L"    Bit 0 (CMP-M):                        %d\n", (Capabilities & BIT0) ? 1 : 0);
  Print (L"    Bit 1 (No Clean Writeback):           %d\n", (Capabilities & BIT1) ? 1 : 0);
  Print (L"    Bit 2 (Viral Policy):                 %d\n", (Capabilities & BIT2) ? 1 : 0);
}

/**
  An ACPI_PARSER array describing the CXL System Description Structure (CSDS).
**/
STATIC CONST ACPI_PARSER  CedtCsdsParser[] = {
  { L"Type",         1, 0, L"0x%x", NULL,                 NULL, NULL, NULL },
  { L"Reserved",     1, 1, L"0x%x", NULL,                 NULL, NULL, NULL },
  { L"Length",       2, 2, L"%d",   NULL,                 NULL, NULL, NULL },
  { L"Capabilities", 2, 4, NULL,    DumpCsdsCapabilities, NULL, NULL, NULL },
  { L"Reserved",     2, 6, L"0x%x", NULL,                 NULL, NULL, NULL }
};

/**
  This function parses the ACPI CEDT table.
  When trace is enabled this function parses the CEDT table and
  traces the ACPI table fields.

  This function parses the following CXL structures:
    - CXL Host Bridge Structure (CHBS)
    - CXL Fixed Memory Window Structure (CFMWS)
    - CXL XOR Interleave Math Structure (CXIMS)
    - RCEC Downstream Port Association Structure (RDPAS)
    - CXL System Description Structure (CSDS)

  This function also performs validation of the ACPI table fields.

  @param [in] Trace              If TRUE, trace the ACPI fields.
  @param [in] Ptr                Pointer to the start of the buffer.
  @param [in] AcpiTableLength    Length of the ACPI table.
  @param [in] AcpiTableRevision  Revision of the ACPI table.
**/
VOID
EFIAPI
ParseAcpiCedt (
  IN BOOLEAN  Trace,
  IN UINT8    *Ptr,
  IN UINT32   AcpiTableLength,
  IN UINT8    AcpiTableRevision
  )
{
  UINT32  Offset;
  UINT8   *StructurePtr;
  UINT32  ChbsIndex;
  UINT32  CfmwsIndex;
  UINT32  CximsIndex;
  UINT32  RdpasIndex;
  UINT32  CsdsIndex;
  CHAR8   Buffer[CEDT_STRUCTURE_NAME_BUFFER_SIZE]; // Used for AsciiName param of ParseAcpi

  ChbsIndex  = 0;
  CfmwsIndex = 0;
  CximsIndex = 0;
  RdpasIndex = 0;
  CsdsIndex  = 0;

  if (!Trace) {
    return;
  }

  Offset = ParseAcpi (
             TRUE,
             0,
             "CEDT",
             Ptr,
             AcpiTableLength,
             PARSER_PARAMS (CedtParser)
             );

  StructurePtr = Ptr + Offset;

  while (Offset < AcpiTableLength) {
    // Parse structure header to get type and length
    ParseAcpi (
      FALSE,
      0,
      NULL,
      StructurePtr,
      AcpiTableLength - Offset,
      PARSER_PARAMS (CedtStructureHeaderParser)
      );

    // Check if the values used to control the parsing logic have been
    // successfully read.
    if ((CedtStructureType == NULL) ||
        (CedtStructureLength == NULL))
    {
      IncrementErrorCount ();
      Print (
        L"ERROR: Insufficient remaining table buffer length to read the " \
        L"CEDT structure header. Length = %d.\n",
        AcpiTableLength - Offset
        );
      return;
    }

    // Validate Structure length
    if ((*CedtStructureLength == 0) ||
        ((Offset + (*CedtStructureLength)) > AcpiTableLength))
    {
      IncrementErrorCount ();
      Print (
        L"ERROR: Invalid CEDT Structure length. " \
        L"Length = %d. Offset = %d. AcpiTableLength = %d.\n",
        *CedtStructureLength,
        Offset,
        AcpiTableLength
        );
      return;
    }

    switch (*CedtStructureType) {
      case CEDT_TYPE_CHBS:
        AsciiSPrint (
          Buffer,
          sizeof (Buffer),
          "CXL Host Bridge Structure [%d]",
          ChbsIndex++
          );
        ParseAcpi (
          TRUE,
          2,
          Buffer,
          StructurePtr,
          *CedtStructureLength,
          PARSER_PARAMS (CedtChbsParser)
          );
        break;

      case CEDT_TYPE_CFMWS:
        if (AcpiTableRevision < CXL_EARLY_DISCOVERY_TABLE_REVISION_02) {
          IncrementErrorCount ();
          Print (
            L"ERROR: CFMWS structure not supported in CEDT revision %d\n",
            AcpiTableRevision
            );
        } else {
          UINT32  TargetListLength;

          AsciiSPrint (
            Buffer,
            sizeof (Buffer),
            "CXL Fixed Memory Window Structure [%d]",
            CfmwsIndex++
            );

          // Parse fixed part of CFMWS
          ParseAcpi (
            TRUE,
            2,
            Buffer,
            StructurePtr,
            *CedtStructureLength,
            PARSER_PARAMS (CedtCfmwsParser)
            );

          // Parse variable target list
          if (*CedtStructureLength > CFMWS_FIXED_FIELDS_SIZE) {
            TargetListLength = *CedtStructureLength - CFMWS_FIXED_FIELDS_SIZE;
            Print (L"  ");
            PrintFieldName (2, L"Interleave Target List");
            DumpCfmwsTargetList (
              NULL,
              StructurePtr + CFMWS_FIXED_FIELDS_SIZE,
              TargetListLength
              );
          }
        }

        break;

      case CEDT_TYPE_CXIMS:
        if (AcpiTableRevision < CXL_EARLY_DISCOVERY_TABLE_REVISION_02) {
          IncrementErrorCount ();
          Print (
            L"ERROR: CXIMS structure not supported in CEDT revision %d\n",
            AcpiTableRevision
            );
        } else {
          UINT8   NIB;
          UINT32  XormapCount;
          UINT32  XormapIndex;

          AsciiSPrint (
            Buffer,
            sizeof (Buffer),
            "CXL XOR Interleave Math Structure [%d]",
            CximsIndex++
            );
          ParseAcpi (
            TRUE,
            2,
            Buffer,
            StructurePtr,
            *CedtStructureLength,
            PARSER_PARAMS (CedtCximsParser)
            );

          // Parse XORMAP list
          NIB         = *(StructurePtr + CXIMS_NIB_OFFSET);
          XormapCount = (NIB <= CXIMS_MAX_NIB_VALUE) ? NIB : CXIMS_MAX_NIB_VALUE;

          Print (L"\n");
          for (XormapIndex = 0; XormapIndex < XormapCount; XormapIndex++) {
            UINT64  *XormapPtr;
            XormapPtr = (UINT64 *)(StructurePtr + CXIMS_XORMAP_OFFSET + (XormapIndex * sizeof (UINT64)));
            Print (L"    XORMAP[%d]: 0x%lx\n", XormapIndex, *XormapPtr);
          }
        }

        break;

      case CEDT_TYPE_RDPAS:
        if (AcpiTableRevision < CXL_EARLY_DISCOVERY_TABLE_REVISION_02) {
          IncrementErrorCount ();
          Print (
            L"ERROR: RDPAS structure not supported in CEDT revision %d\n",
            AcpiTableRevision
            );
        } else {
          AsciiSPrint (
            Buffer,
            sizeof (Buffer),
            "RCEC Downstream Port Association Structure [%d]",
            RdpasIndex++
            );
          ParseAcpi (
            TRUE,
            2,
            Buffer,
            StructurePtr,
            *CedtStructureLength,
            PARSER_PARAMS (CedtRdpasParser)
            );
        }

        break;

      case CEDT_TYPE_CSDS:
        if (AcpiTableRevision < CXL_EARLY_DISCOVERY_TABLE_REVISION_02) {
          IncrementErrorCount ();
          Print (
            L"ERROR: CSDS structure not supported in CEDT revision %d\n",
            AcpiTableRevision
            );
        } else {
          AsciiSPrint (
            Buffer,
            sizeof (Buffer),
            "CXL System Description Structure [%d]",
            CsdsIndex++
            );
          ParseAcpi (
            TRUE,
            2,
            Buffer,
            StructurePtr,
            *CedtStructureLength,
            PARSER_PARAMS (CedtCsdsParser)
            );
        }

        break;

      default:
        IncrementErrorCount ();
        Print (L"ERROR: Unknown CEDT Structure type = 0x%x\n", *CedtStructureType);
        break;
    }

    StructurePtr += (*CedtStructureLength);
    Offset       += (*CedtStructureLength);
  }
}
