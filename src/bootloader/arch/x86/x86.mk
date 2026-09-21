$(BUILD_DIR)bootloader/arch/x86/idt.o: $(SRC_DIR)bootloader/arch/x86/idt_exception.cpp
$(BUILD_DIR)bootloader/arch/x86/idt_asm.o: $(SRC_DIR)bootloader/arch/x86/idt_asm.asm
$(BUILD_DIR)bootloader/arch/x86/gdt.o: $(SRC_DIR)bootloader/arch/x86/gdt.cpp
$(BUILD_DIR)bootloader/arch/x86/gdt_asm.o: $(SRC_DIR)bootloader/arch/x86/gdt_asm.asm
$(BUILD_DIR)bootloader/arch/x86/tss.o: $(SRC_DIR)bootloader/arch/x86/tss.cpp
$(BUILD_DIR)bootloader/arch/x86/cpu.o: $(SRC_DIR)bootloader/arch/x86/cpu.cpp
$(BUILD_DIR)bootloader/arch/x86/ldt.o: $(SRC_DIR)bootloader/arch/x86/ldt.cpp

$(BUILD_DIR)bootloader/arch/x86.o: \
	$(BUILD_DIR)bootloader/arch/x86/idt.o\
	$(BUILD_DIR)bootloader/arch/x86/idt_asm.o\
	$(BUILD_DIR)bootloader/arch/x86/gdt.o\
	$(BUILD_DIR)bootloader/arch/x86/gdt_asm.o\
	$(BUILD_DIR)bootloader/arch/x86/tss.o\
	$(BUILD_DIR)bootloader/arch/x86/cpu.o\
	$(BUILD_DIR)bootloader/arch/x86/ldt.o\

	$(LDCMD32) $@ $^
