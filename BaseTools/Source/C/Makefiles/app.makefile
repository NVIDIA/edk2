## @file
# Makefiles
#
# Copyright (c) 2007 - 2014, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.    The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

MAKEROOT ?= ../..

ifdef BASETOOLS_OUT_DIR
OUTPUT_DIRECTORY = $(BASETOOLS_OUT_DIR)/$(APPNAME)/
else
OUTPUT_DIRECTORY = ./
endif
OUT_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY), $(OBJECTS))

include $(MAKEROOT)/Makefiles/header.makefile

APPLICATION = $(MAKEROOT)/bin/$(APPNAME)

.PHONY:all
all: $(MAKEROOT)/bin $(APPLICATION) 

$(APPLICATION): $(OUT_OBJECTS)
	$(LINKER) -o $(APPLICATION) $(BUILD_LFLAGS) $(OUT_OBJECTS) -L$(MAKEROOT)/libs $(LIBS)

$(OUT_OBJECTS): $(MAKEROOT)/Include/Common/BuildVersion.h

include $(MAKEROOT)/Makefiles/footer.makefile

print-%  : ; @echo $* = $($*)