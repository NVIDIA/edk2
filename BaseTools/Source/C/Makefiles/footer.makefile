## @file
# Makefile
#
# Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.    The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

DEPFILES = $(OUT_OBJECTS:%.o=%.d)

$(MAKEROOT)/libs-$(HOST_ARCH):
	mkdir -p $(MAKEROOT)/libs-$(HOST_ARCH)

.PHONY: install
install: $(MAKEROOT)/libs-$(HOST_ARCH) $(LIBRARY)
	cp $(LIBRARY) $(MAKEROOT)/libs-$(HOST_ARCH)

$(LIBRARY): $(OUT_OBJECTS)
	$(BUILD_AR) crs $@ $^

$(OUTPUT_DIRECTORY)%.o : %.c
	mkdir -p $(dir $@)
	$(BUILD_CC)  -c $(BUILD_CPPFLAGS) $(BUILD_CFLAGS) $< -o $@

$(OUTPUT_DIRECTORY)%.o : %.cpp
	$(BUILD_CXX) -c $(BUILD_CPPFLAGS) $(BUILD_CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f $(OUT_OBJECTS) $(LIBRARY) $(DEPFILES)

-include $(DEPFILES)
