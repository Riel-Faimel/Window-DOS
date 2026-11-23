$(BUILD_DIR)lib/cppstdlib/stdio.o: $(SRC_DIR)lib/cppstdlib/stdio.cpp
$(BUILD_DIR)lib/cppstdlib/rtti.o: $(SRC_DIR)lib/cppstdlib/rtti.cpp
$(BUILD_DIR)lib/cppstdlib/string.o: $(SRC_DIR)lib/cppstdlib/string.cpp
$(BUILD_DIR)lib/cppstdlib/eh_frame.o: $(SRC_DIR)lib/cppstdlib/eh_frame_exp.cpp
$(BUILD_DIR)lib/cppstdlib/eh_arch.o: $(SRC_DIR)lib/cppstdlib/eh_arch_exp.cpp
$(BUILD_DIR)lib/cppstdlib/eh_cxa.o: $(SRC_DIR)lib/cppstdlib/eh_cxa_exp.cpp
$(BUILD_DIR)lib/cppstdlib/lsda.o: $(SRC_DIR)lib/cppstdlib/lsda.cpp


LIBOBJ += \
	$(BUILD_DIR)lib/cppstdlib/stdio.o \
	$(BUILD_DIR)lib/cppstdlib/rtti.o\
	$(BUILD_DIR)lib/cppstdlib/string.o\
	$(BUILD_DIR)lib/cppstdlib/eh_frame.o\
	$(BUILD_DIR)lib/cppstdlib/eh_arch.o\
	$(BUILD_DIR)lib/cppstdlib/eh_cxa.o\
	$(BUILD_DIR)lib/cppstdlib/lsda.o