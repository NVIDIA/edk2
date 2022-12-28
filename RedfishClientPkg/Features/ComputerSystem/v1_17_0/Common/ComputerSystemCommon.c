/** @file
  Redfish feature driver implementation - common functions

  (C) Copyright 2020-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ComputerSystemCommon.h"

CHAR8  ComputerSystemEmptyJson[] = "{\"@odata.id\": \"\", \"@odata.type\": \"#ComputerSystem.v1_17_0.ComputerSystem\", \"Id\": \"\", \"Name\": \"\", \"Boot\":{}}";

REDFISH_RESOURCE_COMMON_PRIVATE  *mRedfishResourcePrivate = NULL;

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
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN  CHAR8                            *Json,
  IN  CHAR8                            *HeaderEtag OPTIONAL
  )
{
  EFI_STATUS                             Status;
  EFI_REDFISH_COMPUTERSYSTEM_V1_17_0     *ComputerSystem;
  EFI_REDFISH_COMPUTERSYSTEM_V1_17_0_CS  *ComputerSystemCs;
  EFI_STRING                             ConfigureLang;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  ComputerSystem   = NULL;
  ComputerSystemCs = NULL;
  ConfigureLang    = NULL;

  Status = Private->JsonStructProtocol->ToStructure (
                                          Private->JsonStructProtocol,
                                          NULL,
                                          Json,
                                          (EFI_REST_JSON_STRUCTURE_HEADER **)&ComputerSystem
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure() failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  ComputerSystemCs = ComputerSystem->ComputerSystem;

  //
  // Check ETAG to see if we need to consume it
  //
  if (CheckEtag (Private->Uri, HeaderEtag, NULL)) {
    //
    // No change
    //
    DEBUG ((DEBUG_INFO, "%a: ETAG: %s has no change, ignore consume action\n", __FUNCTION__, Private->Uri));
    Status = EFI_ALREADY_STARTED;
    goto ON_RELEASE;
  }

  //
  // Handle ASSETTAG
  //
  if (ComputerSystemCs->AssetTag != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "AssetTag");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->AssetTag);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // BIOS will be handled by feature driver.
  //

  //
  // Handle BIOSVERSION
  //
  if (ComputerSystemCs->BiosVersion != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "BiosVersion");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->BiosVersion);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT
  //
  if (ComputerSystemCs->Boot == NULL) {
    ComputerSystemCs->Boot = AllocateZeroPool (sizeof (RedfishComputerSystem_V1_17_0_Boot_CS));
    ASSERT (ComputerSystemCs->Boot != NULL);
  }

  //
  // Handle BOOT->BOOTNEXT
  //
  if (ComputerSystemCs->Boot->BootNext != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/BootNext");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->BootNext);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT->BOOTORDER
  //
  if (ComputerSystemCs->Boot->BootOrder != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/BootOrder");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringArrayType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->BootOrder);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT->BOOTSOURCEOVERRIDEENABLED
  //
  if (ComputerSystemCs->Boot->BootSourceOverrideEnabled != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/BootSourceOverrideEnabled");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->BootSourceOverrideEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT->BOOTSOURCEOVERRIDEMODE
  //
  if (ComputerSystemCs->Boot->BootSourceOverrideMode != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/BootSourceOverrideMode");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->BootSourceOverrideMode);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT->BOOTSOURCEOVERRIDETARGET
  //
  if (ComputerSystemCs->Boot->BootSourceOverrideTarget != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/BootSourceOverrideTarget");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->BootSourceOverrideTarget);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle BOOT->UEFITARGETBOOTSOURCEOVERRIDE
  //
  if (ComputerSystemCs->Boot->UefiTargetBootSourceOverride != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Boot/UefiTargetBootSourceOverride");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Boot->UefiTargetBootSourceOverride);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // ETHERNETINTERFACES will be handled by collection driver.
  //

  //
  // Handle HOSTNAME
  //
  if (ComputerSystemCs->HostName != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "HostName");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->HostName);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle HOSTWATCHDOGTIMER
  //
  if (ComputerSystemCs->HostWatchdogTimer == NULL) {
    ComputerSystemCs->HostWatchdogTimer = AllocateZeroPool (sizeof (RedfishComputerSystem_V1_17_0_WatchdogTimer_CS));
    ASSERT (ComputerSystemCs->HostWatchdogTimer != NULL);
  }

  //
  // Handle HOSTWATCHDOGTIMER->FUNCTIONENABLED
  //
  if (ComputerSystemCs->HostWatchdogTimer->FunctionEnabled != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "HostWatchdogTimer/FunctionEnabled");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsBooleanType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (BOOLEAN)*ComputerSystemCs->HostWatchdogTimer->FunctionEnabled);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle HOSTWATCHDOGTIMER->TIMEOUTACTION
  //
  if (ComputerSystemCs->HostWatchdogTimer->TimeoutAction != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "HostWatchdogTimer/TimeoutAction");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->HostWatchdogTimer->TimeoutAction);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle HOSTWATCHDOGTIMER->WARNINGACTION
  //
  if (ComputerSystemCs->HostWatchdogTimer->WarningAction != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "HostWatchdogTimer/WarningAction");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->HostWatchdogTimer->WarningAction);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle HOSTEDSERVICES
  //
  if (ComputerSystemCs->HostedServices == NULL) {
    ComputerSystemCs->HostedServices = AllocateZeroPool (sizeof (RedfishComputerSystem_V1_17_0_HostedServices_CS));
    ASSERT (ComputerSystemCs->HostedServices != NULL);
  }

  //
  // Handle HOSTINGROLES
  //
  //
  // ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle INDICATORLED
  //
  if (ComputerSystemCs->IndicatorLED != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "IndicatorLED");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->IndicatorLED);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // LOGSERVICES will be handled by collection driver.
  //

  //
  // Handle MANUFACTURER
  //
  if (ComputerSystemCs->Manufacturer != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Manufacturer");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Manufacturer);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // MEMORY will be handled by collection driver.
  //

  //
  // Handle MEMORYSUMMARY
  //
  if (ComputerSystemCs->MemorySummary == NULL) {
    ComputerSystemCs->MemorySummary = AllocateZeroPool (sizeof (RedfishComputerSystem_V1_17_0_MemorySummary_CS));
    ASSERT (ComputerSystemCs->MemorySummary != NULL);
  }

  //
  // Handle MEMORYSUMMARY->MEMORYMIRRORING
  //
  if (ComputerSystemCs->MemorySummary->MemoryMirroring != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "MemorySummary/MemoryMirroring");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->MemorySummary->MemoryMirroring);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYSUMMARY->TOTALSYSTEMMEMORYGIB
  //
  if (ComputerSystemCs->MemorySummary->TotalSystemMemoryGiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "MemorySummary/TotalSystemMemoryGiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*ComputerSystemCs->MemorySummary->TotalSystemMemoryGiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MEMORYSUMMARY->TOTALSYSTEMPERSISTENTMEMORYGIB
  //
  if (ComputerSystemCs->MemorySummary->TotalSystemPersistentMemoryGiB != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "MemorySummary/TotalSystemPersistentMemoryGiB");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*ComputerSystemCs->MemorySummary->TotalSystemPersistentMemoryGiB);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle MODEL
  //
  if (ComputerSystemCs->Model != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "Model");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->Model);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // NETWORKINTERFACES will be handled by collection driver.
  //

  //
  // Handle PCIEDEVICES
  //
  //
  // ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle PCIEFUNCTIONS
  //
  //
  // ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle PARTNUMBER
  //
  if (ComputerSystemCs->PartNumber != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "PartNumber");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->PartNumber);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle POWERSTATE
  //
  if (ComputerSystemCs->PowerState != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "PowerState");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->PowerState);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PROCESSORSUMMARY
  //
  if (ComputerSystemCs->ProcessorSummary == NULL) {
    ComputerSystemCs->ProcessorSummary = AllocateZeroPool (sizeof (RedfishComputerSystem_V1_17_0_ProcessorSummary_CS));
    ASSERT (ComputerSystemCs->ProcessorSummary != NULL);
  }

  //
  // Handle PROCESSORSUMMARY->COUNT
  //
  if (ComputerSystemCs->ProcessorSummary->Count != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "ProcessorSummary/Count");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*ComputerSystemCs->ProcessorSummary->Count);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PROCESSORSUMMARY->LOGICALPROCESSORCOUNT
  //
  if (ComputerSystemCs->ProcessorSummary->LogicalProcessorCount != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "ProcessorSummary/LogicalProcessorCount");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsNumericType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, (UINTN)*ComputerSystemCs->ProcessorSummary->LogicalProcessorCount);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle PROCESSORSUMMARY->MODEL
  //
  if (ComputerSystemCs->ProcessorSummary->Model != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "ProcessorSummary/Model");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->ProcessorSummary->Model);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // PROCESSORS will be handled by collection driver.
  //

  //
  // Handle REDUNDANCY
  //
  //
  // ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle SKU
  //
  if (ComputerSystemCs->SKU != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "SKU");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->SKU);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // SECUREBOOT will be handled by feature driver.
  //

  //
  // Handle SERIALNUMBER
  //
  if (ComputerSystemCs->SerialNumber != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "SerialNumber");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->SerialNumber);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // SIMPLESTORAGE will be handled by collection driver.
  //

  //
  // STORAGE will be handled by collection driver.
  //

  //
  // Handle SUBMODEL
  //
  if (ComputerSystemCs->SubModel != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "SubModel");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->SubModel);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle SYSTEMTYPE
  //
  if (ComputerSystemCs->SystemType != NULL) {
    //
    // Find corresponding configure language for collection resource.
    //
    ConfigureLang = GetConfigureLang (ComputerSystemCs->odata_id, "SystemType");
    if (ConfigureLang != NULL) {
      Status = ApplyFeatureSettingsStringType (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, ConfigureLang, ComputerSystemCs->SystemType);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: apply setting for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
      }

      FreePool (ConfigureLang);
    } else {
      DEBUG ((DEBUG_ERROR, "%a: can not get configure language for URI: %s\n", __FUNCTION__, Private->Uri));
    }
  }

  //
  // Handle TRUSTEDMODULES
  //
  //
  // ****** Warning ******
  // Unsupported array type:
  //

ON_RELEASE:

  //
  // Release resource.
  //
  Private->JsonStructProtocol->DestoryStructure (
                                 Private->JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)ComputerSystem
                                 );

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningComputerSystemProperties (
  IN  EFI_REST_JSON_STRUCTURE_PROTOCOL *JsonStructProtocol,
  IN  CHAR8 *InputJson,
  IN  CHAR8 *ResourceId, OPTIONAL
  IN  EFI_STRING                        ConfigureLang,
  IN  BOOLEAN                           ProvisionMode,
  OUT CHAR8                             **ResultJson
  )
{
  EFI_REDFISH_COMPUTERSYSTEM_V1_17_0     *ComputerSystem;
  EFI_REDFISH_COMPUTERSYSTEM_V1_17_0_CS  *ComputerSystemCs;
  EFI_STATUS                             Status;
  BOOLEAN                                PropertyChanged;
  CHAR8                                  *AsciiStringValue;
  CHAR8                                  **AsciiStringArrayValue;
  UINTN                                  ArraySize;
  BOOLEAN                                *BooleanValue;
  INT32                                  *IntegerValue;
  INT64                                  *NumericValue;

  if ((JsonStructProtocol == NULL) || (ResultJson == NULL) || IS_EMPTY_STRING (InputJson) || IS_EMPTY_STRING (ConfigureLang)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a provision for %s with: %s\n", __FUNCTION__, ConfigureLang, (ProvisionMode ? L"Provision resource" : L"Update resource")));

  *ResultJson     = NULL;
  PropertyChanged = FALSE;

  ComputerSystem = NULL;
  Status         = JsonStructProtocol->ToStructure (
                                         JsonStructProtocol,
                                         NULL,
                                         InputJson,
                                         (EFI_REST_JSON_STRUCTURE_HEADER **)&ComputerSystem
                                         );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToStructure failure: %r\n", __FUNCTION__, Status));
    return Status;
  }

  ComputerSystemCs = ComputerSystem->ComputerSystem;

  //
  // ID
  //
  if ((ComputerSystemCs->Id == NULL) && !IS_EMPTY_STRING (ResourceId)) {
    ComputerSystemCs->Id = AllocateCopyPool (AsciiStrSize (ResourceId), ResourceId);
  }

  //
  // Handle ASSETTAG
  //
  if (PropertyChecker (ComputerSystemCs->AssetTag, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"AssetTag", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->AssetTag, AsciiStringValue) != 0)) {
        ComputerSystemCs->AssetTag = AsciiStringValue;
        PropertyChanged            = TRUE;
      }
    }
  }

  //
  // Handle BIOSVERSION
  //
  if (PropertyChecker (ComputerSystemCs->BiosVersion, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"BiosVersion", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->BiosVersion, AsciiStringValue) != 0)) {
        ComputerSystemCs->BiosVersion = AsciiStringValue;
        PropertyChanged               = TRUE;
      }
    }
  }

  //
  // Handle BOOT
  //
  if (ComputerSystemCs->Boot != NULL) {
    //
    // Handle BOOT->BOOTNEXT
    //
    if (PropertyChecker (ComputerSystemCs->Boot->BootNext, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/BootNext", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Boot->BootNext, AsciiStringValue) != 0)) {
          ComputerSystemCs->Boot->BootNext = AsciiStringValue;
          PropertyChanged                  = TRUE;
        }
      }
    }

    //
    // Handle BOOT->BOOTORDER
    //
    if (PropertyChecker (ComputerSystemCs->Boot->BootOrder, ProvisionMode)) {
      AsciiStringArrayValue = GetPropertyStringArrayValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/BootOrder", ConfigureLang, &ArraySize);
      if (AsciiStringArrayValue != NULL) {
        if (ProvisionMode || !CompareRedfishStringArrayValues (ComputerSystemCs->Boot->BootOrder, AsciiStringArrayValue, ArraySize)) {
          AddRedfishCharArray (&ComputerSystemCs->Boot->BootOrder, AsciiStringArrayValue, ArraySize);
          PropertyChanged = TRUE;
        }
      }
    }

    //
    // Handle BOOT->BOOTSOURCEOVERRIDEENABLED
    //
    if (PropertyChecker (ComputerSystemCs->Boot->BootSourceOverrideEnabled, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/BootSourceOverrideEnabled", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Boot->BootSourceOverrideEnabled, AsciiStringValue) != 0)) {
          ComputerSystemCs->Boot->BootSourceOverrideEnabled = AsciiStringValue;
          PropertyChanged                                   = TRUE;
        }
      }
    }

    //
    // Handle BOOT->BOOTSOURCEOVERRIDEMODE
    //
    if (PropertyChecker (ComputerSystemCs->Boot->BootSourceOverrideMode, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/BootSourceOverrideMode", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Boot->BootSourceOverrideMode, AsciiStringValue) != 0)) {
          ComputerSystemCs->Boot->BootSourceOverrideMode = AsciiStringValue;
          PropertyChanged                                = TRUE;
        }
      }
    }

    //
    // Handle BOOT->BOOTSOURCEOVERRIDETARGET
    //
    if (PropertyChecker (ComputerSystemCs->Boot->BootSourceOverrideTarget, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/BootSourceOverrideTarget", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Boot->BootSourceOverrideTarget, AsciiStringValue) != 0)) {
          ComputerSystemCs->Boot->BootSourceOverrideTarget = AsciiStringValue;
          PropertyChanged                                  = TRUE;
        }
      }
    }

    //
    // Handle BOOT->UEFITARGETBOOTSOURCEOVERRIDE
    //
    if (PropertyChecker (ComputerSystemCs->Boot->UefiTargetBootSourceOverride, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Boot/UefiTargetBootSourceOverride", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Boot->UefiTargetBootSourceOverride, AsciiStringValue) != 0)) {
          ComputerSystemCs->Boot->UefiTargetBootSourceOverride = AsciiStringValue;
          PropertyChanged                                      = TRUE;
        }
      }
    }
  }

  //
  // Handle HOSTNAME
  //
  if (PropertyChecker (ComputerSystemCs->HostName, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"HostName", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->HostName, AsciiStringValue) != 0)) {
        ComputerSystemCs->HostName = AsciiStringValue;
        PropertyChanged            = TRUE;
      }
    }
  }

  //
  // Handle HOSTWATCHDOGTIMER
  //
  if (ComputerSystemCs->HostWatchdogTimer != NULL) {
    //
    // Handle HOSTWATCHDOGTIMER->FUNCTIONENABLED
    //
    if (PropertyChecker (ComputerSystemCs->HostWatchdogTimer->FunctionEnabled, ProvisionMode)) {
      BooleanValue = GetPropertyBooleanValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"HostWatchdogTimer/FunctionEnabled", ConfigureLang);
      if (BooleanValue != NULL) {
        if (ProvisionMode || (*ComputerSystemCs->HostWatchdogTimer->FunctionEnabled != *BooleanValue)) {
          IntegerValue = AllocatePool (sizeof (*IntegerValue));
          if (IntegerValue != NULL) {
            *IntegerValue                                        = (BooleanValue ? 0x01 : 0x00);
            ComputerSystemCs->HostWatchdogTimer->FunctionEnabled = IntegerValue;
            PropertyChanged                                      = TRUE;
          }
        }
      }
    }

    //
    // Handle HOSTWATCHDOGTIMER->TIMEOUTACTION
    //
    if (PropertyChecker (ComputerSystemCs->HostWatchdogTimer->TimeoutAction, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"HostWatchdogTimer/TimeoutAction", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->HostWatchdogTimer->TimeoutAction, AsciiStringValue) != 0)) {
          ComputerSystemCs->HostWatchdogTimer->TimeoutAction = AsciiStringValue;
          PropertyChanged                                    = TRUE;
        }
      }
    }

    //
    // Handle HOSTWATCHDOGTIMER->WARNINGACTION
    //
    if (PropertyChecker (ComputerSystemCs->HostWatchdogTimer->WarningAction, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"HostWatchdogTimer/WarningAction", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->HostWatchdogTimer->WarningAction, AsciiStringValue) != 0)) {
          ComputerSystemCs->HostWatchdogTimer->WarningAction = AsciiStringValue;
          PropertyChanged                                    = TRUE;
        }
      }
    }
  }

  //
  // Handle HOSTEDSERVICES
  //
  if (ComputerSystemCs->HostedServices != NULL) {
  }

  //
  // Handle HOSTINGROLES
  //
  //// ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle INDICATORLED
  //
  if (PropertyChecker (ComputerSystemCs->IndicatorLED, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"IndicatorLED", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->IndicatorLED, AsciiStringValue) != 0)) {
        ComputerSystemCs->IndicatorLED = AsciiStringValue;
        PropertyChanged                = TRUE;
      }
    }
  }

  //
  // Handle MANUFACTURER
  //
  if (PropertyChecker (ComputerSystemCs->Manufacturer, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Manufacturer", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Manufacturer, AsciiStringValue) != 0)) {
        ComputerSystemCs->Manufacturer = AsciiStringValue;
        PropertyChanged                = TRUE;
      }
    }
  }

  //
  // Handle MEMORYSUMMARY
  //
  if (ComputerSystemCs->MemorySummary != NULL) {
    //
    // Handle MEMORYSUMMARY->MEMORYMIRRORING
    //
    if (PropertyChecker (ComputerSystemCs->MemorySummary->MemoryMirroring, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemorySummary/MemoryMirroring", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->MemorySummary->MemoryMirroring, AsciiStringValue) != 0)) {
          ComputerSystemCs->MemorySummary->MemoryMirroring = AsciiStringValue;
          PropertyChanged                                  = TRUE;
        }
      }
    }

    //
    // Handle MEMORYSUMMARY->TOTALSYSTEMMEMORYGIB
    //
    if (PropertyChecker (ComputerSystemCs->MemorySummary->TotalSystemMemoryGiB, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemorySummary/TotalSystemMemoryGiB", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || (*ComputerSystemCs->MemorySummary->TotalSystemMemoryGiB != *NumericValue)) {
          ComputerSystemCs->MemorySummary->TotalSystemMemoryGiB = NumericValue;
          PropertyChanged                                       = TRUE;
        }
      }
    }

    //
    // Handle MEMORYSUMMARY->TOTALSYSTEMPERSISTENTMEMORYGIB
    //
    if (PropertyChecker (ComputerSystemCs->MemorySummary->TotalSystemPersistentMemoryGiB, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"MemorySummary/TotalSystemPersistentMemoryGiB", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || (*ComputerSystemCs->MemorySummary->TotalSystemPersistentMemoryGiB != *NumericValue)) {
          ComputerSystemCs->MemorySummary->TotalSystemPersistentMemoryGiB = NumericValue;
          PropertyChanged                                                 = TRUE;
        }
      }
    }
  }

  //
  // Handle MODEL
  //
  if (PropertyChecker (ComputerSystemCs->Model, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"Model", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->Model, AsciiStringValue) != 0)) {
        ComputerSystemCs->Model = AsciiStringValue;
        PropertyChanged         = TRUE;
      }
    }
  }

  //
  // Handle PCIEDEVICES
  //
  //// ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle PCIEFUNCTIONS
  //
  //// ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle PARTNUMBER
  //
  if (PropertyChecker (ComputerSystemCs->PartNumber, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PartNumber", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->PartNumber, AsciiStringValue) != 0)) {
        ComputerSystemCs->PartNumber = AsciiStringValue;
        PropertyChanged              = TRUE;
      }
    }
  }

  //
  // Handle POWERSTATE
  //
  if (PropertyChecker (ComputerSystemCs->PowerState, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"PowerState", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->PowerState, AsciiStringValue) != 0)) {
        ComputerSystemCs->PowerState = AsciiStringValue;
        PropertyChanged              = TRUE;
      }
    }
  }

  //
  // Handle PROCESSORSUMMARY
  //
  if (ComputerSystemCs->ProcessorSummary != NULL) {
    //
    // Handle PROCESSORSUMMARY->COUNT
    //
    if (PropertyChecker (ComputerSystemCs->ProcessorSummary->Count, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ProcessorSummary/Count", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || (*ComputerSystemCs->ProcessorSummary->Count != *NumericValue)) {
          ComputerSystemCs->ProcessorSummary->Count = NumericValue;
          PropertyChanged                           = TRUE;
        }
      }
    }

    //
    // Handle PROCESSORSUMMARY->LOGICALPROCESSORCOUNT
    //
    if (PropertyChecker (ComputerSystemCs->ProcessorSummary->LogicalProcessorCount, ProvisionMode)) {
      NumericValue = GetPropertyNumericValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ProcessorSummary/LogicalProcessorCount", ConfigureLang);
      if (NumericValue != NULL) {
        if (ProvisionMode || (*ComputerSystemCs->ProcessorSummary->LogicalProcessorCount != *NumericValue)) {
          ComputerSystemCs->ProcessorSummary->LogicalProcessorCount = NumericValue;
          PropertyChanged                                           = TRUE;
        }
      }
    }

    //
    // Handle PROCESSORSUMMARY->MODEL
    //
    if (PropertyChecker (ComputerSystemCs->ProcessorSummary->Model, ProvisionMode)) {
      AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"ProcessorSummary/Model", ConfigureLang);
      if (AsciiStringValue != NULL) {
        if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->ProcessorSummary->Model, AsciiStringValue) != 0)) {
          ComputerSystemCs->ProcessorSummary->Model = AsciiStringValue;
          PropertyChanged                           = TRUE;
        }
      }
    }
  }

  //
  // Handle REDUNDANCY
  //
  //// ****** Warning ******
  // Unsupported array type:
  //

  //
  // Handle SKU
  //
  if (PropertyChecker (ComputerSystemCs->SKU, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SKU", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->SKU, AsciiStringValue) != 0)) {
        ComputerSystemCs->SKU = AsciiStringValue;
        PropertyChanged       = TRUE;
      }
    }
  }

  //
  // Handle SERIALNUMBER
  //
  if (PropertyChecker (ComputerSystemCs->SerialNumber, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SerialNumber", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->SerialNumber, AsciiStringValue) != 0)) {
        ComputerSystemCs->SerialNumber = AsciiStringValue;
        PropertyChanged                = TRUE;
      }
    }
  }

  //
  // Handle SUBMODEL
  //
  if (PropertyChecker (ComputerSystemCs->SubModel, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SubModel", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->SubModel, AsciiStringValue) != 0)) {
        ComputerSystemCs->SubModel = AsciiStringValue;
        PropertyChanged            = TRUE;
      }
    }
  }

  //
  // Handle SYSTEMTYPE
  //
  if (PropertyChecker (ComputerSystemCs->SystemType, ProvisionMode)) {
    AsciiStringValue = GetPropertyStringValue (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, L"SystemType", ConfigureLang);
    if (AsciiStringValue != NULL) {
      if (ProvisionMode || (AsciiStrCmp (ComputerSystemCs->SystemType, AsciiStringValue) != 0)) {
        ComputerSystemCs->SystemType = AsciiStringValue;
        PropertyChanged              = TRUE;
      }
    }
  }

  //
  // Handle TRUSTEDMODULES
  //
  //// ****** Warning ******
  // Unsupported array type:
  //

  //
  // Convert C structure back to JSON text.
  //
  Status = JsonStructProtocol->ToJson (
                                 JsonStructProtocol,
                                 (EFI_REST_JSON_STRUCTURE_HEADER *)ComputerSystem,
                                 ResultJson
                                 );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: ToJson() failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  //
  // Release resource.
  //
  JsonStructProtocol->DestoryStructure (
                        JsonStructProtocol,
                        (EFI_REST_JSON_STRUCTURE_HEADER *)ComputerSystem
                        );

  return (PropertyChanged ? EFI_SUCCESS : EFI_NOT_FOUND);
}

EFI_STATUS
ProvisioningComputerSystemResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private,
  IN  UINTN                            Index,
  IN  EFI_STRING                       ConfigureLang
  )
{
  CHAR8       *Json;
  EFI_STATUS  Status;
  EFI_STRING  NewResourceLocation;
  CHAR8       *EtagStr;
  CHAR8       ResourceId[16];

  if (IS_EMPTY_STRING (ConfigureLang) || (Private == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr = NULL;
  AsciiSPrint (ResourceId, sizeof (ResourceId), "%d", Index);

  Status = ProvisioningComputerSystemProperties (
             Private->JsonStructProtocol,
             ComputerSystemEmptyJson,
             ResourceId,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: provisioning resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    return Status;
  }

  Status = CreatePayloadToPostResource (Private->RedfishService, Private->Payload, Json, &NewResourceLocation, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: post ComputerSystem resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
ProvisioningComputerSystemResources (
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
  if (EFI_ERROR (Status) || (UnifiedConfigureLangList.Count == 0)) {
    DEBUG ((DEBUG_ERROR, "%a: No HII question found with configure language: %s: %r\n", __FUNCTION__, REDPATH_ARRAY_PATTERN, Status));
    return EFI_NOT_FOUND;
  }

  //
  // Set the configuration language in the RESOURCE_INFORMATION_EXCHANGE.
  // This information is sent back to the parent resource (e.g. the collection driver).
  //
  EdkIIRedfishResourceSetConfigureLang (&UnifiedConfigureLangList);

  for (Index = 0; Index < UnifiedConfigureLangList.Count; Index++) {
    DEBUG ((DEBUG_INFO, "[%d] create ComputerSystem resource from: %s\n", UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang));
    ProvisioningComputerSystemResource (Private, UnifiedConfigureLangList.List[Index].Index, UnifiedConfigureLangList.List[Index].ConfigureLang);
    FreePool (UnifiedConfigureLangList.List[Index].ConfigureLang);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ProvisioningComputerSystemExistResource (
  IN  REDFISH_RESOURCE_COMMON_PRIVATE  *Private
  )
{
  EFI_STATUS  Status;
  EFI_STRING  ConfigureLang;
  CHAR8       *EtagStr;
  CHAR8       *Json;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr       = NULL;
  Json          = NULL;
  ConfigureLang = NULL;

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningComputerSystemProperties (
             Private->JsonStructProtocol,
             ComputerSystemEmptyJson,
             NULL,
             ConfigureLang,
             TRUE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: provisioning existing resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: provisioning existing resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    }

    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: provisioning existing resource for %s\n", __FUNCTION__, ConfigureLang));
  //
  // PUT back to instance
  //
  Status = CreatePayloadToPatchResource (Private->RedfishService, Private->Payload, Json, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: patch resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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

  return (ResourceExist ? ProvisioningComputerSystemExistResource (Private) : ProvisioningComputerSystemResources (Private));
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
  UINTN       Index;
  EFI_STATUS  Status;
  EFI_STRING  *ConfigureLangList;
  UINTN       Count;
  EFI_STRING  Property;

  if ((Private == NULL) || IS_EMPTY_STRING (Json)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = RedfishPlatformConfigGetConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, REDPATH_ARRAY_PATTERN, &ConfigureLangList, &Count);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: BiosConfigToRedfishGetConfigureLangRegex failed: %r\n", __FUNCTION__, Status));
    return Status;
  }

  if (Count == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < Count; Index++) {
    Property = GetPropertyFromConfigureLang (Private->Uri, ConfigureLangList[Index]);
    if (Property == NULL) {
      continue;
    }

    DEBUG ((DEBUG_INFO, "%a: [%d] check attribute for: %s\n", __FUNCTION__, Index, Property));
    if (!MatchPropertyWithJsonContext (Property, Json)) {
      DEBUG ((DEBUG_INFO, "%a: property is missing: %s\n", __FUNCTION__, Property));
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
  EFI_STATUS  Status;
  CHAR8       *Json;
  EFI_STRING  ConfigureLang;
  CHAR8       *EtagStr;

  if ((Private == NULL) || IS_EMPTY_STRING (InputJson)) {
    return EFI_INVALID_PARAMETER;
  }

  EtagStr       = NULL;
  Json          = NULL;
  ConfigureLang = NULL;

  ConfigureLang = RedfishGetConfigLanguage (Private->Uri);
  if (ConfigureLang == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = ProvisioningComputerSystemProperties (
             Private->JsonStructProtocol,
             InputJson,
             NULL,
             ConfigureLang,
             FALSE,
             &Json
             );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      DEBUG ((REDFISH_DEBUG_TRACE, "%a: update resource for %s ignored. Nothing changed\n", __FUNCTION__, ConfigureLang));
    } else {
      DEBUG ((DEBUG_ERROR, "%a: update resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
    }

    goto ON_RELEASE;
  }

  DEBUG ((REDFISH_DEBUG_TRACE, "%a: update resource for %s\n", __FUNCTION__, ConfigureLang));
  //
  // PUT back to instance
  //
  Status = CreatePayloadToPatchResource (Private->RedfishService, Private->Payload, Json, &EtagStr);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: patch resource for %s failed: %r\n", __FUNCTION__, ConfigureLang, Status));
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
  BOOLEAN                                      Supported;
  EFI_STATUS                                   Status;
  EFI_STRING                                   EndOfChar;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  ConfigLangList;

  Supported = RedfishIdentifyResource (Private->Uri, Private->Json);
  if (Supported) {
    Status = RedfishFeatureGetUnifiedArrayTypeConfigureLang (RESOURCE_SCHEMA, RESOURCE_SCHEMA_VERSION, REDPATH_ARRAY_PATTERN, &ConfigLangList);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: RedfishFeatureGetUnifiedArrayTypeConfigureLang failed: %r\n", __FUNCTION__, Status));
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

    // EndOfChar = StrStr (ConfigLangList.List[0].ConfigureLang, L"}");
    Status = IsRedpathArray (ConfigLangList.List[0].ConfigureLang, NULL, &EndOfChar);
    if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
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
