include $(SRC_DIR)kernel/syscall/DOS/DOS.mk
include $(SRC_DIR)kernel/syscall/re/re.mk

$(BUILD_DIR)kernel/syscall.o:\
	
#\
	$(BUILD_DIR)kernel/syscall/re.o\
	$(BUILD_DIR)kernel/syscall/DOS.o\

	$(LDCMD32) $@ $^