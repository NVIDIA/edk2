/** @file
  Redfish feature driver implementation - common functions

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MemoryCommon.h"

CHAR8 MemoryEmptyJson[] = "{\"@odata.id\": \"\", \"@odata.type\": \"#Memory.v1_7_1.Memory\", \"Id\": \"\", \"Name\": \"\"}";

REDFISH_RESOURCE_COMMON_PRIVATE *mRedfishResourcePrivate = NULL;

/**
  Consume resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.
  @param[in]   HeaderEtag          The Etag string returned in HTTP header.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishConsumeResourceCommon (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE *Private,
  IN  CHAR8                           *Json,
  IN  CHAR8                           *HeaderEtag OPTIONAL
  )
{
  EFI_STATUS                   Status;
  EFI_REDFISH_MEMORY_V1_7_1     *Memory;
  EFI_REDFISH_MEMORY_V1_7_1_CS  *MemoryCs;
  EFI_STRING                   ConfigureLang;


  if (Private == NULL || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  Memory = NULL;
  MemoryCs = NULL;
  ConfigureLang = NULL;

  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Json,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&Memory
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, ToStructure() failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  MemoryCs = Memory->Memory;

  //
  // Check ETAG to see if we need to consume it
  //
  if (CheckEtag (Private->Uri, HeaderEtag, MemoryCs->odata_etag)) {
    //
    // No change
    //
    DEBUG ((DEBUG_INFO, "%a, ETAG: %s has no change, ignore consume action\n", __FUNCTION__, Private->Uri));
    Status = EFI_ALREADY_STARTED;
    goto ON_RELEASE;
  }

  //
  // Handle ALLOCATIONALIGNMENTMIB
  //
  if (MemoryCs->AllocationAlignmentMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "AllocationAlignmentMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->AllocationAlignmentMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ALLOCATIONINCREMENTMIB
  //
  if (MemoryCs->AllocationIncrementMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "AllocationIncrementMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->AllocationIncrementMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ALLOWEDSPEEDSMHZ
  //
  if (MemoryCs->AllowedSpeedsMHz != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "AllowedSpeedsMHz");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericArrayType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->AllowedSpeedsMHz);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // ASSEMBLY will be handled by feature driver.
  //

  //
  // Handle BASEMODULETYPE
  //
  if (MemoryCs->BaseModuleType != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "BaseModuleType");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->BaseModuleType);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BUSWIDTHBITS
  //
  if (MemoryCs->BusWidthBits != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "BusWidthBits");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->BusWidthBits);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle CACHESIZEMIB
  //
  if (MemoryCs->CacheSizeMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "CacheSizeMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->CacheSizeMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle CAPACITYMIB
  //
  if (MemoryCs->CapacityMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "CapacityMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->CapacityMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle CONFIGURATIONLOCKED
  //
  if (MemoryCs->ConfigurationLocked != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "ConfigurationLocked");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->ConfigurationLocked);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle DATAWIDTHBITS
  //
  if (MemoryCs->DataWidthBits != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "DataWidthBits");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->DataWidthBits);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle DEVICEID
  //
  if (MemoryCs->DeviceID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "DeviceID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->DeviceID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle DEVICELOCATOR
  //
  if (MemoryCs->DeviceLocator != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "DeviceLocator");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->DeviceLocator);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ERRORCORRECTION
  //
  if (MemoryCs->ErrorCorrection != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "ErrorCorrection");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->ErrorCorrection);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle FIRMWAREAPIVERSION
  //
  if (MemoryCs->FirmwareApiVersion != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "FirmwareApiVersion");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->FirmwareApiVersion);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle FIRMWAREREVISION
  //
  if (MemoryCs->FirmwareRevision != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "FirmwareRevision");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->FirmwareRevision);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle FUNCTIONCLASSES
  //
  if (MemoryCs->FunctionClasses != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "FunctionClasses");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringArrayType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->FunctionClasses);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ISRANKSPAREENABLED
  //
  if (MemoryCs->IsRankSpareEnabled != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "IsRankSpareEnabled");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->IsRankSpareEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle ISSPAREDEVICEENABLED
  //
  if (MemoryCs->IsSpareDeviceEnabled != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "IsSpareDeviceEnabled");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->IsSpareDeviceEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // LOCATION is not handled. Defined in http://redfish.dmtf.org/schemas/v1/Resource.json
  //

  //
  // Handle LOGICALSIZEMIB
  //
  if (MemoryCs->LogicalSizeMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "LogicalSizeMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->LogicalSizeMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MANUFACTURER
  //
  if (MemoryCs->Manufacturer != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "Manufacturer");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->Manufacturer);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MAXTDPMILLIWATTS
  //
  if (MemoryCs->MaxTDPMilliWatts != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MaxTDPMilliWatts");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericArrayType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->MaxTDPMilliWatts);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYDEVICETYPE
  //
  if (MemoryCs->MemoryDeviceType != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryDeviceType");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->MemoryDeviceType);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYLOCATION
  //
  if (MemoryCs->MemoryLocation == NULL) {
    MemoryCs->MemoryLocation = AllocateZeroPool (sizeof (RedfishMemory_V1_7_1_MemoryLocation_CS));
    ASSERT (MemoryCs->MemoryLocation != NULL);
  }

  //
  // Handle MEMORYLOCATION->CHANNEL
  //
  if (MemoryCs->MemoryLocation->Channel != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryLocation/Channel");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->MemoryLocation->Channel);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYLOCATION->MEMORYCONTROLLER
  //
  if (MemoryCs->MemoryLocation->MemoryController != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryLocation/MemoryController");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->MemoryLocation->MemoryController);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYLOCATION->SLOT
  //
  if (MemoryCs->MemoryLocation->Slot != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryLocation/Slot");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->MemoryLocation->Slot);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYLOCATION->SOCKET
  //
  if (MemoryCs->MemoryLocation->Socket != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryLocation/Socket");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->MemoryLocation->Socket);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYMEDIA
  //
//
// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle MEMORYSUBSYSTEMCONTROLLERMANUFACTURERID
  //
  if (MemoryCs->MemorySubsystemControllerManufacturerID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemorySubsystemControllerManufacturerID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->MemorySubsystemControllerManufacturerID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYSUBSYSTEMCONTROLLERPRODUCTID
  //
  if (MemoryCs->MemorySubsystemControllerProductID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemorySubsystemControllerProductID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->MemorySubsystemControllerProductID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYTYPE
  //
  if (MemoryCs->MemoryType != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "MemoryType");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->MemoryType);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // METRICS will be handled by feature driver.
  //

  //
  // Handle MODULEMANUFACTURERID
  //
  if (MemoryCs->ModuleManufacturerID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "ModuleManufacturerID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->ModuleManufacturerID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MODULEPRODUCTID
  //
  if (MemoryCs->ModuleProductID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "ModuleProductID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->ModuleProductID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle NONVOLATILESIZEMIB
  //
  if (MemoryCs->NonVolatileSizeMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "NonVolatileSizeMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->NonVolatileSizeMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle OPERATINGMEMORYMODES
  //
//
// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle OPERATINGSPEEDMHZ
  //
  if (MemoryCs->OperatingSpeedMhz != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "OperatingSpeedMhz");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->OperatingSpeedMhz);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PARTNUMBER
  //
  if (MemoryCs->PartNumber != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PartNumber");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->PartNumber);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PERSISTENTREGIONNUMBERLIMIT
  //
  if (MemoryCs->PersistentRegionNumberLimit != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PersistentRegionNumberLimit");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PersistentRegionNumberLimit);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PERSISTENTREGIONSIZELIMITMIB
  //
  if (MemoryCs->PersistentRegionSizeLimitMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PersistentRegionSizeLimitMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PersistentRegionSizeLimitMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PERSISTENTREGIONSIZEMAXMIB
  //
  if (MemoryCs->PersistentRegionSizeMaxMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PersistentRegionSizeMaxMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PersistentRegionSizeMaxMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle POWERMANAGEMENTPOLICY
  //
  if (MemoryCs->PowerManagementPolicy == NULL) {
    MemoryCs->PowerManagementPolicy = AllocateZeroPool (sizeof (RedfishMemory_V1_7_1_PowerManagementPolicy_CS));
    ASSERT (MemoryCs->PowerManagementPolicy != NULL);
  }

  //
  // Handle POWERMANAGEMENTPOLICY->AVERAGEPOWERBUDGETMILLIWATTS
  //
  if (MemoryCs->PowerManagementPolicy->AveragePowerBudgetMilliWatts != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PowerManagementPolicy/AveragePowerBudgetMilliWatts");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PowerManagementPolicy->AveragePowerBudgetMilliWatts);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle POWERMANAGEMENTPOLICY->MAXTDPMILLIWATTS
  //
  if (MemoryCs->PowerManagementPolicy->MaxTDPMilliWatts != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PowerManagementPolicy/MaxTDPMilliWatts");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PowerManagementPolicy->MaxTDPMilliWatts);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle POWERMANAGEMENTPOLICY->PEAKPOWERBUDGETMILLIWATTS
  //
  if (MemoryCs->PowerManagementPolicy->PeakPowerBudgetMilliWatts != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PowerManagementPolicy/PeakPowerBudgetMilliWatts");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->PowerManagementPolicy->PeakPowerBudgetMilliWatts);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle POWERMANAGEMENTPOLICY->POLICYENABLED
  //
  if (MemoryCs->PowerManagementPolicy->PolicyEnabled != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "PowerManagementPolicy/PolicyEnabled");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->PowerManagementPolicy->PolicyEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle RANKCOUNT
  //
  if (MemoryCs->RankCount != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "RankCount");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->RankCount);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle REGIONS
  //
//
// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle SECURITYCAPABILITIES
  //
  if (MemoryCs->SecurityCapabilities == NULL) {
    MemoryCs->SecurityCapabilities = AllocateZeroPool (sizeof (RedfishMemory_V1_7_1_SecurityCapabilities_CS));
    ASSERT (MemoryCs->SecurityCapabilities != NULL);
  }

  //
  // Handle SECURITYCAPABILITIES->CONFIGURATIONLOCKCAPABLE
  //
  if (MemoryCs->SecurityCapabilities->ConfigurationLockCapable != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityCapabilities/ConfigurationLockCapable");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->SecurityCapabilities->ConfigurationLockCapable);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SECURITYCAPABILITIES->DATALOCKCAPABLE
  //
  if (MemoryCs->SecurityCapabilities->DataLockCapable != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityCapabilities/DataLockCapable");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->SecurityCapabilities->DataLockCapable);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SECURITYCAPABILITIES->MAXPASSPHRASECOUNT
  //
  if (MemoryCs->SecurityCapabilities->MaxPassphraseCount != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityCapabilities/MaxPassphraseCount");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->SecurityCapabilities->MaxPassphraseCount);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SECURITYCAPABILITIES->PASSPHRASECAPABLE
  //
  if (MemoryCs->SecurityCapabilities->PassphraseCapable != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityCapabilities/PassphraseCapable");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*MemoryCs->SecurityCapabilities->PassphraseCapable);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SECURITYCAPABILITIES->PASSPHRASELOCKLIMIT
  //
  if (MemoryCs->SecurityCapabilities->PassphraseLockLimit != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityCapabilities/PassphraseLockLimit");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->SecurityCapabilities->PassphraseLockLimit);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SECURITYCAPABILITIES->SECURITYSTATES
  //
//
// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle SECURITYSTATE
  //
  if (MemoryCs->SecurityState != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SecurityState");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->SecurityState);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SERIALNUMBER
  //
  if (MemoryCs->SerialNumber != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SerialNumber");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->SerialNumber);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SPAREDEVICECOUNT
  //
  if (MemoryCs->SpareDeviceCount != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SpareDeviceCount");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->SpareDeviceCount);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SUBSYSTEMDEVICEID
  //
  if (MemoryCs->SubsystemDeviceID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SubsystemDeviceID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->SubsystemDeviceID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SUBSYSTEMVENDORID
  //
  if (MemoryCs->SubsystemVendorID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "SubsystemVendorID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->SubsystemVendorID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle VENDORID
  //
  if (MemoryCs->VendorID != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "VendorID");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, MemoryCs->VendorID);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle VOLATILEREGIONNUMBERLIMIT
  //
  if (MemoryCs->VolatileRegionNumberLimit != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "VolatileRegionNumberLimit");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->VolatileRegionNumberLimit);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle VOLATILEREGIONSIZELIMITMIB
  //
  if (MemoryCs->VolatileRegionSizeLimitMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "VolatileRegionSizeLimitMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->VolatileRegionSizeLimitMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle VOLATILEREGIONSIZEMAXMIB
  //
  if (MemoryCs->VolatileRegionSizeMaxMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "VolatileRegionSizeMaxMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->VolatileRegionSizeMaxMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle VOLATILESIZEMIB
  //
  if (MemoryCs->VolatileSizeMiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (MemoryCs->odata_id, "VolatileSizeMiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*MemoryCs->VolatileSizeMiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a, apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a, can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }


ON_RELEASE:

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (
                                 Private->JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)Memory
                                 );

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningMemoryProperties (
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL  *JsonStructProtocol,
  IN  CHAR8                             *InputJson,
  IN  CHAR8                             *ResourceId,  OPTIONAL
  IN  EFI_STRING                        ConfigureLang,
  IN  BOOLEAN                           ProvisionMode,
  OUT CHAR8                             **ResultJson
  )
{
  EFI_REDFISH_MEMORY_V1_7_1     *Memory;
  EFI_REDFISH_MEMORY_V1_7_1_CS  *MemoryCs;
  EFI_STATUS                    Status;
  BOOLEAN                       PropertyChanged;
  INT64                         *NumericValue;
  INT64                         *NumericArrayValue;
  UINTN                         ArraySize;
  CHAR8                         *AsciiStringValue;
  BOOLEAN                       *BooleanValue;
  INT32                         *IntegerValue;
  CHAR8                         **AsciiStringArrayValue;



  if (JsonStructProtocol == NULL || ResultJson == NULL || IS_EMPTY_STRING (InputJson) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a provision for %s with: %s\n", __FUNCTION__, ConfigureLang, (ProvisionMode ? L"Provision resource" : L"Update resource")));

  *ResultJson = NULL;
  PropertyChanged = FALSE;

  Memory = NULL;
  Status = JsonStructProtocol->ToStructure (
                                 JsonStructProtocol,
                                 NULL,
                                 InputJson,
                                 (EFI_REST_JSON_STRUCTURE_HEADER **)&Memory
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, ToStructure failure: %r\n", __FUNCTION__, Status));
    return Status;
  }

  MemoryCs = Memory->Memory;

  //
  // ID
  //
  if (MemoryCs->Id == NULL && !IS_EMPTY_STRING (ResourceId)) {
    MemoryCs->Id = AllocateCopyPool (AsciiStrSize (ResourceId), ResourceId);
  }

  //
  // Handle ALLOCATIONALIGNMENTMIB
  //
  if (PropertyChecker (MemoryCs->AllocationAlignmentMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"AllocationAlignmentMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->AllocationAlignmentMiB != *NumericValue) {
        MemoryCs->AllocationAlignmentMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle ALLOCATIONINCREMENTMIB
  //
  if (PropertyChecker (MemoryCs->AllocationIncrementMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"AllocationIncrementMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->AllocationIncrementMiB != *NumericValue) {
        MemoryCs->AllocationIncrementMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle ALLOWEDSPEEDSMHZ
  //
  if (PropertyChecker (MemoryCs->AllowedSpeedsMHz, ProvisionMode)) {
    NumericArrayValue = GetPropertyNumericArrayValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"AllowedSpeedsMHz", ConfigureLang, &ArraySize);
    if (NumericArrayValue != NULL) {
      if (ProvisionMode || !CompareRedfishNumericArrayValues (MemoryCs->AllowedSpeedsMHz, NumericArrayValue, ArraySize)) {
        AddRedfishNumericArray (&MemoryCs->AllowedSpeedsMHz, NumericArrayValue, ArraySize);
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle BASEMODULETYPE
  //
  if (PropertyChecker (MemoryCs->BaseModuleType, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"BaseModuleType", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->BaseModuleType, AsciiStringValue) != 0) {
        MemoryCs->BaseModuleType = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle BUSWIDTHBITS
  //
  if (PropertyChecker (MemoryCs->BusWidthBits, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"BusWidthBits", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->BusWidthBits != *NumericValue) {
        MemoryCs->BusWidthBits = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle CACHESIZEMIB
  //
  if (PropertyChecker (MemoryCs->CacheSizeMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"CacheSizeMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->CacheSizeMiB != *NumericValue) {
        MemoryCs->CacheSizeMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle CAPACITYMIB
  //
  if (PropertyChecker (MemoryCs->CapacityMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"CapacityMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->CapacityMiB != *NumericValue) {
        MemoryCs->CapacityMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle CONFIGURATIONLOCKED
  //
  if (PropertyChecker (MemoryCs->ConfigurationLocked, ProvisionMode)) {
    BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ConfigurationLocked", ConfigureLang);
    if (BooleanValue != NULL) {
      if (ProvisionMode || *MemoryCs->ConfigurationLocked != *BooleanValue) {
        IntegerValue = AllocatePool (sizeof (*IntegerValue));
        if (IntegerValue != NULL) {
          *IntegerValue = (BooleanValue ? 0x01 : 0x00);
          MemoryCs->ConfigurationLocked = IntegerValue;
          PropertyChanged = TRUE;
        }
      }
    }
  }
  //
  // Handle DATAWIDTHBITS
  //
  if (PropertyChecker (MemoryCs->DataWidthBits, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"DataWidthBits", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->DataWidthBits != *NumericValue) {
        MemoryCs->DataWidthBits = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle DEVICEID
  //
  if (PropertyChecker (MemoryCs->DeviceID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"DeviceID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->DeviceID, AsciiStringValue) != 0) {
        MemoryCs->DeviceID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle DEVICELOCATOR
  //
  if (PropertyChecker (MemoryCs->DeviceLocator, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"DeviceLocator", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->DeviceLocator, AsciiStringValue) != 0) {
        MemoryCs->DeviceLocator = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle ERRORCORRECTION
  //
  if (PropertyChecker (MemoryCs->ErrorCorrection, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ErrorCorrection", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->ErrorCorrection, AsciiStringValue) != 0) {
        MemoryCs->ErrorCorrection = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle FIRMWAREAPIVERSION
  //
  if (PropertyChecker (MemoryCs->FirmwareApiVersion, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"FirmwareApiVersion", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->FirmwareApiVersion, AsciiStringValue) != 0) {
        MemoryCs->FirmwareApiVersion = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle FIRMWAREREVISION
  //
  if (PropertyChecker (MemoryCs->FirmwareRevision, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"FirmwareRevision", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->FirmwareRevision, AsciiStringValue) != 0) {
        MemoryCs->FirmwareRevision = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle FUNCTIONCLASSES
  //
  if (PropertyChecker (MemoryCs->FunctionClasses, ProvisionMode)) {
    AsciiStringArrayValue = GetPropertyStringArrayValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"FunctionClasses", ConfigureLang, &ArraySize);
    if (AsciiStringArrayValue != NULL) {
      if (ProvisionMode || !CompareRedfishStringArrayValues (MemoryCs->FunctionClasses, AsciiStringArrayValue, ArraySize)) {
        AddRedfishCharArray (&MemoryCs->FunctionClasses, AsciiStringArrayValue, ArraySize);
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle ISRANKSPAREENABLED
  //
  if (PropertyChecker (MemoryCs->IsRankSpareEnabled, ProvisionMode)) {
    BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"IsRankSpareEnabled", ConfigureLang);
    if (BooleanValue != NULL) {
      if (ProvisionMode || *MemoryCs->IsRankSpareEnabled != *BooleanValue) {
        IntegerValue = AllocatePool (sizeof (*IntegerValue));
        if (IntegerValue != NULL) {
          *IntegerValue = (BooleanValue ? 0x01 : 0x00);
          MemoryCs->IsRankSpareEnabled = IntegerValue;
          PropertyChanged = TRUE;
        }
      }
    }
  }
  //
  // Handle ISSPAREDEVICEENABLED
  //
  if (PropertyChecker (MemoryCs->IsSpareDeviceEnabled, ProvisionMode)) {
    BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"IsSpareDeviceEnabled", ConfigureLang);
    if (BooleanValue != NULL) {
      if (ProvisionMode || *MemoryCs->IsSpareDeviceEnabled != *BooleanValue) {
        IntegerValue = AllocatePool (sizeof (*IntegerValue));
        if (IntegerValue != NULL) {
          *IntegerValue = (BooleanValue ? 0x01 : 0x00);
          MemoryCs->IsSpareDeviceEnabled = IntegerValue;
          PropertyChanged = TRUE;
        }
      }
    }
  }
  //
  // Handle LOGICALSIZEMIB
  //
  if (PropertyChecker (MemoryCs->LogicalSizeMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"LogicalSizeMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->LogicalSizeMiB != *NumericValue) {
        MemoryCs->LogicalSizeMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MANUFACTURER
  //
  if (PropertyChecker (MemoryCs->Manufacturer, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Manufacturer", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->Manufacturer, AsciiStringValue) != 0) {
        MemoryCs->Manufacturer = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MAXTDPMILLIWATTS
  //
  if (PropertyChecker (MemoryCs->MaxTDPMilliWatts, ProvisionMode)) {
    NumericArrayValue = GetPropertyNumericArrayValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MaxTDPMilliWatts", ConfigureLang, &ArraySize);
    if (NumericArrayValue != NULL) {
      if (ProvisionMode || !CompareRedfishNumericArrayValues (MemoryCs->MaxTDPMilliWatts, NumericArrayValue, ArraySize)) {
        AddRedfishNumericArray (&MemoryCs->MaxTDPMilliWatts, NumericArrayValue, ArraySize);
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MEMORYDEVICETYPE
  //
  if (PropertyChecker (MemoryCs->MemoryDeviceType, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryDeviceType", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->MemoryDeviceType, AsciiStringValue) != 0) {
        MemoryCs->MemoryDeviceType = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MEMORYLOCATION
  //
  if (MemoryCs->MemoryLocation != NULL) {
    //
    // Handle MEMORYLOCATION->CHANNEL
    //
    if (PropertyChecker (MemoryCs->MemoryLocation->Channel, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryLocation/Channel", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->MemoryLocation->Channel != *NumericValue) {
          MemoryCs->MemoryLocation->Channel = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle MEMORYLOCATION->MEMORYCONTROLLER
    //
    if (PropertyChecker (MemoryCs->MemoryLocation->MemoryController, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryLocation/MemoryController", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->MemoryLocation->MemoryController != *NumericValue) {
          MemoryCs->MemoryLocation->MemoryController = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle MEMORYLOCATION->SLOT
    //
    if (PropertyChecker (MemoryCs->MemoryLocation->Slot, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryLocation/Slot", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->MemoryLocation->Slot != *NumericValue) {
          MemoryCs->MemoryLocation->Slot = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle MEMORYLOCATION->SOCKET
    //
    if (PropertyChecker (MemoryCs->MemoryLocation->Socket, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryLocation/Socket", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->MemoryLocation->Socket != *NumericValue) {
          MemoryCs->MemoryLocation->Socket = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
  }

  //
  // Handle MEMORYMEDIA
  //
//// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle MEMORYSUBSYSTEMCONTROLLERMANUFACTURERID
  //
  if (PropertyChecker (MemoryCs->MemorySubsystemControllerManufacturerID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemorySubsystemControllerManufacturerID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->MemorySubsystemControllerManufacturerID, AsciiStringValue) != 0) {
        MemoryCs->MemorySubsystemControllerManufacturerID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MEMORYSUBSYSTEMCONTROLLERPRODUCTID
  //
  if (PropertyChecker (MemoryCs->MemorySubsystemControllerProductID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemorySubsystemControllerProductID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->MemorySubsystemControllerProductID, AsciiStringValue) != 0) {
        MemoryCs->MemorySubsystemControllerProductID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MEMORYTYPE
  //
  if (PropertyChecker (MemoryCs->MemoryType, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemoryType", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->MemoryType, AsciiStringValue) != 0) {
        MemoryCs->MemoryType = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MODULEMANUFACTURERID
  //
  if (PropertyChecker (MemoryCs->ModuleManufacturerID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ModuleManufacturerID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->ModuleManufacturerID, AsciiStringValue) != 0) {
        MemoryCs->ModuleManufacturerID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle MODULEPRODUCTID
  //
  if (PropertyChecker (MemoryCs->ModuleProductID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ModuleProductID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->ModuleProductID, AsciiStringValue) != 0) {
        MemoryCs->ModuleProductID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle NONVOLATILESIZEMIB
  //
  if (PropertyChecker (MemoryCs->NonVolatileSizeMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"NonVolatileSizeMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->NonVolatileSizeMiB != *NumericValue) {
        MemoryCs->NonVolatileSizeMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle OPERATINGMEMORYMODES
  //
//// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle OPERATINGSPEEDMHZ
  //
  if (PropertyChecker (MemoryCs->OperatingSpeedMhz, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"OperatingSpeedMhz", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->OperatingSpeedMhz != *NumericValue) {
        MemoryCs->OperatingSpeedMhz = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle PARTNUMBER
  //
  if (PropertyChecker (MemoryCs->PartNumber, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PartNumber", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->PartNumber, AsciiStringValue) != 0) {
        MemoryCs->PartNumber = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle PERSISTENTREGIONNUMBERLIMIT
  //
  if (PropertyChecker (MemoryCs->PersistentRegionNumberLimit, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PersistentRegionNumberLimit", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->PersistentRegionNumberLimit != *NumericValue) {
        MemoryCs->PersistentRegionNumberLimit = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle PERSISTENTREGIONSIZELIMITMIB
  //
  if (PropertyChecker (MemoryCs->PersistentRegionSizeLimitMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PersistentRegionSizeLimitMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->PersistentRegionSizeLimitMiB != *NumericValue) {
        MemoryCs->PersistentRegionSizeLimitMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle PERSISTENTREGIONSIZEMAXMIB
  //
  if (PropertyChecker (MemoryCs->PersistentRegionSizeMaxMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PersistentRegionSizeMaxMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->PersistentRegionSizeMaxMiB != *NumericValue) {
        MemoryCs->PersistentRegionSizeMaxMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle POWERMANAGEMENTPOLICY
  //
  if (MemoryCs->PowerManagementPolicy != NULL) {
    //
    // Handle POWERMANAGEMENTPOLICY->AVERAGEPOWERBUDGETMILLIWATTS
    //
    if (PropertyChecker (MemoryCs->PowerManagementPolicy->AveragePowerBudgetMilliWatts, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PowerManagementPolicy/AveragePowerBudgetMilliWatts", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->PowerManagementPolicy->AveragePowerBudgetMilliWatts != *NumericValue) {
          MemoryCs->PowerManagementPolicy->AveragePowerBudgetMilliWatts = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle POWERMANAGEMENTPOLICY->MAXTDPMILLIWATTS
    //
    if (PropertyChecker (MemoryCs->PowerManagementPolicy->MaxTDPMilliWatts, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PowerManagementPolicy/MaxTDPMilliWatts", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->PowerManagementPolicy->MaxTDPMilliWatts != *NumericValue) {
          MemoryCs->PowerManagementPolicy->MaxTDPMilliWatts = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle POWERMANAGEMENTPOLICY->PEAKPOWERBUDGETMILLIWATTS
    //
    if (PropertyChecker (MemoryCs->PowerManagementPolicy->PeakPowerBudgetMilliWatts, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PowerManagementPolicy/PeakPowerBudgetMilliWatts", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->PowerManagementPolicy->PeakPowerBudgetMilliWatts != *NumericValue) {
          MemoryCs->PowerManagementPolicy->PeakPowerBudgetMilliWatts = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle POWERMANAGEMENTPOLICY->POLICYENABLED
    //
    if (PropertyChecker (MemoryCs->PowerManagementPolicy->PolicyEnabled, ProvisionMode)) {
      BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PowerManagementPolicy/PolicyEnabled", ConfigureLang);
      if (BooleanValue != NULL) {
        if (ProvisionMode || *MemoryCs->PowerManagementPolicy->PolicyEnabled != *BooleanValue) {
          IntegerValue = AllocatePool (sizeof (*IntegerValue));
          if (IntegerValue != NULL) {
            *IntegerValue = (BooleanValue ? 0x01 : 0x00);
            MemoryCs->PowerManagementPolicy->PolicyEnabled = IntegerValue;
            PropertyChanged = TRUE;
          }
        }
      }
    }
  }

  //
  // Handle RANKCOUNT
  //
  if (PropertyChecker (MemoryCs->RankCount, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"RankCount", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->RankCount != *NumericValue) {
        MemoryCs->RankCount = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle REGIONS
  //
//// ****** Warning ******
// Unsupported array type:
//

  //
  // Handle SECURITYCAPABILITIES
  //
  if (MemoryCs->SecurityCapabilities != NULL) {
    //
    // Handle SECURITYCAPABILITIES->CONFIGURATIONLOCKCAPABLE
    //
    if (PropertyChecker (MemoryCs->SecurityCapabilities->ConfigurationLockCapable, ProvisionMode)) {
      BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityCapabilities/ConfigurationLockCapable", ConfigureLang);
      if (BooleanValue != NULL) {
        if (ProvisionMode || *MemoryCs->SecurityCapabilities->ConfigurationLockCapable != *BooleanValue) {
          IntegerValue = AllocatePool (sizeof (*IntegerValue));
          if (IntegerValue != NULL) {
            *IntegerValue = (BooleanValue ? 0x01 : 0x00);
            MemoryCs->SecurityCapabilities->ConfigurationLockCapable = IntegerValue;
            PropertyChanged = TRUE;
          }
        }
      }
    }
    //
    // Handle SECURITYCAPABILITIES->DATALOCKCAPABLE
    //
    if (PropertyChecker (MemoryCs->SecurityCapabilities->DataLockCapable, ProvisionMode)) {
      BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityCapabilities/DataLockCapable", ConfigureLang);
      if (BooleanValue != NULL) {
        if (ProvisionMode || *MemoryCs->SecurityCapabilities->DataLockCapable != *BooleanValue) {
          IntegerValue = AllocatePool (sizeof (*IntegerValue));
          if (IntegerValue != NULL) {
            *IntegerValue = (BooleanValue ? 0x01 : 0x00);
            MemoryCs->SecurityCapabilities->DataLockCapable = IntegerValue;
            PropertyChanged = TRUE;
          }
        }
      }
    }
    //
    // Handle SECURITYCAPABILITIES->MAXPASSPHRASECOUNT
    //
    if (PropertyChecker (MemoryCs->SecurityCapabilities->MaxPassphraseCount, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityCapabilities/MaxPassphraseCount", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->SecurityCapabilities->MaxPassphraseCount != *NumericValue) {
          MemoryCs->SecurityCapabilities->MaxPassphraseCount = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle SECURITYCAPABILITIES->PASSPHRASECAPABLE
    //
    if (PropertyChecker (MemoryCs->SecurityCapabilities->PassphraseCapable, ProvisionMode)) {
      BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityCapabilities/PassphraseCapable", ConfigureLang);
      if (BooleanValue != NULL) {
        if (ProvisionMode || *MemoryCs->SecurityCapabilities->PassphraseCapable != *BooleanValue) {
          IntegerValue = AllocatePool (sizeof (*IntegerValue));
          if (IntegerValue != NULL) {
            *IntegerValue = (BooleanValue ? 0x01 : 0x00);
            MemoryCs->SecurityCapabilities->PassphraseCapable = IntegerValue;
            PropertyChanged = TRUE;
          }
        }
      }
    }
    //
    // Handle SECURITYCAPABILITIES->PASSPHRASELOCKLIMIT
    //
    if (PropertyChecker (MemoryCs->SecurityCapabilities->PassphraseLockLimit, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityCapabilities/PassphraseLockLimit", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || *MemoryCs->SecurityCapabilities->PassphraseLockLimit != *NumericValue) {
          MemoryCs->SecurityCapabilities->PassphraseLockLimit = NumericValue;
          PropertyChanged = TRUE;
        }
      }
    }
    //
    // Handle SECURITYCAPABILITIES->SECURITYSTATES
    //
  //// ****** Warning ******
  // Unsupported array type:
  //

  }

  //
  // Handle SECURITYSTATE
  //
  if (PropertyChecker (MemoryCs->SecurityState, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SecurityState", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->SecurityState, AsciiStringValue) != 0) {
        MemoryCs->SecurityState = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle SERIALNUMBER
  //
  if (PropertyChecker (MemoryCs->SerialNumber, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SerialNumber", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->SerialNumber, AsciiStringValue) != 0) {
        MemoryCs->SerialNumber = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle SPAREDEVICECOUNT
  //
  if (PropertyChecker (MemoryCs->SpareDeviceCount, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SpareDeviceCount", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->SpareDeviceCount != *NumericValue) {
        MemoryCs->SpareDeviceCount = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle SUBSYSTEMDEVICEID
  //
  if (PropertyChecker (MemoryCs->SubsystemDeviceID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SubsystemDeviceID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->SubsystemDeviceID, AsciiStringValue) != 0) {
        MemoryCs->SubsystemDeviceID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle SUBSYSTEMVENDORID
  //
  if (PropertyChecker (MemoryCs->SubsystemVendorID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SubsystemVendorID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->SubsystemVendorID, AsciiStringValue) != 0) {
        MemoryCs->SubsystemVendorID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle VENDORID
  //
  if (PropertyChecker (MemoryCs->VendorID, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"VendorID", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || AsciiStrCmp (MemoryCs->VendorID, AsciiStringValue) != 0) {
        MemoryCs->VendorID = AsciiStringValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle VOLATILEREGIONNUMBERLIMIT
  //
  if (PropertyChecker (MemoryCs->VolatileRegionNumberLimit, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"VolatileRegionNumberLimit", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->VolatileRegionNumberLimit != *NumericValue) {
        MemoryCs->VolatileRegionNumberLimit = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle VOLATILEREGIONSIZELIMITMIB
  //
  if (PropertyChecker (MemoryCs->VolatileRegionSizeLimitMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"VolatileRegionSizeLimitMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->VolatileRegionSizeLimitMiB != *NumericValue) {
        MemoryCs->VolatileRegionSizeLimitMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle VOLATILEREGIONSIZEMAXMIB
  //
  if (PropertyChecker (MemoryCs->VolatileRegionSizeMaxMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"VolatileRegionSizeMaxMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->VolatileRegionSizeMaxMiB != *NumericValue) {
        MemoryCs->VolatileRegionSizeMaxMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }
  //
  // Handle VOLATILESIZEMIB
  //
  if (PropertyChecker (MemoryCs->VolatileSizeMiB, ProvisionMode)) {
    NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"VolatileSizeMiB", ConfigureLang);
    if (NumericValue != NULL) {
      if (ProvisionMode || *MemoryCs->VolatileSizeMiB != *NumericValue) {
        MemoryCs->VolatileSizeMiB = NumericValue;
        PropertyChanged = TRUE;
      }
    }
  }

  //
  // Convert C structure back to JSON text.
  //
  Status = JsonStructProtocol->ToJson (
                                 JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)Memory,
                                 ResultJson
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, ToJson() failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Release resource.
  //
  JsonStructProtocol->DestoryStructure (
                        JsonStructProtocol,
                        (EFI_REST_JSON_STRUCTURE_HEADER *)Memory
                        );

  return (PropertyChanged ? EFI_SUCCESS : EFI_NOT_FOUND);
}

EFI_STATUS
ProvisioningMemoryResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE   *Private,
  IN  UINTN                             Index,
  IN  EFI_STRING                        ConfigureLang
  )
{
  CHAR8       *Json;
  EFI_STATUS  Status;
  EFI_STRING  NewResourceLocation;
  CHAR8       *EtagStr;
  CHAR8       ResourceId[16];

  if (IS_EMPTY_STRING (ConfigureLang) || Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr = NULL;
  AsciiSPrint (ResourceId, sizeof (ResourceId), "%d", Index);

  Status = ProvisioningMemoryProperties (
             Private->JsonStructProtocol,
             MemoryEmptyJson,
             ResourceId,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, provisioning resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    return Status;
  }

  Status = CreatePayloadToPostResource (Private->RedfishService, Private->Payload, Json, &NewResourceLocation, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, post Memory resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    goto RELEASE_RESOURCE;
  }

  ASSERT (NewResourceLocation != NULL);

  //
  // Keep location of new resource.
  //
  if (NewResourceLocation != NULL) {
    RedfisSetRedfishUri (ConfigureLang, NewResourceLocation);
  }

  //
  // Handle Etag
  //
  if (EtagStr != NULL) {
    SetEtagWithUri (EtagStr, NewResourceLocation);
    FreePool (EtagStr);
  }

RELEASE_RESOURCE:

  if (NewResourceLocation != NULL) {
    FreePool (NewResourceLocation);
  }

  if (Json != NULL) {
    FreePool (Json);
  }

  return Status;
}

EFI_STATUS
ProvisioningMemoryResources (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private
  )
{
  UINTN                                        Index;
  EFI_STATUS                                   Status;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  UnifiedConfigureLangList;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishFeatureGetUnifiedArrayTypeConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, REDPATH_ARRAY_PATTERN, &UnifiedConfigureLangList);
  if (EFI_ERROR (Status) || UnifiedConfigureLangList.Count == 0) {
    DEBUG ((DEBUG_ERROR, "%a, No HII question found with configure language: %s: %r\n", __FUNCTION__, REDPATH_ARRAY_PATTERN, Status));
    return EFI_NOT_FOUND;
  }
  //
  // Set the configuration language in the RESOURCE_INFORMATION_EXCHANGE.
  // This information is sent back to the parent resource (e.g. the collection driver).
  //
  EdkIIRedfishResourceSetConfigureLang (&UnifiedConfigureLangList);

  for (Index = 0; Index < UnifiedConfigureLangList.Count; Index++) {
    DEBUG ((DEBUG_INFO, "[%d] create Memory resource from: %s\n", UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang));
    ProvisioningMemoryResource (Private, UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang);
    FreePool (UnifiedConfigureLangList.List[Index].ConfigureLang);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ProvisioningMemoryExistResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private
  )
{
  EFI_STATUS Status;
  EFI_STRING ConfigureLang;
  CHAR8      *EtagStr;
  CHAR8      *Json;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr = NULL;
  Json = NULL;
  ConfigureLang = NULL;

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningMemoryProperties (
             Private->JsonStructProtocol,
             MemoryEmptyJson,
             NULL,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a, provisioning existing resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
    } else {
      DEBUG ((DEBUG_ERROR, "%a, provisioning existing resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    }
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a, provisioning existing resource for %s\n", __FUNCTION__, ConfigureLang));
  //
  // PUT back to instance
  //
  Status = CreatePayloadToPatchResource (Private->RedfishService, Private->Payload, Json, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, patch resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
  }

  //
  // Handle Etag
  //
  if (EtagStr != NULL) {
    SetEtagWithUri (EtagStr, Private->Uri);
    FreePool (EtagStr);
  }

ON_RELEASE:

  if (Json != NULL) {
    FreePool (Json);
  }

  if (ConfigureLang != NULL) {
    FreePool (ConfigureLang);
  }

  return Status;
}

/**
  Provisioning redfish resource by given URI.

  @param[in]   This                Pointer to EFI_HP_REDFISH_HII_PROTOCOL instance.
  @param[in]   ResourceExist       TRUE if resource exists, PUT method will be used.
                                   FALSE if resource does not exist POST method is used.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishProvisioningResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     BOOLEAN                          ResourceExist
  )
{
  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return (ResourceExist ? ProvisioningMemoryExistResource (Private) : ProvisioningMemoryResources (Private));
}

/**
  Check resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishCheckResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json
  )
{
  UINTN      Index;
  EFI_STATUS Status;
  EFI_STRING *ConfigureLangList;
  UINTN      Count;
  EFI_STRING Property;

  if (Private == NULL || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishPlatformConfigGetConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, REDPATH_ARRAY_PATTERN, &ConfigureLangList, &Count);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, BiosConfigToRedfishGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < Count; Index++) {

    Property = GetPropertyFromConfigureLang (Private->Uri, ConfigureLangList[Index]);
    if (Property == NULL) {
      continue;
    }

    DEBUG ((DEBUG_INFO, "%a, [%d] check attribute for: %s\n", __FUNCTION__, Index, Property));
    if (!MatchPropertyWithJsonContext (Property, Json)) {
      DEBUG ((DEBUG_INFO, "%a, property is missing: %s\n", __FUNCTION__, Property));
      Status = EFI_NOT_FOUND;
    }
  }

  FreePool (ConfigureLangList);

  return Status;
}

/**
  Update resource to given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishUpdateResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *InputJson
  )
{
  EFI_STATUS Status;
  CHAR8      *Json;
  EFI_STRING ConfigureLang;
  CHAR8      *EtagStr;

  if (Private == NULL || IS_EMPTY_STRING (InputJson)) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr = NULL;
  Json = NULL;
  ConfigureLang = NULL;

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningMemoryProperties (
             Private->JsonStructProtocol,
             InputJson,
             NULL,
             ConfigureLang,
             FALSE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a, update resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
    } else {
      DEBUG ((DEBUG_ERROR, "%a, update resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    }
    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a, update resource for %s\n", __FUNCTION__, ConfigureLang));
  //
  // PUT back to instance
  //
  Status = CreatePayloadToPatchResource (Private->RedfishService, Private->Payload, Json, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a, patch resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
  }

  //
  // Handle Etag
  //
  if (EtagStr != NULL) {
    SetEtagWithUri (EtagStr, Private->Uri);
    FreePool (EtagStr);
  }

ON_RELEASE:

  if (Json != NULL) {
    FreePool (Json);
  }

  if (ConfigureLang != NULL) {
    FreePool (ConfigureLang);
  }

  return Status;
}

/**
  Identify resource from given URI.

  @param[in]   This                Pointer to REDFISH_RESOURCE_COMMON_PRIVATE instance.
  @param[in]   Json                The JSON to consume.

  @retval EFI_SUCCESS              Value is returned successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
RedfishIdentifyResourceCommon (
  IN     REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN     CHAR8                            *Json
  )
{
  BOOLEAN     Supported;
  EFI_STATUS  Status;
  EFI_STRING  EndOfChar;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST ConfigLangList;

  Supported = RedfishIdentifyResource (Private->Uri, Private->Json);
  if (Supported) {
    Status = RedfishFeatureGetUnifiedArrayTypeConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, REDPATH_ARRAY_PATTERN, &ConfigLangList);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a, BiosConfigToRedfishGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
      return Status;
    }

    if (ConfigLangList.Count == 0) {
      return EFI_SUCCESS;
    }

    EndOfChar = StrStr (ConfigLangList.List[0].ConfigureLang, L"}");
    if (EndOfChar == NULL) {
      ASSERT (FALSE);
      return EFI_DEVICE_ERROR;
    }

    //EndOfChar = StrStr (ConfigLangList.List[0].ConfigureLang, L"}");
    Status = IsRedpathArray (ConfigLangList.List[0].ConfigureLang, NULL, &EndOfChar);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      ASSERT (FALSE);
      return EFI_DEVICE_ERROR;
    }
    if (Status != EFI_SUCCESS) {
      //
      // This is not the collection redpath.
      //
      GetRedpathNodeByIndex (ConfigLangList.List[0].ConfigureLang, 0, &EndOfChar);
    }
    *(++EndOfChar) = '\0';
    //
    // Keep URI and ConfigLang mapping
    //
    RedfisSetRedfishUri (ConfigLangList.List[0].ConfigureLang, Private->Uri);
    //
    // Set the configuration language in the RESOURCE_INFORMATION_EXCHANGE.
    // This information is sent back to the parent resource (e.g. the collection driver).
    //
    EdkIIRedfishResourceSetConfigureLang (&ConfigLangList);
    DestroyConfiglanguageList (&ConfigLangList);
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}