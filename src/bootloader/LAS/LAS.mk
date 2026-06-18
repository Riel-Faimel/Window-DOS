$(BUILD_DIR)bootloader/LAS/LAS.o: $(SRC_DIR)bootloader/LAS/LAS.cpp
$(BUILD_DIR)bootloader/LAS/PM.o: $(SRC_DIR)bootloader/LAS/PM.cpp

$(BUILD_DIR)bootloader/LAS.o:\
	$(BUILD_DIR)bootloader/LAS/LAS.o\
	$(BUILD_DIR)bootloader/LAS/PM.o

	$(LDCMD32) $@ $^