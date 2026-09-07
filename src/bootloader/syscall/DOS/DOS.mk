$(BUILD_DIR)bootloader/syscall/DOS/DOScall.o: $(SRC_DIR)bootloader/syscall/DOS/DOScall.cpp
$(BUILD_DIR)bootloader/syscall/DOS/DOScall_asm.o: $(SRC_DIR)bootloader/syscall/DOS/DOScall_asm.asm

$(BUILD_DIR)bootloader/syscall/DOS.o:\
	$(BUILD_DIR)bootloader/syscall/DOS/DOScall.o\
	$(BUILD_DIR)bootloader/syscall/DOS/DOScall_asm.o

	$(LDCMD32) $@ $^