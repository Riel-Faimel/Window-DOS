$(BUILD_DIR)bootloader/PDS/pds.o: $(SRC_DIR)bootloader/PDS/pds.cpp

$(BUILD_DIR)bootloader/PDS.o:\
	$(BUILD_DIR)bootloader/PDS/pds.o

	$(LDCMD32) $@ $^