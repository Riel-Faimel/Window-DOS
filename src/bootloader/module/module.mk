$(BUILD_DIR)bootloader/module/changestack.o: $(SRC_DIR)bootloader/module/changestack.cppm
$(BUILD_DIR)bootloader/module/lib.o: $(SRC_DIR)bootloader/module/lib.cppm
$(BUILD_DIR)bootloader/module/string.o: $(SRC_DIR)bootloader/module/string.cpp
#include $(SRC_DIR)bootloader/CenterShell/CenterShell.mk

module_32: $(BUILD_DIR)bootloader/module.o

$(BUILD_DIR)bootloader/module.o: \
	$(BUILD_DIR)bootloader/module/changestack.o \
	$(BUILD_DIR)bootloader/module/lib.o\
	$(BUILD_DIR)bootloader/module/string.o
	
#$(BUILD_DIR)bootloader/CenterShell/cs_m.o

	$(LDCMD32) $@ $^