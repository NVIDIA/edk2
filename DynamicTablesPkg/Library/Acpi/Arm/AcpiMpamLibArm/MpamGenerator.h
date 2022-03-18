/** @file
  Header file for the dynamic MPAM generator

  Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
  Copyright (c) 2022, ARM Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Reference(s):
  - ACPI 6.4 Specification, January 2021
  - ARM Architecture Reference Manual ARMv8

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
**/

#ifndef MPAM_GENERATOR_H_
#define MPAM_GENERATOR_H_

#pragma pack(1)

/** A structure that describes the Node indexer
    used for indexing the MPAM MSC nodes.
*/
typedef struct MpamNodeIndexer {
  /// Index token for the Node
  CM_OBJECT_TOKEN    Token;
  /// Pointer to the node
  VOID               *Object;
  /// Node offset from the start of the MPAM table
  UINT32             Offset;
} MPAM_NODE_INDEXER;

typedef struct AcpiMpamGenerator {
  /// ACPI Table generator header
  ACPI_TABLE_GENERATOR    Header;
  /// MPAM structure count
  UINT32                  MpamNodeCount;
  /// Count of Msc Nodes
  UINT32                  MscNodeCount;
  /// List of indexed CM objects for MPAM generation
  MPAM_NODE_INDEXER       *NodeIndexer;
} ACPI_MPAM_GENERATOR;

#pragma pack()

#endif // MPAM_GENERATOR_H_
