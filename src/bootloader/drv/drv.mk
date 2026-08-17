include $(SRC_DIR)bootloader/drv/screen/screen.mk
include $(SRC_DIR)bootloader/drv/disk/disk.mk
include $(SRC_DIR)bootloader/drv/fs/fs.mk
#include $(SRC_DIR)bootloader/drv/keyboard/keyboard.mk

$(BUILD_DIR)bootloader/drv.o: \
	$(BUILD_DIR)bootloader/drv/screen.o\
	$(BUILD_DIR)bootloader/drv/disk.o\
	$(BUILD_DIR)bootloader/drv/fs.o\

#$(BUILD_DIR)bootloader/drv/keyboard.o\

	$(LDCMD32) $@ $^