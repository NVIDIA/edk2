/** @file
  This file defines the EFI Redfish C Structure Interpreter for BootOption v1_0_0

  (C) Copyright 2019-2021 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  Auto-generated file by Redfish Schema C Structure Generator.
  https://github.com/DMTF/Redfish-Schema-C-Struct-Generator
  
  Copyright Notice:
  Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md  

**/

#ifndef EFI_REDFISH_INTERP_BOOTOPTION_V1_0_0_H__
#define EFI_REDFISH_INTERP_BOOTOPTION_V1_0_0_H__

#include <Uefi.h>
#include <Protocol/RestJsonStructure.h>
#include "ConverterLib/edk2library/BootOption/v1_0_0/Redfish_BootOption_v1_0_0_CS.h"
//
// EFI structure of Redfish BootOption v1_0_0
//
typedef struct _EFI_REDFISH_BOOTOPTION_V1_0_0 {
  EFI_REST_JSON_STRUCTURE_HEADER Header;
  EFI_REDFISH_BOOTOPTION_V1_0_0_CS *BootOption;
} EFI_REDFISH_BOOTOPTION_V1_0_0;

#endif
