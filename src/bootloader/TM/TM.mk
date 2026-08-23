$(BUILD_DIR)bootloader/TM/TM.o: $(SRC_DIR)bootloader/TM/TM.cpp
$(BUILD_DIR)bootloader/TM/test.o: $(SRC_DIR)bootloader/TM/test.asm

$(BUILD_DIR)bootloader/TM.o: \
	$(BUILD_DIR)bootloader/TM/TM.o\
	$(BUILD_DIR)bootloader/TM/test.o

	$(LDCMD32) $@ $^