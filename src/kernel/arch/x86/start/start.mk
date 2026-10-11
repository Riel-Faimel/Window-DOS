B_R = $(BUILD_DIR)kernel/arch/x86/start/
S_R = $(SRC_DIR)kernel/arch/x86/start/

$(B_R)_start.o: $(S_R)_start.asm

$(BUILD_DIR)kernel/arch/x86/start.o:\
	$(B_R)_start.o\

	$(LDCMD) $@ $^