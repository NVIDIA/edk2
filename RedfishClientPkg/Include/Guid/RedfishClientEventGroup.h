/** @file
  GUID for Redfish Client Event Group GUID

  (C) Copyright 2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_CLIENT_EVENT_GROUP_GUID_H_
#define REDFISH_CLIENT_EVENT_GROUP_GUID_H_

#define REDFISH_CLIENT_FEATURE_READY_TO_PROVISIONING_GUID \
  { \
    0x77E4FC1C, 0x2428, 0x47EE, { 0x9E, 0xEC, 0x8B, 0x77, 0xEF, 0x9D, 0x4E, 0xF0 } \
  }

extern EFI_GUID gEfiRedfishClientFeatureReadyToProvisioningGuid;

#define REDFISH_CLIENT_FEATURE_AFTER_PROVISIONING_GUID \
  { \
    0xE547CB6F, 0x306F, 0x4226, { 0xAB, 0x70, 0xA0, 0x6E, 0x26, 0xF1, 0x2E, 0xD0 } \
  }

extern EFI_GUID gEfiRedfishClientFeatureAfterProvisioningGuid;

#endif
