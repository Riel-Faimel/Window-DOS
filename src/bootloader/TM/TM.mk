$(BUILD_DIR)bootloader/TM/TM.o: $(SRC_DIR)bootloader/TM/TM.cpp
$(BUILD_DIR)bootloader/TM/tss.o: $(SRC_DIR)bootloader/TM/tss.cpp

$(BUILD_DIR)bootloader/TM.o: \
	$(BUILD_DIR)bootloader/TM/TM.o\
	$(BUILD_DIR)bootloader/TM/tss.o

	$(LDCMD32) $@ $^