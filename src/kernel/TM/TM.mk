$(BUILD_DIR)kernel/TM/TM.o: $(SRC_DIR)kernel/TM/TM.cpp

$(BUILD_DIR)kernel/TM.o: \
	$(BUILD_DIR)kernel/TM/TM.o

	$(LDCMD32) $@ $^