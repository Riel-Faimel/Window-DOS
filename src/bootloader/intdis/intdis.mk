$(BUILD_DIR)bootloader/intdis/intdis.o: $(SRC_DIR)bootloader/intdis/intdis.cpp

$(BUILD_DIR)bootloader/intdis.o:\
	$(BUILD_DIR)bootloader/intdis/intdis.o\

	$(LDCMD32) $@ $^