$(BUILD_DIR)kernel/drv/fs/FAT16/FAT16.o: $(SRC_DIR)kernel/drv/fs/FAT16/FAT16.cpp

$(BUILD_DIR)kernel/drv/fs/FAT16.o:\
	$(BUILD_DIR)kernel/drv/fs/FAT16/FAT16.o

	$(LDCMD) $@ $^