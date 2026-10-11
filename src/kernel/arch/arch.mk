include $(SRC_DIR)kernel/arch/x86/x86.mk

$(BUILD_DIR)kernel/arch.o:\
	$(BUILD_DIR)kernel/arch/x86.o

	$(LDCMD) $@ $^