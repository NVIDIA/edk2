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

ifdef BASETOOLS_OUT_DIR
OUTPUT_DIRECTORY = $(BASETOOLS_OUT_DIR)/$(LIBNAME)/
else
OUTPUT_DIRECTORY = ./
endif
OUT_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY), $(OBJECTS))

include $(MAKEROOT)/Makefiles/header.makefile

LIBRARY = $(MAKEROOT)/libs/lib$(LIBNAME).a

all: $(MAKEROOT)/libs $(LIBRARY) 

include $(MAKEROOT)/Makefiles/footer.makefile
