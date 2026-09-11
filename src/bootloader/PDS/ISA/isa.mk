$(BUILD_DIR)bootloader/PDS/ISA/isa.o: $(SRC_DIR)bootloader/PDS/ISA/isa.cpp

$(BUILD_DIR)bootloader/PDS/isa.o:\
	$(BUILD_DIR)bootloader/PDS/ISA/isa.o\

	$(LDCMD32) $@ $^