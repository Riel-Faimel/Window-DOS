$(BUILD_DIR)bootloader/PDS/USB/usb.o: $(SRC_DIR)bootloader/PDS/USB/usb.cpp

$(BUILD_DIR)bootloader/PDS/usb.o:\
	$(BUILD_DIR)bootloader/PDS/USB/usb.o

	$(LDCMD32) $@ $^