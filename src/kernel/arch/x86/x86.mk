include $(SRC_DIR)kernel/arch/x86/cpu/cpu.mk
include $(SRC_DIR)kernel/arch/x86/start/start.mk
include $(SRC_DIR)kernel/arch/x86/sche/sche.mk

$(BUILD_DIR)kernel/arch/x86.o:\
	$(BUILD_DIR)kernel/arch/x86/cpu.o\
	$(BUILD_DIR)kernel/arch/x86/start.o\
	$(BUILD_DIR)kernel/arch/x86/sche.o\

#\

	$(LDCMD32) $@ $^