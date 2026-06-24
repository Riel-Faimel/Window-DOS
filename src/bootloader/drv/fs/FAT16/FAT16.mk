$(BUILD_DIR)bootloader/drv/fs/FAT16/FAT16.o: $(SRC_DIR)bootloader/drv/fs/FAT16/FAT16.cpp

$(BUILD_DIR)bootloader/drv/fs/FAT16.o:\
	$(BUILD_DIR)bootloader/drv/fs/FAT16/FAT16.o

	$(LDCMD32) $@ $^