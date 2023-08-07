/** @file

  Redfish feature driver implementation - internal header file
  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef EFI_REDFISH_SECURE_BOOT_DATABASE_COMMON_H_
#define EFI_REDFISH_SECURE_BOOT_DATABASE_COMMON_H_

#include <RedfishJsonStructure/SecureBootDatabase/v1_0_1/EfiSecureBootDatabaseV1_0_1.h>
#include <RedfishResourceCommon.h>
#include <Library/RedfishDebugLib.h>
#include <Library/RedfishSecureBootKeysLib.h>
#include <Protocol/EdkIIRedfishTaskProtocol.h>

//
// Schema information.
//
#define RESOURCE_SCHEMA                     "SecureBootDatabase"
#define RESOURCE_SCHEMA_MAJOR               "1"
#define RESOURCE_SCHEMA_MINOR               "0"
#define RESOURCE_SCHEMA_ERRATA              "1"
#define RESOURCE_SCHEMA_VERSION             "v1_17_0"
#define RESOURCE_SCHEMA_FULL                "x-uefi-redfish-SecureBootDatabase.v1_0_1"
#define REDFISH_SECURE_BOOT_DATABASE_DEBUG  DEBUG_VERBOSE
#define REDFISH_SECURE_BOOT_TASK_DEBUG      DEBUG_VERBOSE

//
// Attributes defined in Certificate and Signature schema
//
#define REDFISH_CERTIFICATE_OWNER                       "UefiSignatureOwner"
#define REDFISH_CERTIFICATE_STRING                      "CertificateString"
#define REDFISH_CERTIFICATE_TYPE                        "CertificateType"
#define REDFISH_SIGNATURE_OWNER                         "UefiSignatureOwner"
#define REDFISH_SIGNATURE_STRING                        "SignatureString"
#define REDFISH_SIGNATURE_TYPE                          "SignatureType"
#define REDFISH_SIGNATURE_REGISTRY                      "SignatureTypeRegistry"
#define REDFISH_SIGNATURE_REGISTRY_VALUE                "UEFI"
#define REDFISH_TASK_LISTEN_URI                         L"/SecureBoot/SecureBootDatabases"
#define REDFISH_TASK_ACTION_KEYWORD                     "Actions"
#define REDFISH_TASK_ACTION_ATTRIBUTE                   "ResetKeysType"
#define REDFISH_TASK_ACTION_DELETE_ALL                  "DeleteAllKeys"
#define REDFISH_TASK_ACTION_RESET_ALL                   "ResetAllKeysToDefault"
#define REDFISH_TASK_TARGET_URI_SEARCH                  "SecureBootDatabases/"
#define REDFISH_TASK_SUPPORTED_CERTIFICATE_TYPE         "PEM"
#define REDFISH_TASK_SUPPORTED_SIGNATURE_TYPE           "EFI_CERT_SHA256_GUID"
#define REDFISH_TASK_FAILURE_NOT_FOUND_MESSAGE          "Cannot find request key"
#define REDFISH_TASK_FAILURE_UNSUPPORTED_MESSAGE        "Request key is unsupported"
#define REDFISH_TASK_FAILURE_ACCESS_DENIED_MESSAGE      "Security violation"
#define REDFISH_TASK_FAILURE_GENERIC_MESSAGE            "Cannot finish request"
#define REDFISH_TASK_FAILURE_INTERNAL_MESSAGE           "UEFI internal failure"
#define REDFISH_TASK_FAILURE_WRONG_CERT_FORMAT_MESSAGE  "Only support PEM type of certificate"
#define REDFISH_TASK_FAILURE_WRONG_SIG_FORMAT_MESSAGE   "Only support EFI_CERT_SHA256_GUID type of signature"

///
/// Definition of REDFISH_TASK_REQUEST_TYPE
///
typedef enum {
  RedfishTaskRequestEnroll = 0x00,
  RedfishTaskRequestDelete,
  RedfishTaskRequestDeleteAll,
  RedfishTaskRequestResetAll
} REDFISH_TASK_REQUEST_TYPE;

///
/// Definition of REDFISH_SECURE_BOOT_TASK_REQUEST
///
typedef struct {
  REDFISH_TASK_PAYLOAD         *Payload;
  REDFISH_TASK_REQUEST_TYPE    RequestType;
  EFI_STRING                   KeyName;
  CHAR8                        *KeyType;
  CHAR8                        *KeyString;
  BOOLEAN                      IsCertificate;
} REDFISH_SECURE_BOOT_TASK_REQUEST;

/**
  The callback function provided by Redfish feature driver.

  @param[in]     TaskId              The Task ID.
  @param[in]     JsonText            The context of task resource in JSON format.
  @param[in]     Context             The context of Redfish feature driver.
  @param[in,out] Result              The pointer to REDFISH_TASK_RESULT.
                                     On input, it is "TaskState" and "TaskStatus" in task resource.
                                     On ouput, it is "TaskState" and "TaskStatus" that will be update
                                     to task resource.

  @retval EFI_SUCCESS              Redfish feature driver callback is executed successfully.
  @retval Others                   Some errors happened.

**/
EFI_STATUS
EFIAPI
RedfishTaskCallback (
  IN     UINTN                TaskId,
  IN     CHAR8                *JsonText,
  IN     VOID                 *Context,
  IN OUT REDFISH_TASK_RESULT  *Result
  );

#endif
