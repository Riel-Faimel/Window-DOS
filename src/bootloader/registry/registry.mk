$(BUILD_DIR)bootloader/registry/registry.o: $(SRC_DIR)bootloader/registry/registry.cpp

$(BUILD_DIR)bootloader/registry.o:\
	$(BUILD_DIR)bootloader/registry/registry.o

	$(LDCMD32) $@ $^