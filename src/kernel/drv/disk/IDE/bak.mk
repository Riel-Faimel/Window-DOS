$(BUILD_DIR)kernel/drv/disk/IDE/ide_bak.o: $(SRC_DIR)kernel/drv/disk/IDE/ide_bak.cpp
$(BUILD_DIR)kernel/drv/disk/IDE/ide_asm.o: $(SRC_DIR)kernel/drv/disk/IDE/ide_asm.asm


$(BUILD_DIR)kernel/drv/disk/IDE.o:\
	$(BUILD_DIR)kernel/drv/disk/IDE/ide_bak.o\
	$(BUILD_DIR)kernel/drv/disk/IDE/ide_asm.o\


	$(LDCMD) $@ $^