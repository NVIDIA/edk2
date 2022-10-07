/** @file

  Redfish feature driver implementation - internal header file

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_MEMORY_COLLECTION_H_
#define EFI_REDFISH_MEMORY_COLLECTION_H_

#include <RedfishJsonStructure/MemoryCollection/EfiMemoryCollection.h>
#include <RedfishCollectionCommon.h>

#define REDFISH_SCHEMA_NAME             "Memory"
#define REDFISH_MANAGED_URI             L"Chassis/{}/Memory/{};Systems/{}/Memory/{}"
#define MAX_URI_LENGTH                  256

#endif
