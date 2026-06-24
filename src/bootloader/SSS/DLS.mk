$(BUILD_DIR)bootloader/SSS/DLS.o: $(SRC_DIR)bootloader/SSS/DLS.cpp
$(BUILD_DIR)bootloader/SSS/PM.o: $(SRC_DIR)bootloader/SSS/PM.cpp

$(BUILD_DIR)bootloader/SSS.o:\
	$(BUILD_DIR)bootloader/SSS/DLS.o\
	$(BUILD_DIR)bootloader/SSS/PM.o

	$(LDCMD32) $@ $^