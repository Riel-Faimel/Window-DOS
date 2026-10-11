$(BUILD_DIR)kernel/syscall/DOS/DOScall.o: $(SRC_DIR)kernel/syscall/DOS/DOScall.cpp
$(BUILD_DIR)kernel/syscall/DOS/DOScall_asm.o: $(SRC_DIR)kernel/syscall/DOS/DOScall_asm.asm

$(BUILD_DIR)kernel/syscall/DOS.o:\
	$(BUILD_DIR)kernel/syscall/DOS/DOScall.o\
	$(BUILD_DIR)kernel/syscall/DOS/DOScall_asm.o

	$(LDCMD) $@ $^