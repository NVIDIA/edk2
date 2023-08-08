/** @file
  RedfishFeatureCoreDxe produces EdkIIRedfishFeatureCoreProtocol
  for EDK2 Redfish Feature driver registration.

  (C) Copyright 2021-2022 Hewlett Packard Enterprise Development LP<BR>
  Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <RedfishFeatureCoreDxe.h>

EFI_EVENT                        mEdkIIRedfishFeatureDriverStartupEvent;
REDFISH_FEATURE_STARTUP_CONTEXT  mFeatureDriverStartupContext;
REDFISH_FEATURE_INTERNAL_DATA    *ResourceUriNodeList;
REDFISH_FEATURE_INTERNAL_DATA    *mTaskServiceNode;
RESOURCE_INFORMATION_EXCHANGE    *mInformationExchange;

/**
  Setup the information to deliver to child feature/collection driver.

  @param[in]  ThisList                 REDFISH_FEATURE_INTERNAL_DATA instance.
  @param[in]  ParentConfgLanguageUri   Parent configure language URI.

**/
EFI_STATUS
SetupExchangeInformationInfo (
  IN REDFISH_FEATURE_INTERNAL_DATA  *ThisList,
  IN EFI_STRING                     ParentConfgLanguageUri
  )
{
  ThisList->InformationExchange->SendInformation.ParentUri = (EFI_STRING)AllocateZeroPool (MaxParentUriLength * sizeof (CHAR16));
  if (ThisList->InformationExchange->SendInformation.ParentUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ThisList->InformationExchange->SendInformation.PropertyName = (EFI_STRING)AllocateZeroPool (MaxNodeNameLength * sizeof (CHAR16));
  if (ThisList->InformationExchange->SendInformation.PropertyName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ThisList->InformationExchange->SendInformation.FullUri = (EFI_STRING)AllocateZeroPool (MaxParentUriLength * sizeof (CHAR16));
  if (ThisList->InformationExchange->SendInformation.FullUri == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Setup property name
  //
  StrCpyS (ThisList->InformationExchange->SendInformation.PropertyName, MaxNodeNameLength, ThisList->NodeName);

  //
  // Setup parent config language URI
  //
  StrCpyS (ThisList->InformationExchange->SendInformation.ParentUri, MaxParentUriLength, ParentConfgLanguageUri);

  //
  // Full config language URI
  //
  StrCpyS (
    ThisList->InformationExchange->SendInformation.FullUri,
    MaxParentUriLength,
    ThisList->InformationExchange->SendInformation.ParentUri
    );
  if (StrLen (ThisList->InformationExchange->SendInformation.FullUri) != 0) {
    StrCatS (ThisList->InformationExchange->SendInformation.FullUri, MaxParentUriLength, NodeSeparatorStr);
  }

  StrCatS (ThisList->InformationExchange->SendInformation.FullUri, MaxParentUriLength, ThisList->InformationExchange->SendInformation.PropertyName);
  return EFI_SUCCESS;
}

/**
  Destroy the exchange information.

  @param[in]  ThisList  REDFISH_FEATURE_INTERNAL_DATA instance.

**/
EFI_STATUS
DestroryExchangeInformation (
  IN REDFISH_FEATURE_INTERNAL_DATA  *ThisList
  )
{
  if (ThisList->InformationExchange != NULL) {
    if (ThisList->InformationExchange->SendInformation.Type == InformationTypeCollectionMemberUri) {
      if (ThisList->InformationExchange->SendInformation.ParentUri != NULL) {
        FreePool (ThisList->InformationExchange->SendInformation.ParentUri);
        ThisList->InformationExchange->SendInformation.ParentUri = NULL;
      }

      if (ThisList->InformationExchange->SendInformation.PropertyName != NULL) {
        FreePool (ThisList->InformationExchange->SendInformation.PropertyName);
        ThisList->InformationExchange->SendInformation.PropertyName = NULL;
      }

      if (ThisList->InformationExchange->SendInformation.FullUri != NULL) {
        FreePool (ThisList->InformationExchange->SendInformation.FullUri);
        ThisList->InformationExchange->SendInformation.FullUri = NULL;
      }
    }

    if (ThisList->InformationExchange->ReturnedInformation.Type == InformationTypeCollectionMemberConfigLanguage) {
      DestroyConfiglanguageList (&ThisList->InformationExchange->ReturnedInformation.ConfigureLanguageList);
    }

    ThisList->InformationExchange->SendInformation.Type     = InformationTypeNone;
    ThisList->InformationExchange->ReturnedInformation.Type = InformationTypeNone;
  }

  return EFI_SUCCESS;
}

/**
  Startup child feature drivers and it's sibling feature drivers.

  @param[in]  ThisFeatureDriverList    This feature driver list.
  @param[in]  CurrentConfigLanguageUri The current parent configure language URI.
  @param[in]  StartupContext           Start up information

**/
VOID
StartUpFeatureDriver (
  IN REDFISH_FEATURE_INTERNAL_DATA    *ThisFeatureDriverList,
  IN EFI_STRING                       CurrentConfigLanguageUri OPTIONAL,
  IN REDFISH_FEATURE_STARTUP_CONTEXT  *StartupContext
  )
{
  EFI_STATUS                                   Status;
  UINTN                                        Index;
  REDFISH_FEATURE_INTERNAL_DATA                *ThisList;
  REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG_LIST  ConfigLangList;
  EFI_STRING                                   NextParentUri;

  if ((ThisFeatureDriverList == NULL) || (StartupContext == NULL)) {
    return;
  }

  NextParentUri = (EFI_STRING)AllocateZeroPool (MaxParentUriLength * sizeof (CHAR16));
  if (NextParentUri == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for parent configure language.\n", __FUNCTION__));
    return;
  }

  if (CurrentConfigLanguageUri != NULL) {
    StrCpyS (NextParentUri, MaxParentUriLength, CurrentConfigLanguageUri);
  }

  ThisList = ThisFeatureDriverList;
  while (TRUE) {
    if (ThisList->Callback != NULL) {
      ThisList->InformationExchange = mInformationExchange;
      Status                        = SetupExchangeInformationInfo (ThisList, NextParentUri);
      if (!EFI_ERROR (Status)) {
        Status = ThisList->Callback (
                             StartupContext->This,
                             StartupContext->Action,
                             ThisList->Context,
                             ThisList->InformationExchange
                             );
      }

      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: Callback to EDK2 Redfish feature driver fail: %s.\n", __FUNCTION__, ThisList->InformationExchange->SendInformation.FullUri));

        //
        // Report status code for Redfish failure
        //
        REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
          EFI_ERROR_CODE | EFI_ERROR_MAJOR,
          EFI_COMPUTING_UNIT_MANAGEABILITY | EFI_MANAGEABILITY_EC_REDFISH_COMMUNICATION_ERROR,
          REDFISH_INTERNAL_ERROR,
          sizeof (REDFISH_INTERNAL_ERROR)
          );
      }
    }

    if (!EFI_ERROR (Status) && (ThisList->Callback != NULL) && (ThisList->ChildList != NULL)) {
      //
      // Go through child list only when the parent node is managed by feature driver.
      //
      if (ThisList->Flags & REDFISH_FEATURE_INTERNAL_DATA_IS_COLLECTION) {
        //
        // The collection driver's callback is invoked. InformationTypeCollectionMemberConfigLanguage
        // should be returned in RESOURCE_INFORMATION_RETURNED.
        //
        if (ThisList->InformationExchange->ReturnedInformation.Type == InformationTypeCollectionMemberConfigLanguage) {
          //
          // Copy RESOURCE_INFORMATION_RETURNED then destroy the exchange information.
          //
          CopyConfiglanguageList (&ThisList->InformationExchange->ReturnedInformation.ConfigureLanguageList, &ConfigLangList);
          DestroryExchangeInformation (ThisList);
          //
          // Modify the collection instance according to the returned InformationTypeCollectionMemberConfigLanguage.
          //
          for (Index = 0; Index < ConfigLangList.Count; Index++) {
            StrCatS (NextParentUri, MaxParentUriLength, ThisList->NodeName);
            StrCatS (NextParentUri, MaxParentUriLength, NodeIsCollectionSymbol);
            SetResourceConfigLangMemberInstance (&NextParentUri, MaxParentUriLength, (REDFISH_FEATURE_ARRAY_TYPE_CONFIG_LANG *)&ConfigLangList.List[Index]);
            StartUpFeatureDriver (ThisList->ChildList, NextParentUri, StartupContext);
          }

          DestroyConfiglanguageList (&ConfigLangList);
        } else {
          DEBUG ((DEBUG_ERROR, "%a: No InformationTypeCollectionMemberConfigLanguage of %s returned.\n", __FUNCTION__, ThisList->InformationExchange->SendInformation.FullUri));
          DEBUG ((DEBUG_ERROR, "%a: Redfish service maybe not connected or the network has problems.\n", __FUNCTION__));

          //
          // Report status code for Redfish failure
          //
          REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
            EFI_ERROR_CODE | EFI_ERROR_MAJOR,
            EFI_COMPUTING_UNIT_MANAGEABILITY | EFI_MANAGEABILITY_EC_REDFISH_COMMUNICATION_ERROR,
            REDFISH_COMMUNICATION_ERROR,
            sizeof (REDFISH_COMMUNICATION_ERROR)
            );
          return;
        }
      } else {
        StrCatS (NextParentUri, MaxParentUriLength, NodeSeparatorStr);
        StrCatS (NextParentUri, MaxParentUriLength, ThisList->NodeName);
        StartUpFeatureDriver (ThisList->ChildList, NextParentUri, StartupContext);
      }

      //
      // Restore the parent configure language URI for this level.
      //
      if (CurrentConfigLanguageUri != NULL) {
        StrCpyS (NextParentUri, MaxParentUriLength, CurrentConfigLanguageUri);
      } else {
        NextParentUri[0] = 0;
      }
    } else {
      DestroryExchangeInformation (ThisList);
    }

    //
    // Check sibling Redfish feature driver.
    //
    if (ThisList->SiblingList == NULL) {
      break;
    }

    //
    // Go next sibling Redfish feature driver.
    //
    ThisList = ThisList->SiblingList;
  }

  if (NextParentUri != NULL) {
    FreePool (NextParentUri);
  }
}

