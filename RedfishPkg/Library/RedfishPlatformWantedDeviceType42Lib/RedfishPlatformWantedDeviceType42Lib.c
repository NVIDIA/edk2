/** @file
  Implementation of RedfishPlatformWantedDeviceLibType42.
  This library assumes that the SMBIOS type 42h record is available before
  IsPlatformWantedDevice() is called with the Redfish device controller handle.

  Copyright (c) 2025, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RedfishPlatformWantedDeviceType42Lib.h"

REDFISH_INTERFACE_DATA  *mDeviceDescriptor = NULL;

/**
  The function gets information reported in Redfish Host Interface.
  This function searches for and validates the SMBIOS type 42h record.

  @param[out] DeviceDescriptor Pointer to REDFISH_INTERFACE_DATA.

  @retval EFI_SUCCESS            Get host interface successfully
  @retval EFI_NOT_FOUND          Type 42h record is not found
  @retval EFI_INVALID_PARAMETER  DeviceDescriptor is NULL
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory for DeviceDescriptor

**/
EFI_STATUS
RedfishGetHostInterfaceDeviceDescriptor (
  OUT REDFISH_INTERFACE_DATA  **DeviceDescriptor
  )
{
  EFI_STATUS                         Status;
  EFI_SMBIOS_PROTOCOL                *Smbios;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER            *Record;
  SMBIOS_TABLE_TYPE42                *Type42Record;
  REDFISH_INTERFACE_DATA             *DeviceDescriptorTemp;
  MC_HOST_INTERFACE_PROTOCOL_RECORD  *ProtocolRecord;
  REDFISH_OVER_IP_PROTOCOL_DATA      *ProtocolData;
  UINT8                              *ProtocolCount;
  UINT8                              ExpectedLength;

  if (DeviceDescriptor == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *DeviceDescriptor = NULL;

  //
  // If mDeviceDescriptor is not already populated
  // search for the SMBIOS type 42h record and sanity check the type42 record data.
  // This search should only happen one time after the type 42h record is installed.
  //
  if (mDeviceDescriptor == NULL) {
    Smbios = NULL;
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: LocateProtocol gEfiSmbiosProtocolGuid failed.\n", __func__));
      return EFI_NOT_FOUND;
    }

    //
    // Search SMBIOS tables for valid type 42h record
    // Initialize SmbiosHandle to SMBIOS_HANDLE_PI_RESERVED
    // so GetNext() will start from the first record.
    //
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    do {
      Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Smbios GetNext failed, %r.\n", __func__, Status));
        break;
      }

      //
      // Check record is type 42h
      //
      if (Record->Type != SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE) {
        continue;
      }

      //
      // Check interface type is network host interface
      //
      Type42Record = (SMBIOS_TABLE_TYPE42 *)Record;
      if (Type42Record->InterfaceType != MCHostInterfaceTypeNetworkHostInterface) {
        continue;
      }

      //
      // Verify descriptor data type and length for supported device types, PCIE_V2 or USB_V2.
      // The expected length includes
      // 1. Device Type field
      // 2. Device Descriptor specific data length
      //
      DeviceDescriptorTemp = (REDFISH_INTERFACE_DATA *)Type42Record->InterfaceTypeSpecificData;
      if (DeviceDescriptorTemp->DeviceType == REDFISH_HOST_INTERFACE_DEVICE_TYPE_PCI_PCIE_V2) {
        ExpectedLength = DESCRIPTOR_DEVICE_TYPE_SIZE +
                         sizeof (PCI_OR_PCIE_INTERFACE_DEVICE_DESCRIPTOR_V2);
      } else if (DeviceDescriptorTemp->DeviceType == REDFISH_HOST_INTERFACE_DEVICE_TYPE_USB_V2) {
        ExpectedLength = DESCRIPTOR_DEVICE_TYPE_SIZE +
                         sizeof (USB_INTERFACE_DEVICE_DESCRIPTOR_V2);
      } else {
        continue;
      }

      if (Type42Record->InterfaceTypeSpecificDataLength != ExpectedLength) {
        ASSERT (Type42Record->InterfaceTypeSpecificDataLength == ExpectedLength);
        continue;
      }

      //
      // Verify protocol count. Only one protocol is supported.
      // The protocol count is stored in the byte following the descriptor data.
      //
      ProtocolCount = (UINT8 *)DeviceDescriptorTemp + Type42Record->InterfaceTypeSpecificDataLength;
      if (*ProtocolCount != 1) {
        DEBUG ((DEBUG_ERROR, "%a: Type42 Protocol count is not 1\n", __func__));
        ASSERT (*ProtocolCount == 1);
        continue;
      }

      //
      // Verify protocol record type is RedfishOverIP
      //
      ProtocolRecord = (MC_HOST_INTERFACE_PROTOCOL_RECORD *)(ProtocolCount + sizeof (*ProtocolCount));
      if (ProtocolRecord->ProtocolType != MCHostInterfaceProtocolTypeRedfishOverIP) {
        continue;
      }

      //
      // Verify protocol type data length
      // 1. REDFISH_OVER_IP_PROTOCOL_DATA structure not including the ProtocolTypeData placeholder
      // 2. ProtocolTypeData, using RedfishServiceHostnameLength
      //
      ProtocolData   = (REDFISH_OVER_IP_PROTOCOL_DATA *)ProtocolRecord->ProtocolTypeData;
      ExpectedLength = (sizeof (REDFISH_OVER_IP_PROTOCOL_DATA) - RECORD_TYPE_DATA_SIZE) +
                       ProtocolData->RedfishServiceHostnameLength;
      if (ProtocolRecord->ProtocolTypeDataLen != ExpectedLength) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: ProtocolTypeDataLen (%d) does not match expected value: (%d)\n",
          __func__,
          ProtocolRecord->ProtocolTypeDataLen,
          ExpectedLength
          ));
        ASSERT (ProtocolRecord->ProtocolTypeDataLen == ExpectedLength);
        continue;
      }

      //
      // Verify type 42 header length is valid
      // 1. Type42 SMBIOS structure size not including the InterfaceTypeSpecificData placeholder
      // 2. Interface Type Specific Data length (device descriptor)
      // 3. Protocol Count
      // 4. Protocol record structure size not including the ProtocolTypeData placeholder
      // 5. Protocol Type Data length
      //
      ExpectedLength = (sizeof (SMBIOS_TABLE_TYPE42) - TYPE_SPECIFIC_DATA_SIZE) +
                       Type42Record->InterfaceTypeSpecificDataLength +
                       1 +
                       (sizeof (MC_HOST_INTERFACE_PROTOCOL_RECORD) - RECORD_TYPE_DATA_SIZE) +
                       ProtocolRecord->ProtocolTypeDataLen;

      if (ExpectedLength > Type42Record->Hdr.Length) {
        DEBUG ((
          DEBUG_ERROR,
          "%a: Calculated header length (%d) > Type42Record->Hdr.Length (%d).\n",
          __func__,
          ExpectedLength,
          Type42Record->Hdr.Length
          ));
        ASSERT (ExpectedLength <= Type42Record->Hdr.Length);
        continue;
      }

      //
      // Allocate a local copy of the relevant type42 device descriptor data
      //
      DEBUG ((DEBUG_MANAGEABILITY, "%a: Found valid type 42h device descriptor\n", __func__));
      mDeviceDescriptor = AllocateCopyPool (
                            Type42Record->InterfaceTypeSpecificDataLength,
                            DeviceDescriptorTemp
                            );
      if (mDeviceDescriptor == NULL) {
        ASSERT (mDeviceDescriptor != NULL);
        return EFI_OUT_OF_RESOURCES;
      }

      break;
    } while (SmbiosHandle != SMBIOS_HANDLE_PI_RESERVED);

    //
    // No valid type 42h record found at this time.
    // The search will be reattempted for subsequent RedfishRestEx Supported() calls
    // to support the case where a valid type 42h record is installed after the initial
    // RedfishRestEx Supported() call.
    //
    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: No valid type 42h record found, RedfishRestEx will not be installed\n",
        __func__
        ));
      return EFI_NOT_FOUND;
    }
  }

  //
  // Return the populated device descriptor
  //
  *DeviceDescriptor = mDeviceDescriptor;
  return EFI_SUCCESS;
}

