$(BUILD_DIR)bootloader/DescrpTable/idt.o: $(SRC_DIR)bootloader/DescrpTable/idt.cpp
$(BUILD_DIR)bootloader/DescrpTable/idt_asm.o: $(SRC_DIR)bootloader/DescrpTable/idt_asm.asm
$(BUILD_DIR)bootloader/DescrpTable/gdt.o: $(SRC_DIR)bootloader/DescrpTable/gdt.cpp
$(BUILD_DIR)bootloader/DescrpTable/gdt_asm.o: $(SRC_DIR)bootloader/DescrpTable/gdt_asm.asm

$(BUILD_DIR)bootloader/DescrpTable.o: \
	$(BUILD_DIR)bootloader/DescrpTable/idt.o\
	$(BUILD_DIR)bootloader/DescrpTable/idt_asm.o\
	$(BUILD_DIR)bootloader/DescrpTable/gdt.o\
	$(BUILD_DIR)bootloader/DescrpTable/gdt_asm.o

	$(LDCMD32) $@ $^
