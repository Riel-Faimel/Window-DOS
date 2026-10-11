$(BUILD_DIR)kernel/PDS/USB/usb.o: $(SRC_DIR)kernel/PDS/USB/usb.cpp

$(BUILD_DIR)kernel/PDS/usb.o:\
	$(BUILD_DIR)kernel/PDS/USB/usb.o

	$(LDCMD32) $@ $^