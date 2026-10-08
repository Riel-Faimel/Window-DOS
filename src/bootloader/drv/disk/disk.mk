include $(SRC_DIR)bootloader/drv/disk/IDE/ide.mk
include $(SRC_DIR)bootloader/drv/disk/AHCI/ahci.mk

$(BUILD_DIR)bootloader/drv/disk.o:\
	$(BUILD_DIR)bootloader/drv/disk/IDE.o\
	$(BUILD_DIR)bootloader/drv/disk/AHCI.o\
	
#\

	$(LDCMD32) $@ $^