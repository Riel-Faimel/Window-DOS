include $(SRC_DIR)bootloader/part-fs/FAT16/FAT16.mk
include $(SRC_DIR)bootloader/part-fs/MBR/MBR.mk

$(BUILD_DIR)bootloader/part-fs.o:\
	$(BUILD_DIR)bootloader/part-fs/FAT16/FAT16.o\
	$(BUILD_DIR)bootloader/part-fs/MBR/MBR.o

	$(LDCMD32) $@ $^