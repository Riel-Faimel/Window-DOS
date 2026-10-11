B_R = $(BUILD_DIR)kernel/arch/x86/sche/
S_R = $(SRC_DIR)kernel/arch/x86/sche/

$(B_R)ds.o: $(S_R)ds.cpp
$(B_R)ds_asm.o: $(S_R)ds_asm.asm

$(BUILD_DIR)kernel/arch/x86/sche.o:\
	$(B_R)ds.o\
	$(B_R)ds_asm.o\

	$(LDCMD) $@ $^