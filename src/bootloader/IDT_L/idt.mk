$(BUILD_DIR)bootloader/IDT_L/idt.o: $(SRC_DIR)bootloader/IDT_L/idt.cpp
$(BUILD_DIR)bootloader/IDT_L/idt_asm.o: $(SRC_DIR)bootloader/IDT_L/idt_asm.asm

$(BUILD_DIR)bootloader/IDT_L.o: \
	$(BUILD_DIR)bootloader/IDT_L/idt.o\
	$(BUILD_DIR)bootloader/IDT_L/idt_asm.o

	$(LDCMD32) $@ $^