/**
  Callback routine when mEdkIIRedfishFeatureDriverStartupEvent
  is signaled.

  @param[in]  Event                 Event whose notification function is being invoked.
  @param[in]  Context               The pointer to the notification function's context,
                                    which is implementation-dependent.

**/
VOID
EFIAPI
RedfishFeatureDriverStartup (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                       Status;
  REDFISH_FEATURE_STARTUP_CONTEXT  *StartupContext;
  UINT16                           RebootTimeout;
  EDKII_REDFISH_OVERRIDE_PROTOCOL  *RedfishOverride;

  RedfishOverride = NULL;
  StartupContext  = (REDFISH_FEATURE_STARTUP_CONTEXT *)Context;
  RebootTimeout   = PcdGet16 (PcdSystemRebootTimeout);

  //
  // Invoke EDK2 Redfish feature driver callback to start up
  // the Redfish operations.
  //
  if (ResourceUriNodeList == NULL) {
    return;
  }

  //
  // Initial dispatcher variables.
  //
  mInformationExchange = (RESOURCE_INFORMATION_EXCHANGE *)AllocateZeroPool (sizeof (RESOURCE_INFORMATION_EXCHANGE));
  if (mInformationExchange == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to allocate memory for exchange information.\n", __FUNCTION__));
    return;
  }

  //
  // Lower the TPL to TPL_APPLICATION so that
  // Redfish event and report status code can be
  // triggered
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  //
  // Reset PcdSystemRebootRequired flag
  //
  PcdSetBoolS (PcdSystemRebootRequired, FALSE);

  //
  // Signal event before doing provisioning
  //
  SignalReadyToProvisioningEvent ();

  //
  // Workaround: disable TLS Host Verify.
  // Solution is under discussion with EDK2 owner.
  //
  PcdSetBoolS (PcdHttpTlsHostVerifyDisabled, TRUE);

  //
  // Invoke task service callback before invoking other callback.
  //
  StartUpFeatureDriver (mTaskServiceNode, NULL, StartupContext);

  //
  // Invoke the callback by the hierarchy level
  //
  StartUpFeatureDriver (ResourceUriNodeList, NULL, StartupContext);

  //
  // Workaround: disable TLS Host Verify.
  // Solution is under discussion with EDK2 owner.
  //
  PcdSetBoolS (PcdHttpTlsHostVerifyDisabled, FALSE);

  //
  // Signal event after provisioning finished
  //
  SignalAfterProvisioningEvent ();

  //
  // If system configuration is changed, reboot system.
  //
  if (PcdGetBool (PcdSystemRebootRequired)) {
    //
    // Save ETag and Config Language list immediately.
    //
    SaveETagList ();
    RedfishSaveConfigList ();

    //
    // Report status code before reset system.
    //
    REPORT_STATUS_CODE_WITH_EXTENDED_DATA (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_RESET_SYSTEM,
      REDFISH_CONFIG_CHANGED,
      sizeof (REDFISH_CONFIG_CHANGED)
      );

    Print (L"System configuration is changed from RESTful interface. Reboot system in %d seconds...\n", RebootTimeout);
    gBS->Stall (RebootTimeout * 1000000U);

    //
    // Call override protocol to notify platform that Redfish is processed
    // and about to reboot system.
    //
    Status = gBS->LocateProtocol (
                    &gEdkiiRedfishOverrideProtocolGuid,
                    NULL,
                    (VOID **)&RedfishOverride
                    );
    if (!EFI_ERROR (Status)) {
      Status = RedfishOverride->NotifyPhase (RedfishOverride, EdkiiRedfishPhaseBeforeReboot);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: abort the reboot because NotifyPhase() returns failure: %r\n", __func__, Status));
        goto ON_EXIT;
      }
    }

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    CpuDeadLoop ();
  }

