/** @file
  This file defines the EDKII_REDFISH_OVERRIDE_PROTOCOL interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EDKII_REDFISH_OVERRIDE_PROTOCOL_H_
#define EDKII_REDFISH_OVERRIDE_PROTOCOL_H_

#include <Uefi.h>

typedef struct _EDKII_REDFISH_OVERRIDE_PROTOCOL EDKII_REDFISH_OVERRIDE_PROTOCOL;

///
/// Definition of EDKII_REDFISH_PHASE_TYPE.
///
typedef enum {
  EdkiiRedfishPhaseBeforeReboot,
  EdkiiRedfishPhaseMax
} EDKII_REDFISH_PHASE_TYPE;

/**
  The callback function to notify platform and provide Redfish phase.

  @param[in] This           Pointer to EDKII_REDFISH_OVERRIDE_PROTOCOL instance.
  @param[in] PhaseType      The type of phase in Redfish operation.

  @retval EFI_SUCCESS       The notify function completed successfully.
  @retval Others            Some errors happened.

**/
typedef
EFI_STATUS
(EFIAPI *EDKII_REDFISH_NOTIFY_PHASE)(
  IN EDKII_REDFISH_OVERRIDE_PROTOCOL  *This,
  IN EDKII_REDFISH_PHASE_TYPE         PhaseType
  );

//
// The definition of _EDKII_REDFISH_OVERRIDE_PROTOCOL
//
struct _EDKII_REDFISH_OVERRIDE_PROTOCOL {
  //
  // Protocol version of this implementation
  //
  UINT32                        Version;
  //
  // Callback to notify Redfish phase
  //
  EDKII_REDFISH_NOTIFY_PHASE    NotifyPhase;
};

#define EDKII_REDFISH_OVERRIDE_PROTOCOL_REVISION  0x00001000

extern EFI_GUID  gEdkiiRedfishOverrideProtocolGuid;

#endif
