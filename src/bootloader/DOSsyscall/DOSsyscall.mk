$(BUILD_DIR)bootloader/DOSsyscall/DOScall.o: $(SRC_DIR)bootloader/DOSsyscall/DOScall.cpp
$(BUILD_DIR)bootloader/DOSsyscall/DOScall_asm.o: $(SRC_DIR)bootloader/DOSsyscall/DOScall_asm.asm

$(BUILD_DIR)bootloader/DOSsyscall.o:\
	$(BUILD_DIR)bootloader/DOSsyscall/DOScall.o\
	$(BUILD_DIR)bootloader/DOSsyscall/DOScall_asm.o

	$(LDCMD32) $@ $^