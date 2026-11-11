$(BUILD_DIR)kernel/drv/disk/IDE/ide.o: $(SRC_DIR)kernel/drv/disk/IDE/ide.cpp
$(BUILD_DIR)kernel/drv/disk/IDE/ide_asm.o: $(SRC_DIR)kernel/drv/disk/IDE/ide_asm.asm


OBJ += \
	$(BUILD_DIR)kernel/drv/disk/IDE/ide.o\
	$(BUILD_DIR)kernel/drv/disk/IDE/ide_asm.o