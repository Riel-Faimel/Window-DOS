$(BUILD_DIR)bootloader/mm/mm.o: $(SRC_DIR)bootloader/mm/mm.cpp
$(BUILD_DIR)bootloader/mm/mem.o: $(SRC_DIR)bootloader/mm/mem.cpp
$(BUILD_DIR)bootloader/mm/phy.o: $(SRC_DIR)bootloader/mm/phy.cpp

$(BUILD_DIR)bootloader/mm.o: \
	$(BUILD_DIR)bootloader/mm/mm.o\
	$(BUILD_DIR)bootloader/mm/mem.o\
	$(BUILD_DIR)bootloader/mm/phy.o

	$(LDCMD32) $@ $^