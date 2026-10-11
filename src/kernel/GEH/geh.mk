$(BUILD_DIR)kernel/GEH/geh.o: $(SRC_DIR)kernel/GEH/geh.cpp

$(BUILD_DIR)kernel/GEH.o:\
	$(BUILD_DIR)kernel/GEH/geh.o

	$(LDCMD) $@ $^