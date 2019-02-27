## @file
# GNU/Linux makefile for 'VfrCompile' module build.
#
# Copyright (c) 2008 - 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

MAKEROOT ?= ..

APPNAME = VfrCompile

ifdef BASETOOLS_OUT_DIR
OUTPUT_DIRECTORY = $(BASETOOLS_OUT_DIR)/$(APPNAME)/
else
OUTPUT_DIRECTORY = ./
endif
vpath %.h $(OUTPUT_DIRECTORY)
vpath %.cpp $(OUTPUT_DIRECTORY)
vpath %.dlg $(OUTPUT_DIRECTORY)

LIBS = -lCommon

TOOL_INCLUDE = -I Pccts/h -I $(OUTPUT_DIRECTORY)

#OBJECTS = VfrSyntax.o VfrServices.o DLGLexer.o EfiVfrParser.o ATokenBuffer.o DLexerBase.o AParser.o
OBJECTS = AParser.o DLexerBase.o ATokenBuffer.o EfiVfrParser.o VfrLexer.o VfrSyntax.o \
          VfrFormPkg.o VfrError.o VfrUtilityLib.o VfrCompiler.o
OUT_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY), $(OBJECTS))

VFR_CPPFLAGS = -DPCCTS_USE_NAMESPACE_STD $(BUILD_CPPFLAGS)

# keep BUILD_OPTFLAGS last
VFR_CXXFLAGS = $(BUILD_OPTFLAGS)

# keep EXTRA_LDFLAGS last
VFR_LFLAGS = $(EXTRA_LDFLAGS)

LINKER = $(BUILD_CXX)

EXTRA_CLEAN_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY), EfiVfrParser.cpp EfiVfrParser.h VfrParser.dlg VfrTokens.h VfrLexer.cpp VfrLexer.h VfrSyntax.cpp tokens.h)

MAKEROOT ?= ../..

include $(MAKEROOT)/Makefiles/header.makefile

APPLICATION = $(MAKEROOT)/bin/$(APPNAME)

.PHONY:all output_dir
all: $(OUTPUT_DIRECTORY) $(MAKEROOT)/bin $(APPLICATION) 

output_dir: $(OUTPUT_DIRECTORY)
$(OUTPUT_DIRECTORY):
	mkdir -p $@

$(APPLICATION): $(OUT_OBJECTS)
	$(LINKER) -o $(APPLICATION) $(VFR_LFLAGS) $(OUT_OBJECTS) -L$(MAKEROOT)/libs $(LIBS)

$(OUTPUT_DIRECTORY)VfrCompiler.o: ../Include/Common/BuildVersion.h

include $(MAKEROOT)/Makefiles/footer.makefile

$(OUTPUT_DIRECTORY)VfrSyntax.cpp $(OUTPUT_DIRECTORY)EfiVfrParser.cpp $(OUTPUT_DIRECTORY)EfiVfrParser.h $(OUTPUT_DIRECTORY)VfrParser.dlg $(OUTPUT_DIRECTORY)VfrTokens.h: Pccts/antlr/antlr VfrSyntax.g
	mkdir -p $(OUTPUT_DIRECTORY)
	Pccts/antlr/antlr -CC -e3 -ck 3 -k 2 -fl VfrParser.dlg -ft VfrTokens.h -o $(OUTPUT_DIRECTORY) VfrSyntax.g

VfrLexer.cpp VfrLexer.h: Pccts/dlg/dlg $(OUTPUT_DIRECTORY)VfrParser.dlg
	mkdir -p $(OUTPUT_DIRECTORY)
	Pccts/dlg/dlg -C2 -i -CC -cl VfrLexer -o . $(OUTPUT_DIRECTORY)VfrParser.dlg

Pccts/antlr/antlr:
	BIN_DIR='.' $(MAKE) -C Pccts/antlr

Pccts/dlg/dlg:
	BIN_DIR='.' $(MAKE) -C Pccts/dlg

$(OUTPUT_DIRECTORY)ATokenBuffer.o: Pccts/h/ATokenBuffer.cpp
	$(BUILD_CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $? -o $@

$(OUTPUT_DIRECTORY)DLexerBase.o: Pccts/h/DLexerBase.cpp
	$(BUILD_CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $? -o $@

$(OUTPUT_DIRECTORY)AParser.o: Pccts/h/AParser.cpp
	$(BUILD_CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $? -o $@

$(OUTPUT_DIRECTORY)VfrSyntax.o: VfrSyntax.cpp
	$(BUILD_CXX) -c $(VFR_CPPFLAGS) $(INC) $(VFR_CXXFLAGS) $? -o $@
	
clean: localClean

localClean:
	BIN_DIR='.' $(MAKE) -C Pccts/antlr clean
	BIN_DIR='.' $(MAKE) -C Pccts/dlg clean
	rm -f $(EXTRA_CLEAN_OBJECTS)

