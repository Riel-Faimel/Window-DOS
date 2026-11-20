$(x86BR)cpu/idt.o: $(x86SR)cpu/idt_exp.cpp
$(x86BR)cpu/gdt.o: $(x86SR)cpu/gdt.cpp
$(x86BR)cpu/tss.o: $(x86SR)cpu/tss.cpp
$(x86BR)cpu/cpu.o: $(x86SR)cpu/cpu.cpp
$(x86BR)cpu/inpage.o: $(x86SR)cpu/inpage.cpp
$(x86BR)cpu/ldt.o: $(x86SR)cpu/ldt.cpp
$(x86BR)cpu/idt_asm.o: $(x86SR)cpu/idt_asm.asm
$(x86BR)cpu/gdt_asm.o: $(x86SR)cpu/gdt_asm.asm

OBJ += \
	$(x86BR)cpu/idt.o\
	$(x86BR)cpu/gdt.o\
	$(x86BR)cpu/tss.o\
	$(x86BR)cpu/cpu.o\
	$(x86BR)cpu/inpage.o\
	$(x86BR)cpu/ldt.o\
	$(x86BR)cpu/idt_asm.o\
	$(x86BR)cpu/gdt_asm.o
