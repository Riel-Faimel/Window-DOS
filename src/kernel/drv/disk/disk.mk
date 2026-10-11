include $(SRC_DIR)kernel/drv/disk/IDE/ide.mk
include $(SRC_DIR)kernel/drv/disk/AHCI/ahci.mk

$(BUILD_DIR)kernel/drv/disk.o:\
	$(BUILD_DIR)kernel/drv/disk/IDE.o\
	$(BUILD_DIR)kernel/drv/disk/AHCI.o\
	
#\

	$(LDCMD) $@ $^