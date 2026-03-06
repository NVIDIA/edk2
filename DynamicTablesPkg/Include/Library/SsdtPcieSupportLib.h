/** @file
  Ssdt PCIe Support Library

  Copyright (c) 2021 - 2022, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef SSDT_PCIE_SUPPORT_LIB_H_
#define SSDT_PCIE_SUPPORT_LIB_H_

#pragma pack(1)

/** Structure used to map integer to an index.
*/
typedef struct MappingTable {
  /// Mapping table.
  /// Contains the Index <-> integer mapping
  UINT32    *Table;

  /// Last used index of the Table.
  /// Bound by MaxIndex.
  UINT32    LastIndex;

  /// Number of entries in the Table.
  UINT32    MaxIndex;
} MAPPING_TABLE;

#pragma pack()

/** Add an _OSC template method to the PciNode.

  The _OSC method is provided as an AML blob. The blob is
  parsed and attached at the end of the PciNode list of variable elements.

  @param [in]       PciInfo     Pci device information.
  @param [in, out]  PciNode     Pci node to amend.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate memory.
**/
EFI_STATUS
EFIAPI
AddOscMethod (
  IN      CONST CM_ARCH_COMMON_PCI_CONFIG_SPACE_INFO  *PciInfo,
  IN  OUT   AML_OBJECT_NODE_HANDLE                    PciNode
  );

/** Optional hook to amend the PCI host bridge AML node from the platform template.

  Platform library instances may parse the PCI SSDT template AML, locate an
  object intended for the host bridge, detach it, and attach it under @a PciNode.

  The hook is optional: implementations may return EFI_NOT_FOUND when there is
  nothing to attach. The SSDT generator maps EFI_NOT_FOUND to EFI_SUCCESS.

  The default stub does not modify the AML tree and returns EFI_SUCCESS.

  @param [in]       PciInfo     Root bridge / segment context (platform-defined).
  @param [in, out]  PciNode     Host bridge AML device node to amend.

  @retval EFI_SUCCESS             Attached, or stub no-op success.
  @retval EFI_NOT_FOUND           Nothing to attach (optional; generator maps to success).
  @retval EFI_NOT_READY           Template not initialized (platform-specific).
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate memory.
**/
EFI_STATUS
EFIAPI
AddHostBridgeDsmMethod (
  IN      CONST CM_ARCH_COMMON_PCI_CONFIG_SPACE_INFO  *PciInfo,
  IN  OUT   AML_OBJECT_NODE_HANDLE                    PciNode
  );

/** Generate Pci slots devices.

  PCI Firmware Specification - Revision 3.3,
  s4.8 "Generic ACPI PCI Slot Description" requests to describe the PCI slot
  used. It should be possible to enumerate them, but this is additional
  information.

  @param [in]       PciInfo       Pci device information.
  @param [in]       MappingTable  The mapping table structure.
  @param [in]       Uid           Unique Id of the Pci device.
  @param [in, out]  PciNode       Pci node to amend.

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory.
**/
EFI_STATUS
EFIAPI
GeneratePciSlots (
  IN      CONST CM_ARCH_COMMON_PCI_CONFIG_SPACE_INFO  *PciInfo,
  IN      CONST MAPPING_TABLE                         *MappingTable,
  IN            UINT32                                Uid,
  IN  OUT       AML_OBJECT_NODE_HANDLE                PciNode
  );

#endif // SSDT_PCIE_SUPPORT_LIB_H_
