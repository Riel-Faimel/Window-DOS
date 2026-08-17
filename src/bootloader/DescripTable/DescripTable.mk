$(BUILD_DIR)bootloader/DescripTable/idt.o: $(SRC_DIR)bootloader/DescripTable/idt.cpp
$(BUILD_DIR)bootloader/DescripTable/idt_asm.o: $(SRC_DIR)bootloader/DescripTable/idt_asm.asm
$(BUILD_DIR)bootloader/DescripTable/gdt.o: $(SRC_DIR)bootloader/DescripTable/gdt.cpp
$(BUILD_DIR)bootloader/DescripTable/gdt_asm.o: $(SRC_DIR)bootloader/DescripTable/gdt_asm.asm
$(BUILD_DIR)bootloader/DescripTable/intdis.o: $(SRC_DIR)bootloader/DescripTable/intdis.cpp

$(BUILD_DIR)bootloader/DescripTable.o: \
	$(BUILD_DIR)bootloader/DescripTable/idt.o\
	$(BUILD_DIR)bootloader/DescripTable/idt_asm.o\
	$(BUILD_DIR)bootloader/DescripTable/gdt.o\
	$(BUILD_DIR)bootloader/DescripTable/gdt_asm.o\
	$(BUILD_DIR)bootloader/DescripTable/intdis.o

	$(LDCMD32) $@ $^
