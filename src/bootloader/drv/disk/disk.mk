include $(SRC_DIR)bootloader/drv/disk/IDE/ide.mk

$(BUILD_DIR)bootloader/drv/disk.o:\
	$(BUILD_DIR)bootloader/drv/disk/IDE.o

	$(LDCMD32) $@ $^