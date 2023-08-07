/** @file

  Redfish feature driver implementation - internal header file

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_SECURE_BOOT_DATABASE_COLLECTION_H_
#define EFI_REDFISH_SECURE_BOOT_DATABASE_COLLECTION_H_

#include <RedfishJsonStructure/SecureBootDatabaseCollection/EfiSecureBootDatabaseCollection.h>
#include <RedfishCollectionCommon.h>

#define REDFISH_SCHEMA_NAME  "SecureBootDatabaseCollection"
#define REDFISH_MANAGED_URI  L"Systems/{}/SecureBoot/SecureBootDatabases"
#define MAX_URI_LENGTH       256

#endif
