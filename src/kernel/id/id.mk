$(BUILD_DIR)kernel/id/id.o: $(SRC_DIR)kernel/id/id.cpp

$(BUILD_DIR)kernel/id.o:\
	$(BUILD_DIR)kernel/id/id.o\

	$(LDCMD) $@ $^