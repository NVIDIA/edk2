## @file
# GNU/Linux makefile for 'VfrCompile' module build.
#
# Copyright (c) 2008 - 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

MAKEROOT ?= ..

APPNAME = VfrCompile

BUILD_DIR = Build

LIBS = -lCommon

TOOL_INCLUDE = -I Pccts/h -I $(BUILD_DIR)

OBJECTS = $(BUILD_DIR)/AParser.o $(BUILD_DIR)/DLexerBase.o $(BUILD_DIR)/ATokenBuffer.o $(BUILD_DIR)/EfiVfrParser.o $(BUILD_DIR)/VfrLexer.o $(BUILD_DIR)/VfrSyntax.o \
          $(BUILD_DIR)/VfrFormPkg.o $(BUILD_DIR)/VfrError.o $(BUILD_DIR)/VfrUtilityLib.o $(BUILD_DIR)/VfrCompiler.o
CLANG := $(findstring clang,$(shell $(CC) --version))
ifneq ($(CLANG),)
VFR_CPPFLAGS = -Wno-deprecated-register -std=c++14 -DPCCTS_USE_NAMESPACE_STD $(CPPFLAGS)
else
VFR_CPPFLAGS = -DPCCTS_USE_NAMESPACE_STD $(CPPFLAGS)
endif
# keep BUILD_OPTFLAGS last
VFR_CXXFLAGS = $(BUILD_OPTFLAGS)

# keep EXTRA_LDFLAGS last
VFR_LFLAGS = $(EXTRA_LDFLAGS)

LINKER = $(CXX)

EXTRA_CLEAN_OBJECTS = $(BUILD_DIR)/EfiVfrParser.cpp $(BUILD_DIR)/EfiVfrParser.h $(BUILD_DIR)/VfrParser.dlg $(BUILD_DIR)/VfrTokens.h $(BUILD_DIR)/VfrLexer.cpp $(BUILD_DIR)/VfrLexer.h $(BUILD_DIR)/VfrSyntax.cpp $(BUILD_DIR)/tokens.h

MAKEROOT ?= ../..

include $(MAKEROOT)/Makefiles/header.makefile

APPLICATION = $(MAKEROOT)/bin/$(APPNAME)

BIN_DIR=.
export BIN_DIR

.PHONY:all
all: $(MAKEROOT)/bin $(BUILD_DIR) $(APPLICATION)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APPLICATION): $(OBJECTS)
	$(LINKER) -o $(APPLICATION) $(VFR_LFLAGS) $(OBJECTS) -L$(MAKEROOT)/libs $(LIBS)
ifeq (Windows, $(findstring Windows,$(MAKE_HOST)))
	$(CP) $(APPLICATION).exe $(BIN_PATH)
endif

# Special rule for VfrCompiler.o with additional dependency
$(BUILD_DIR)/VfrCompiler.o: ../Include/Common/BuildVersion.h

# Pattern rules for compiling cpp files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: Pccts/h/%.cpp | $(BUILD_DIR)
	$(CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $< -o $@

include $(MAKEROOT)/Makefiles/footer.makefile

# Use VfrTokens.h as a stamp file to avoid running multiple times
$(BUILD_DIR)/VfrSyntax.cpp $(BUILD_DIR)/EfiVfrParser.cpp $(BUILD_DIR)/EfiVfrParser.h $(BUILD_DIR)/VfrParser.dlg: $(BUILD_DIR)/VfrTokens.h
$(BUILD_DIR)/VfrTokens.h: Pccts/antlr/antlr VfrSyntax.g | $(BUILD_DIR)
	Pccts/antlr/antlr -CC -e3 -ck 3 -k 2 -fl $(BUILD_DIR)/VfrParser.dlg -ft $(BUILD_DIR)/VfrTokens.h -o $(BUILD_DIR) VfrSyntax.g

# Use VfrLexer.h as a stamp file to avoid running multiple times
$(BUILD_DIR)/VfrSyntax.cpp $(BUILD_DIR)/VfrLexer.cpp: $(BUILD_DIR)/VfrLexer.h
$(BUILD_DIR)/VfrLexer.h: Pccts/dlg/dlg $(BUILD_DIR)/VfrParser.dlg | $(BUILD_DIR)
	Pccts/dlg/dlg -C2 -i -CC -cl VfrLexer -o $(BUILD_DIR) $(BUILD_DIR)/VfrParser.dlg

Pccts/antlr/antlr:
	$(MAKE) -C Pccts/antlr

Pccts/dlg/dlg:
	$(MAKE) -C Pccts/dlg

clean: localClean

localClean:
	$(MAKE) -C Pccts/antlr clean
	$(MAKE) -C Pccts/dlg clean
	$(RM) $(EXTRA_CLEAN_OBJECTS)
	$(RM) $(BUILD_DIR)
ifeq (Windows, $(findstring Windows,$(MAKE_HOST)))
	$(RM) $(BIN_PATH)/$(APPNAME).exe
endif
