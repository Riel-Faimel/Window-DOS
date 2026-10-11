$(BUILD_DIR)kernel/PDS/ISA/isa.o: $(SRC_DIR)kernel/PDS/ISA/isa.cpp

$(BUILD_DIR)kernel/PDS/isa.o:\
	$(BUILD_DIR)kernel/PDS/ISA/isa.o\

	$(LDCMD32) $@ $^