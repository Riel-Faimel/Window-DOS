$(BUILD_DIR)bootloader/mm/mm.o: $(SRC_DIR)bootloader/mm/mm.cpp
$(BUILD_DIR)bootloader/mm/gdt.o: $(SRC_DIR)bootloader/mm/gdt.cpp
$(BUILD_DIR)bootloader/mm/gdt_asm.o: $(SRC_DIR)bootloader/mm/gdt_asm.asm

$(BUILD_DIR)bootloader/mm.o: \
	$(BUILD_DIR)bootloader/mm/mm.o\
	$(BUILD_DIR)bootloader/mm/gdt.o\
	$(BUILD_DIR)bootloader/mm/gdt_asm.o

	$(LDCMD32) $@ $^