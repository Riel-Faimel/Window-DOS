#include $(SRC_DIR)bootloader/part-fs/FAT16/FAT16.mk
include $(SRC_DIR)bootloader/part-fs/minfs/minfs.mk
include $(SRC_DIR)bootloader/part-fs/MBR/MBR.mk

#$(BUILD_DIR)bootloader/part-fs/las.o: $(SRC_DIR)bootloader/part-fs/las.cpp

$(BUILD_DIR)bootloader/part-fs.o:\
	$(BUILD_DIR)bootloader/part-fs/MBR.o\
	$(BUILD_DIR)bootloader/part-fs/minfs.o
	
#$(BUILD_DIR)bootloader/part-fs/FAT16/FAT16.o\
	$(BUILD_DIR)bootloader/part-fs/las.o\

	$(LDCMD32) $@ $^