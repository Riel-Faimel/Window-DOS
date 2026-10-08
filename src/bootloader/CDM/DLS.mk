$(BUILD_DIR)bootloader/CDM/DLS.o: $(SRC_DIR)bootloader/CDM/DLS.cpp
$(BUILD_DIR)bootloader/CDM/PM.o: $(SRC_DIR)bootloader/CDM/PM.cpp

$(BUILD_DIR)bootloader/SSS.o:\
	$(BUILD_DIR)bootloader/CDM/DLS.o\
	$(BUILD_DIR)bootloader/CDM/PM.o

	$(LDCMD32) $@ $^