include $(SRC_DIR)bootloader/drv/fs/FAT16/FAT16.mk

#$(BUILD_DIR)bootloader/part-fs/las.o: $(SRC_DIR)bootloader/part-fs/las.cpp\
include $(SRC_DIR)bootloader/part-fs/MBR/MBR.mk\
include $(SRC_DIR)bootloader/part-fs/minfs/minfs.mk

$(BUILD_DIR)bootloader/drv/fs.o:\
	$(BUILD_DIR)bootloader/drv/fs/FAT16/FAT16.o\

#	$(BUILD_DIR)bootloader/part-fs/las.o\
	$(BUILD_DIR)bootloader/part-fs/MBR.o\
	$(BUILD_DIR)bootloader/part-fs/minfs.o\

	$(LDCMD32) $@ $^