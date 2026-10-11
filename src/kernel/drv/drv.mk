include $(SRC_DIR)kernel/drv/screen/screen.mk
include $(SRC_DIR)kernel/drv/disk/disk.mk
include $(SRC_DIR)kernel/drv/fs/fs.mk
#include $(SRC_DIR)kernel/drv/keyboard/keyboard.mk

$(BUILD_DIR)kernel/drv/kmod.o: $(SRC_DIR)kernel/drv/kmod.cpp

$(BUILD_DIR)kernel/drv.o: \
	$(BUILD_DIR)kernel/drv/screen.o\
	$(BUILD_DIR)kernel/drv/fs.o\
	$(BUILD_DIR)kernel/drv/kmod.o\
	$(BUILD_DIR)kernel/drv/disk.o\

#$(BUILD_DIR)kernel/drv/keyboard.o\

	$(LDCMD) $@ $^