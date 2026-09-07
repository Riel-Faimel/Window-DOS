include $(SRC_DIR)bootloader/arch/x86/x86.mk

$(BUILD_DIR)bootloader/arch.o:\
	$(BUILD_DIR)bootloader/arch/x86.o

	$(LDCMD32) $@ $^