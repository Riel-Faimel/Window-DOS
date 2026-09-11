$(BUILD_DIR)bootloader/drv/disk/IDE/ide.o: $(SRC_DIR)bootloader/drv/disk/IDE/ide.cpp
$(BUILD_DIR)bootloader/drv/disk/IDE/ide_asm.o: $(SRC_DIR)bootloader/drv/disk/IDE/ide_asm.asm


$(BUILD_DIR)bootloader/drv/disk/IDE.o:\
	$(BUILD_DIR)bootloader/drv/disk/IDE/ide.o\
	$(BUILD_DIR)bootloader/drv/disk/IDE/ide_asm.o\


	$(LDCMD32) $@ $^