ON_EXIT:
  //
  // Restore to the TPL where this callback handler is called.
  //
  gBS->RaiseTPL (REDFISH_FEATURE_CORE_TPL);
}

/**
  Create new internal data instance.

  @param[in,out] PtrToNewInternalData  Pointer to receive new instance of
                                       REDFISH_FEATURE_INTERNAL_DATA.
  @param[in]     NodeName              Name of URI node.
  @param[in]     NodeIsCollection      TRUE means the node to add is the collection node.
                                       Otherwise it is a resource node.

  @retval EFI_SUCCESS              New entry is inserted successfully.
  @retval EFI_INVALID_PARAMETER    Improper given parameters.
  @retval EFI_OUT_OF_RESOURCES     Lack of memory for the internal data structure.

**/
EFI_STATUS
NewInternalInstance (
  IN OUT REDFISH_FEATURE_INTERNAL_DATA  **PtrToNewInternalData,
  IN EFI_STRING                         NodeName,
  IN BOOLEAN                            NodeIsCollection
  )
{
  REDFISH_FEATURE_INTERNAL_DATA  *NewInternalData;

  if ((PtrToNewInternalData == NULL) || (NodeName == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: Improper given parameters\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  *PtrToNewInternalData = NULL;
  NewInternalData       = AllocateZeroPool (sizeof (REDFISH_FEATURE_INTERNAL_DATA));
  if (NewInternalData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: No memory for REDFISH_FEATURE_INTERNAL_DATA\n", __FUNCTION__));
    return EFI_OUT_OF_RESOURCES;
  }

  NewInternalData->NodeName = AllocateZeroPool (StrSize (NodeName));
  StrnCpyS (NewInternalData->NodeName, StrSize (NodeName), (CONST CHAR16 *)NodeName, StrLen (NodeName));
  NewInternalData->SiblingList = NULL;
  NewInternalData->ChildList   = NULL;
  if (NodeIsCollection) {
    NewInternalData->Flags |= REDFISH_FEATURE_INTERNAL_DATA_IS_COLLECTION;
  }

  *PtrToNewInternalData = NewInternalData;
  return EFI_SUCCESS;
}

/**
  Insert the URI node into internal data structure

  @param[in]        HeadEntryToInsert  The head entry to start the searching.
  @param[in]        PrevisouEntry      Previous entry.
  @param[in]        NodeName           Name of URI node.
  @param[in]        NodeIsCollection   TRUE means the node to add is the collection node.
                                       Otherwise it is a resource node.
  @param[in, out]   NextNodeEntry      Pointer to receive the pointer of next head
                                       entry for inserting the follow up nodes.
                                       The returned LIST_ENTRY is the address of
                                       ChildList link list.
  @param[out]       MatchNodeEntry     The matched node entry.
  @retval EFI_SUCCESS              New entry is inserted successfully.
  @retval EFI_INVALID_PARAMETER    Improper given parameters.
  @retval EFI_OUT_OF_RESOURCES     Lack of memory for the internal data structure.

**/
EFI_STATUS
InsertRedfishFeatureUriNode (
  IN REDFISH_FEATURE_INTERNAL_DATA      *HeadEntryToInsert,
  IN REDFISH_FEATURE_INTERNAL_DATA      **PrevisouEntry,
  IN EFI_STRING                         NodeName,
  IN BOOLEAN                            NodeIsCollection,
  IN OUT REDFISH_FEATURE_INTERNAL_DATA  **NextNodeEntry,
  OUT REDFISH_FEATURE_INTERNAL_DATA     **MatchNodeEntry
  )
{
  EFI_STATUS                     Status;
  REDFISH_FEATURE_INTERNAL_DATA  *NewInternalData;
  REDFISH_FEATURE_INTERNAL_DATA  *ThisInternalData;
  REDFISH_FEATURE_INTERNAL_DATA  *SiblingList;

  *MatchNodeEntry = NULL;
  if (NodeName == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Node name is NULL.\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  if (NextNodeEntry == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: NextNodeEntry can't be NULL.\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  if (HeadEntryToInsert == NULL) {
    Status = NewInternalInstance (&NewInternalData, NodeName, NodeIsCollection);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((HeadEntryToInsert == NULL) && (ResourceUriNodeList == NULL)) {
      ResourceUriNodeList = NewInternalData;
    } else {
      (*PrevisouEntry)->ChildList = NewInternalData;
    }

    *PrevisouEntry = NewInternalData;
    *NextNodeEntry = NewInternalData->ChildList;
    return EFI_SUCCESS;
  }

  //
  // Go through sibling list to find the entry.
  //
  ThisInternalData = HeadEntryToInsert;
  SiblingList      = ThisInternalData->SiblingList;
  while (TRUE) {
    if (StrCmp ((CONST CHAR16 *)ThisInternalData->NodeName, (CONST CHAR16 *)NodeName) == 0) {
      *MatchNodeEntry = ThisInternalData;
      *NextNodeEntry  = ThisInternalData->ChildList;
      *PrevisouEntry  = ThisInternalData;
      return EFI_SUCCESS;
    }

    //
    // If sibing exist?
    //
    if (SiblingList == NULL) {
      Status = NewInternalInstance (&NewInternalData, NodeName, NodeIsCollection);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      ThisInternalData->SiblingList = NewInternalData;
      *PrevisouEntry                = NewInternalData;
      *NextNodeEntry                = NewInternalData->ChildList;
      return EFI_SUCCESS;
    }

    ThisInternalData = SiblingList;
    SiblingList      = ThisInternalData->SiblingList;
  }

  return EFI_SUCCESS;
}

/**
  The registration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]   FeatureManagedUri   The URI represents the hierarchy path of the Redfish
                                   resource in the entire Redfish data model that managed
                                   by Redfish feature driver . Each node in the hierarchy
                                   path is the property name defined in the schema of the
                                   resource.
                                   e.g. "ServiceRoot/" - Managed by ServiceRoot feature driver
                                        "ServiceRoot/Systems[]/" - Managed by ComputerSystemCollection feature driver
                                        "ServiceRoot/Systems[1]/" - Managed by ComputerSystem feature driver
                                        "ServiceRoot/Systems[2]/Bios/" - Managed by Bios feature driver
  @param[in]   Callback            Callback routine associated with this registration that
                                   provided by Redfish feature driver to execute the action
                                   on Redfish resource which is managed by this Redfish
                                   feature driver.
  @param[in]   Context             The context of the registering feature driver. The pointer
                                   to the context is delivered through callback function.
  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval EFI_INVALID_PARAMETER    Improper given parameters or fail to register
                                   feature driver.
  @retval EFI_OUT_OF_RESOURCES     Lack of memory for the internal data structure.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishFeatureRegister (
  IN EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN EFI_STRING                      FeatureManagedUri,
  IN REDFISH_FEATURE_CALLBACK        Callback,
  IN VOID                            *Context
  )
{
  CHAR16                         NodeName[MaxNodeNameLength];
  EFI_STATUS                     Status;
  UINTN                          Index;
  UINTN                          AnchorIndex;
  UINTN                          UriLength;
  BOOLEAN                        NewUri;
  REDFISH_FEATURE_INTERNAL_DATA  *ThisUriNode;
  REDFISH_FEATURE_INTERNAL_DATA  *PreUriNode;
  REDFISH_FEATURE_INTERNAL_DATA  *NewUriNode;
  REDFISH_FEATURE_INTERNAL_DATA  *MatchNodeEntry;
  BOOLEAN                        ItsCollection;

  if ((FeatureManagedUri == NULL) || (Callback == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a: The given parameter is invalid\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Task service is special service which will be launched before other feature
  // drivers.
  //
  if (StrCmp (REDFISH_TASK_SERVICE_URI, FeatureManagedUri) == 0) {
    if (mTaskServiceNode == NULL) {
      Status = NewInternalInstance (&mTaskServiceNode, REDFISH_TASK_SERVICE_URI, FALSE);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a: failed to register task service: %r\n", __func__, Status));
        return Status;
      }
    }

    mTaskServiceNode->Context  = Context;
    mTaskServiceNode->Callback = Callback;

    return EFI_SUCCESS;
  }

  //
  // Walk through URI which managed by this EDK2 Redfish feature driver.
  //
  UriLength   = StrLen (FeatureManagedUri) + 1; // Add one NULL for the last node.
  Index       = 0;
  AnchorIndex = 0;
  ThisUriNode = ResourceUriNodeList;
  PreUriNode  = ResourceUriNodeList;
  NewUri      = FALSE;
  while ((Index < UriLength)) {
    if ((Index - AnchorIndex + 1) >= MaxNodeNameLength) {
      // Increase one for the NULL terminator
      DEBUG ((DEBUG_ERROR, "%a: the length of node name is >= MaxNodeNameLength\n", __FUNCTION__));
      ASSERT (FALSE);
    }

    NodeName[Index - AnchorIndex] = *(FeatureManagedUri + Index);
    if ((NodeName[Index - AnchorIndex] == NodeSeperator) || (NodeName[Index - AnchorIndex] == UriSeperator) || (NodeName[Index - AnchorIndex] == (CHAR16)0)) {
      if (NodeName[Index - AnchorIndex] == UriSeperator) {
        NewUri = TRUE;
      }

      NodeName[Index - AnchorIndex] = 0;
      AnchorIndex                   = Index + 1;
      //
      // Insert node
      //
      if (StrLen (NodeName) != 0) {
        ItsCollection = FALSE;
        if (((Index + StrLen (NodeIsCollectionSymbol)) < UriLength) &&
            (*(FeatureManagedUri + Index + 1) == NodeIsCollectionLeftBracket) &&
            (*(FeatureManagedUri + Index + 2) == NodeIsCollectionRightBracket))
        {
          Index        += (StrLen (NodeIsCollectionSymbol));
          AnchorIndex  += (StrLen (NodeIsCollectionSymbol));
          ItsCollection = TRUE;
          if (*(FeatureManagedUri + Index) == UriSeperator) {
            NewUri = TRUE;
          }
        }

        Status = InsertRedfishFeatureUriNode (ThisUriNode, &PreUriNode, NodeName, ItsCollection, &NewUriNode, &MatchNodeEntry);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        ThisUriNode = NewUriNode;
      }

      if (NewUri || ((Index + 1) >= UriLength)) {
        //
        // Setup the callback and restart the searching for the
        // next URI.
        //
        if (MatchNodeEntry != NULL) {
          MatchNodeEntry->Callback = Callback;
          MatchNodeEntry->Context  = Context;
          MatchNodeEntry           = NULL;
        } else {
          PreUriNode->Callback = Callback;
          PreUriNode->Context  = Context;
        }

        NewUri      = FALSE;
        ThisUriNode = ResourceUriNodeList;
        Index++;
        continue;
      }
    }

    Index++;
  }

  if (ThisUriNode == NULL) {
    //
    // No URI node was created
    //
    DEBUG ((DEBUG_ERROR, "%a: No URI node is added\n", __FUNCTION__));
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  The unregistration function for the Redfish Feature driver.

  @param[in]   This                Pointer to EDKII_REDFISH_FEATURE_PROTOCOL instance.
  @param[in]   FeatureManagedUri   The URI represents the hierarchy path of the Redfish
                                   resource in the entire Redfish data model that managed
                                   by Redfish feature driver . Each node in the hierarchy
                                   path is the property name defined in the schema of the
                                   resource.
  @param[in]   Context             The context used for the previous feature driver
                                   registration.
  @retval EFI_SUCCESS              Redfish feature driver is registered successfully.
  @retval Others                   Some error happened.

**/
EFI_STATUS
EFIAPI
RedfishFeatureUnregister (
  IN     EDKII_REDFISH_FEATURE_PROTOCOL  *This,
  IN     EFI_STRING                      FeatureManagedUri,
  IN     VOID                            *Context
  )
{
  return EFI_UNSUPPORTED;
}

EDKII_REDFISH_FEATURE_PROTOCOL  mRedfishFeatureProtocol = {
  RedfishFeatureRegister,
  RedfishFeatureUnregister
};

/**
  Main entry for this driver.

  @param[in] ImageHandle     Image handle this driver.
  @param[in] SystemTable     Pointer to SystemTable.

  @retval EFI_SUCCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
RedfishFeatureCoreEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  EFI_GUID    *EventGuid;

  Handle              = NULL;
  ResourceUriNodeList = NULL;
  mTaskServiceNode    = NULL;
  EventGuid           = (EFI_GUID *)PcdGetPtr (PcdEdkIIRedfishFeatureDriverStartupEventGuid);

  ZeroMem ((VOID *)&mFeatureDriverStartupContext, sizeof (REDFISH_FEATURE_STARTUP_CONTEXT));
  mFeatureDriverStartupContext.This   = &mRedfishFeatureProtocol;
  mFeatureDriverStartupContext.Action = CallbackActionStartOperation;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  REDFISH_FEATURE_CORE_TPL,
                  RedfishFeatureDriverStartup,
                  (CONST VOID *)&mFeatureDriverStartupContext,
                  EventGuid,
                  &mEdkIIRedfishFeatureDriverStartupEvent
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install the RedfishCredentialProtocol onto Handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEdkIIRedfishFeatureProtocolGuid,
                  &mRedfishFeatureProtocol,
                  NULL
                  );
  return Status;
}
