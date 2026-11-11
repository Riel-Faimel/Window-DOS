$(BUILD_DIR)lib/module/changestack.o: $(SRC_DIR)lib/module/changestack.cppm
$(BUILD_DIR)lib/module/lib.o: $(SRC_DIR)lib/module/lib.cppm
$(BUILD_DIR)lib/module/string.o: $(SRC_DIR)lib/module/string.cpp
$(BUILD_DIR)lib/module/exception.o: $(SRC_DIR)lib/module/exception_exception.cppm

LIBOBJ += \
	$(BUILD_DIR)lib/module/changestack.o \
	$(BUILD_DIR)lib/module/lib.o\
	$(BUILD_DIR)lib/module/string.o\
	$(BUILD_DIR)lib/module/exception.o