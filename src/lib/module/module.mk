$(BUILD_DIR)lib/module/changestack.o: $(SRC_DIR)lib/module/changestack.cppm
$(BUILD_DIR)lib/module/lib.o: $(SRC_DIR)lib/module/lib.cppm
$(BUILD_DIR)lib/module/string.o: $(SRC_DIR)lib/module/string.cpp
$(BUILD_DIR)lib/module/eh_frame.o: $(SRC_DIR)lib/module/eh_frame_exp.cpp
$(BUILD_DIR)lib/module/eh_arch.o: $(SRC_DIR)lib/module/eh_arch_exp.cpp
$(BUILD_DIR)lib/module/eh_cxa.o: $(SRC_DIR)lib/module/eh_cxa_exp.cpp
$(BUILD_DIR)lib/module/lsda.o: $(SRC_DIR)lib/module/lsda.cpp

LIBOBJ += \
	$(BUILD_DIR)lib/module/changestack.o \
	$(BUILD_DIR)lib/module/lib.o\
	$(BUILD_DIR)lib/module/string.o\
	$(BUILD_DIR)lib/module/eh_frame.o\
	$(BUILD_DIR)lib/module/eh_arch.o\
	$(BUILD_DIR)lib/module/eh_cxa.o\
	$(BUILD_DIR)lib/module/lsda.o