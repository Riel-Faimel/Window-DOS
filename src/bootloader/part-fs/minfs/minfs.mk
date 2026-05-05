$(BUILD_DIR)bootloader/part-fs/minfs/minfs.o: $(SRC_DIR)bootloader/part-fs/minfs/minfs.cpp

$(BUILD_DIR)bootloader/part-fs/minfs.o:\
	$(BUILD_DIR)bootloader/part-fs/minfs/minfs.o

	$(LDCMD32) $@ $^