/**
  This is the function to decide if input controller is the device
  that platform want to support. By returning EFI_UNSUPPORTED to
  caller (normally Supported function), caller should ignore this device
  and do not provide Redfish service on this controller.

  @param[in]  ControllerHandle     The handle of the controller to test.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.
                                   This is optional.

  @retval EFI_SUCCESS              This is the device supported by platform.
  @retval EFI_UNSUPPORTED          This device is not supported by platform.
  @retval EFI_INVALID_PARAMETER    ControllerHandle is NULL.

**/
EFI_STATUS
EFIAPI
IsPlatformWantedDevice (
  IN EFI_HANDLE                ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath OPTIONAL
  )
{
  EFI_STATUS              Status;
  REDFISH_INTERFACE_DATA  *DeviceDescriptor;
  EFI_MAC_ADDRESS         MacAddress;
  UINTN                   MacAddressSize;
  UINT8                   MacCompareStatus;
  UINT8                   *RedfishMacAddress;

  if (ControllerHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the device descriptor data from SMBIOS type 42h record.
  //
  DeviceDescriptor = NULL;
  Status           = RedfishGetHostInterfaceDeviceDescriptor (&DeviceDescriptor);
  if (EFI_ERROR (Status) || (DeviceDescriptor == NULL)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: RedfishGetHostInterfaceDeviceData failed, %r\n",
      __func__,
      Status
      ));
    return EFI_UNSUPPORTED;
  }

  //
  // Get the MAC address of the network interface.
  //
  MacAddressSize = 0;
  ZeroMem (&MacAddress, sizeof (EFI_MAC_ADDRESS));
  Status = NetLibGetMacAddress (ControllerHandle, &MacAddress, &MacAddressSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: NetLibGetMacAddress failed, %r.\n", __func__, Status));
    return EFI_UNSUPPORTED;
  }

  if (MacAddressSize != MAC_ADDRESS_SIZE) {
    DEBUG ((DEBUG_ERROR, "%a: MacAddressSize is not %d.\n", __func__, MAC_ADDRESS_SIZE));
    return EFI_UNSUPPORTED;
  }

  //
  // Check if we can reach out Redfish service using this network interface.
  // Check with MAC address using Device Descriptor Data Device Type 04 and Type 05.
  // Those two types of Redfish host interface device has MAC information.
  //
  if (DeviceDescriptor->DeviceType == REDFISH_HOST_INTERFACE_DEVICE_TYPE_PCI_PCIE_V2) {
    RedfishMacAddress = DeviceDescriptor->DeviceDescriptor.PciPcieDeviceV2.MacAddress;
  } else if (DeviceDescriptor->DeviceType == REDFISH_HOST_INTERFACE_DEVICE_TYPE_USB_V2) {
    RedfishMacAddress = DeviceDescriptor->DeviceDescriptor.UsbDeviceV2.MacAddress;
  } else {
    return EFI_UNSUPPORTED;
  }

  MacCompareStatus = CompareMem (
                       &MacAddress,
                       RedfishMacAddress,
                       MAC_ADDRESS_SIZE
                       );
  if (MacCompareStatus != 0) {
    DEBUG ((DEBUG_MANAGEABILITY, "%a: MAC address does not match\n", __func__));
    DEBUG ((
      DEBUG_INFO,
      "    NetworkInterface: %02x %02x %02x %02x %02x %02x.\n",
      MacAddress.Addr[0],
      MacAddress.Addr[1],
      MacAddress.Addr[2],
      MacAddress.Addr[3],
      MacAddress.Addr[4],
      MacAddress.Addr[5]
      ));

    DEBUG ((
      DEBUG_MANAGEABILITY,
      "    Redfish Host interface: %02x %02x %02x %02x %02x %02x.\n",
      RedfishMacAddress[0],
      RedfishMacAddress[1],
      RedfishMacAddress[2],
      RedfishMacAddress[3],
      RedfishMacAddress[4],
      RedfishMacAddress[5]
      ));

    return EFI_UNSUPPORTED;
  }

  DEBUG ((
    DEBUG_MANAGEABILITY,
    "%a: Handle [0x%x], MAC address matches\n",
    __func__,
    ControllerHandle
    ));
  DEBUG ((
    DEBUG_MANAGEABILITY,
    "    NetworkInterface: %02x %02x %02x %02x %02x %02x.\n",
    MacAddress.Addr[0],
    MacAddress.Addr[1],
    MacAddress.Addr[2],
    MacAddress.Addr[3],
    MacAddress.Addr[4],
    MacAddress.Addr[5]
    ));

  return EFI_SUCCESS;
}
