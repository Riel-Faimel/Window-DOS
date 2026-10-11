$(BUILD_DIR)kernel/CDM/DLS.o: $(SRC_DIR)kernel/CDM/DLS.cpp
$(BUILD_DIR)kernel/CDM/PM.o: $(SRC_DIR)kernel/CDM/PM.cpp

$(BUILD_DIR)kernel/SSS.o:\
	$(BUILD_DIR)kernel/CDM/DLS.o\
	$(BUILD_DIR)kernel/CDM/PM.o

	$(LDCMD32) $@ $^