/** @file
  CXL 3.1 definitions

  This file contains the register definitions and firmware interface based
  on the Compute Express Link (CXL) Specification Revision 3.1.

  Copyright (c) 2024, Phytium Technology Co Ltd. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - Compute Express Link (CXL) Specification Revision 3.1.
    (https://computeexpresslink.org/cxl-specification/)

**/

#ifndef CXL31_H_
#define CXL31_H_

#include <IndustryStandard/Cxl30.h>

//
// "CEDT" CXL Early Discovery Table
// Compute Express Link Specification Revision 3.1  - Chapter 9.18.1
//
#define CXL_EARLY_DISCOVERY_TABLE_REVISION_02  0x2

#define CEDT_TYPE_CSDS  0x4

///
/// CEDT CXL CSDS System Description Structure System Capabilities
// Compute Express Link Specification Revision 3.1  - Chapter 9.18.1.6
///
#define CXL_SYSTEM_DESCRIPTION_STRUCTURE_SYSTEM_CAPABILITIES_CMP_M               BIT0
#define CXL_SYSTEM_DESCRIPTION_STRUCTURE_SYSTEM_CAPABILITIES_NO_CLEAN_WRITEBACK  BIT1

//
// Ensure proper structure formats
//
#pragma pack(1)

//
// Definition for CXL System Description Structure (CSDS)
// Compute Express Link Specification Revision 3.1  - Chapter 9.18.1.6
//
typedef struct {
  CEDT_STRUCTURE    Header;
  UINT16            Capabilities;
  UINT16            Reserved;
} CXL_SYSTEM_DESCRIPTION_STRUCTURE;

#pragma pack()

#endif
