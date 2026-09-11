$(BUILD_DIR)bootloader/start/_start.o: $(SRC_DIR)bootloader/start/_start.asm
$(BUILD_DIR)bootloader/start/boot.bin: $(SRC_DIR)bootloader/start/boot.asm


$(BUILD_DIR)bootloader/start.o:\
	$(BUILD_DIR)bootloader/start/_start.o\

	$(LDCMD32) $@ $^