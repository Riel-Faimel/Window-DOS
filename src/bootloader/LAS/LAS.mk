$(BUILD_DIR)bootloader/LAS/LAS.o: $(SRC_DIR)bootloader/LAS/LAS.cpp

$(BUILD_DIR)bootloader/LAS.o:\
	$(BUILD_DIR)bootloader/LAS/LAS.o

	$(LDCMD32) $@ $^