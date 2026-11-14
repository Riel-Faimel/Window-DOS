$(BUILD_DIR)kernel/drv/syscall/DOS/DOScall.o: $(SRC_DIR)kernel/drv/syscall/DOS/DOScall.cpp
$(BUILD_DIR)kernel/drv/syscall/DOS/DOScall_asm.o: $(SRC_DIR)kernel/drv/syscall/DOS/DOScall_asm.asm

OBJ += \
	$(BUILD_DIR)kernel/drv/syscall/DOS/DOScall.o\
	$(BUILD_DIR)kernel/drv/syscall/DOS/DOScall_asm.o