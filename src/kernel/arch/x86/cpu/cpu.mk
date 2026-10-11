B_R = $(BUILD_DIR)kernel/arch/x86/cpu/
S_R = $(SRC_DIR)kernel/arch/x86/cpu/

$(B_R)idt.o: $(S_R)idt_exception.cpp
$(B_R)idt_asm.o: $(S_R)idt_asm.asm
$(B_R)gdt_asm.o: $(S_R)gdt_asm.asm
$(B_R)gdt.o: $(S_R)gdt.cpp
$(B_R)tss.o: $(S_R)tss.cpp
$(B_R)cpu.o: $(S_R)cpu.cpp
$(B_R)ldt.o: $(S_R)ldt.cpp

$(BUILD_DIR)kernel/arch/x86/cpu.o: \
	$(B_R)idt.o\
	$(B_R)idt_asm.o\
	$(B_R)gdt.o\
	$(B_R)gdt_asm.o\
	$(B_R)tss.o\
	$(B_R)cpu.o\
	$(B_R)ldt.o\

	$(LDCMD32) $@ $^
