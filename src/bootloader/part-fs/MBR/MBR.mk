$(BUILD_DIR)bootloader/part-fs/MBR/MBR.o: $(SRC_DIR)bootloader/part-fs/MBR/MBR.cpp

$(BUILD_DIR)bootloader/part-fs/MBR.o:\
	$(BUILD_DIR)bootloader/part-fs/MBR/MBR.o

	$(LDCMD32) $@ $^