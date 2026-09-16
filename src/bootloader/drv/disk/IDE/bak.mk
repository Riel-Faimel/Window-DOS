$(BUILD_DIR)bootloader/drv/disk/IDE/bak.o: $(SRC_DIR)bootloader/drv/disk/IDE/bak.cpp
$(BUILD_DIR)bootloader/drv/disk/IDE/ide_asm.o: $(SRC_DIR)bootloader/drv/disk/IDE/ide_asm.asm


$(BUILD_DIR)bootloader/drv/disk/IDE.o:\
	$(BUILD_DIR)bootloader/drv/disk/IDE/bak.o\
	$(BUILD_DIR)bootloader/drv/disk/IDE/ide_asm.o\


	$(LDCMD32) $@ $^