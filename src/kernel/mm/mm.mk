$(BUILD_DIR)kernel/mm/phy.o: $(SRC_DIR)kernel/mm/phy.cpp
$(BUILD_DIR)kernel/mm/mm.o: $(SRC_DIR)kernel/mm/mm.cpp
$(BUILD_DIR)kernel/mm/mem.o: $(SRC_DIR)kernel/mm/mem.cpp

$(BUILD_DIR)kernel/mm.o: \
	$(BUILD_DIR)kernel/mm/phy.o\
	$(BUILD_DIR)kernel/mm/mm.o\
	$(BUILD_DIR)kernel/mm/mem.o\

	$(LDCMD) $@ $^