$(x86BR)sche/context.o: $(x86SR)sche/context.cpp
$(x86BR)sche/context_asm.o: $(x86SR)sche/context_asm.asm

OBJ += \
	$(x86BR)sche/context.o\
	$(x86BR)sche/context_asm.o
