$(BUILD_DIR)kernel/multiCE/multiCE.o: $(SRC_DIR)kernel/multiCE/multiCE.cpp

$(BUILD_DIR)kernel/multiCE.o: \
	$(BUILD_DIR)kernel/multiCE/multiCE.o\

	$(LDCMD32) $@ $^