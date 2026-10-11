include $(SRC_DIR)kernel/drv/fs/FAT16/FAT16.mk

#$(BUILD_DIR)kernel/part-fs/las.o: $(SRC_DIR)kernel/part-fs/las.cpp\
include $(SRC_DIR)kernel/part-fs/MBR/MBR.mk\
include $(SRC_DIR)kernel/part-fs/minfs/minfs.mk

$(BUILD_DIR)kernel/drv/fs.o:\
	$(BUILD_DIR)kernel/drv/fs/FAT16/FAT16.o\

#	$(BUILD_DIR)kernel/part-fs/las.o\
	$(BUILD_DIR)kernel/part-fs/MBR.o\
	$(BUILD_DIR)kernel/part-fs/minfs.o\

	$(LDCMD) $@ $^