$(BUILD_DIR)lib/cppstdlib/stdio.o: $(SRC_DIR)lib/cppstdlib/stdio.cpp
$(BUILD_DIR)lib/cppstdlib/rtti.o: $(SRC_DIR)lib/cppstdlib/rtti.cpp

LIBOBJ += \
	$(BUILD_DIR)lib/cppstdlib/stdio.o \
	$(BUILD_DIR)lib/cppstdlib/rtti.o