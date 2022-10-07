//
// Auto-generated file by Redfish Schema C Structure Generator.
// https://github.com/DMTF/Redfish-Schema-C-Struct-Generator.
//
//  (C) Copyright 2019-2021 Hewlett Packard Enterprise Development LP<BR>
//
// Copyright Notice:
// Copyright 2019-2021 Distributed Management Task Force, Inc. All rights reserved.
// License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/Redfish-JSON-C-Struct-Converter/blob/master/LICENSE.md
//

#ifndef RedfishRedfisherror_V1_0_1_CSTRUCT_H_
#define RedfishRedfisherror_V1_0_1_CSTRUCT_H_

#include "RedfishCsCommon.h"

typedef struct _Redfishredfisherror_V1_0_1_RedfishError_CS Redfishredfisherror_V1_0_1_RedfishError_CS;
typedef struct _Redfishredfisherror_V1_0_1_RedfishErrorContents_CS Redfishredfisherror_V1_0_1_RedfishErrorContents_CS;
//
// The properties that describe an error from a Redfish Service.
//
typedef struct _Redfishredfisherror_V1_0_1_RedfishErrorContents_CS {
    RedfishCS_Link    Message_ExtendedInfo;    // An array of messages
                                              // describing one or more error
                                              // messages.
    RedfishCS_char    *code;                  // A string indicating a specific
                                              // MessageId from a Message
                                              // Registry.
    RedfishCS_char    *message;               // A human-readable error message
                                              // corresponding to the message
                                              // in a Message Registry.
} Redfishredfisherror_V1_0_1_RedfishErrorContents_CS;

//
// The error payload from a Redfish Service.
//
typedef struct _Redfishredfisherror_V1_0_1_RedfishError_CS {
    RedfishCS_Header                                      Header;
    Redfishredfisherror_V1_0_1_RedfishErrorContents_CS    *error;    // The properties that describe
                                                                    // an error from a Redfish
                                                                    // Service.
} Redfishredfisherror_V1_0_1_RedfishError_CS;

RedfishCS_status
Json_redfisherror_V1_0_1_To_CS (char *JsonRawText, Redfishredfisherror_V1_0_1_RedfishError_CS **ReturnedCS);

RedfishCS_status
CS_To_redfisherror_V1_0_1_JSON (Redfishredfisherror_V1_0_1_RedfishError_CS *CSPtr, char **JsonText);

RedfishCS_status
Destroyredfisherror_V1_0_1_CS (Redfishredfisherror_V1_0_1_RedfishError_CS *CSPtr);

RedfishCS_status
Destroyredfisherror_V1_0_1_Json (RedfishCS_char *JsonText);

#endif
