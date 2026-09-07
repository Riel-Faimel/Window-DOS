include $(SRC_DIR)bootloader/syscall/DOS/DOS.mk
include $(SRC_DIR)bootloader/syscall/re/re.mk

$(BUILD_DIR)bootloader/syscall.o:\
	$(BUILD_DIR)bootloader/syscall/DOS.o\
	
#$(BUILD_DIR)bootloader/syscall/re.o\

	$(LDCMD32) $@ $^