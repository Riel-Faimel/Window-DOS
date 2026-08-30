$(BUILD_DIR)bootloader/kmod/kmod.o: $(SRC_DIR)bootloader/kmod/kmod.cpp

$(BUILD_DIR)bootloader/kmod.o:\
	$(BUILD_DIR)bootloader/kmod/kmod.o

	$(LDCMD32) $@ $^