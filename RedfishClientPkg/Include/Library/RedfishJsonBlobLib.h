/** @file
  This file defines the Redfish JSON Blob library interface.

  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef REDFISH_JSON_BLOB_LIB_H_
#define REDFISH_JSON_BLOB_LIB_H_

#include <Uefi.h>
#include <RedfishBase.h>
#include <Library/JsonLib.h>

typedef VOID *REDFISH_JSON_BLOB;

/**
  The callback function to convert data from private structure to JSON object.

  @param[in, out] JsonValue         Json object which carries data converted from StructureData.
  @param[in]      Context           Pointer to context from caller.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval EFI_NOT_FOUND            No data to be converted to JSON object.
  @retval Others                   Some errors happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_JSON_BLOB_TO_JSON)(
  IN OUT EDKII_JSON_VALUE    JsonValue,
  IN     VOID                *Context
  );

/**
  The callback function to convert data from JSON object to private structure.

  @param[in]    JsonValue         Json object to be converted to private structure.
  @param[in]    Context           Pointer to context from caller.

  @retval EFI_SUCCESS              Data is converted successfully.
  @retval EFI_NOT_FOUND            No data to be converted to private structure.
  @retval Others                   Some errors happened.

**/
typedef
EFI_STATUS
(EFIAPI *REDFISH_JSON_BLOB_TO_STRUCTURE)(
  IN EDKII_JSON_VALUE    JsonValue,
  IN VOID                *Context
  );

/**
  Open JSON blob with given GUID and name. Use RedfishJsonBlobIsEmpty() to see if
  JSON blob exist in system or not. If JSON blob already exist, ToStructureFunc()
  is called in order to convert data from JSON format to private data structure.

  @param[in]    BlobGuid          JSON blob GUID.
  @param[in]    BlobName          JSON blob name.
  @param[in]    ToStructureFunc   Callback function to convert data from JSON blob to
                                  private structure.
  @param[in]    ToJsonFunc        Callback function to convert data to JSON blob.
  @param[in]    Context           Pointer to context from caller.

  @retval REDFISH_JSON_BLOB *     JSON blob is opened successfully.
  @retval NULL                    Can not open JSON blob due to errors.

**/
REDFISH_JSON_BLOB *
RedfishJsonBlobOpen (
  IN EFI_GUID                        *BlobGuid,
  IN EFI_STRING                      BlobName,
  IN REDFISH_JSON_BLOB_TO_STRUCTURE  ToStructureFunc,
  IN REDFISH_JSON_BLOB_TO_JSON       ToJsonFunc,
  IN VOID                            *Context OPTIONAL
  );

/**
  Write JSON blob with given JSON blob handler to system. ToJsonFunc() is
  called in order to convert data from private data structure to JSON object.

  @param[in]    Blob            Pointer to JSON blob instance.

  @retval EFI_SUCCESS           Write JSON blob successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishJsonBlobWrite (
  IN REDFISH_JSON_BLOB  *Blob
  );

/**
  Close JSON blob with given JSON blob handler and save Data to system if DoWrite
  is set to TRUE. After this function is returned, Blob is released and no longer
  be usable.

  @param[in]    Blob            Pointer to JSON blob instance.
  @param[in]    DoWrite         RedfishJsonBlobWrite() is called if it is set to TRUE.

  @retval EFI_SUCCESS           JSON blob is closed successfully.
  @retval Others                Error occurs.

**/
EFI_STATUS
RedfishJsonBlobClose (
  IN REDFISH_JSON_BLOB  *Blob,
  IN BOOLEAN            DoWrite
  );

/**
  Return TRUE if given Blob is empty. FALSE otherwise.

  @param[in]    Blob            Pointer to JSON blob instance.

  @retval TRUE     Blob is empty.
  @retval FALSE    Blob is not empty.

**/
BOOLEAN
RedfishJsonBlobIsEmpty (
  IN REDFISH_JSON_BLOB  *Blob
  );

#endif
