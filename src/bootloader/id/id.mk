$(BUILD_DIR)bootloader/id/id.o: $(SRC_DIR)bootloader/id/id.cpp

$(BUILD_DIR)bootloader/id.o:\
	$(BUILD_DIR)bootloader/id/id.o\

	$(LDCMD32) $@ $^