$(BUILD_DIR)bootloader/GEH/geh.o: $(SRC_DIR)bootloader/GEH/geh.cpp

$(BUILD_DIR)bootloader/GEH.o:\
	$(BUILD_DIR)bootloader/GEH/geh.o

	$(LDCMD32) $@ $^