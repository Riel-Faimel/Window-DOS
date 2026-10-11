$(BUILD_DIR)kernel/module/changestack.o: $(SRC_DIR)kernel/module/changestack.cppm
$(BUILD_DIR)kernel/module/lib.o: $(SRC_DIR)kernel/module/lib.cppm
$(BUILD_DIR)kernel/module/string.o: $(SRC_DIR)kernel/module/string.cpp
$(BUILD_DIR)kernel/module/exception.o: $(SRC_DIR)kernel/module/exception_exception.cppm
#include $(SRC_DIR)kernel/CenterShell/CenterShell.mk

module_32: $(BUILD_DIR)kernel/module.o

$(BUILD_DIR)kernel/module.o:\
	$(BUILD_DIR)kernel/module/changestack.o \
	$(BUILD_DIR)kernel/module/lib.o\
	$(BUILD_DIR)kernel/module/string.o\
	$(BUILD_DIR)kernel/module/exception.o\
	
#$(BUILD_DIR)kernel/CenterShell/cs_m.o

	$(LDCMD) $@ $^