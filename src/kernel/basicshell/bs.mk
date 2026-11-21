$(BUILD_DIR)kernel/basicshell/bs.o: $(SRC_DIR)kernel/basicshell/bs.cpp
$(BUILD_DIR)kernel/basicshell/shell_in.o: $(SRC_DIR)kernel/basicshell/shell_in.cpp
$(BUILD_DIR)kernel/basicshell/shell_vm.o: $(SRC_DIR)kernel/basicshell/shell_vm_exp.cpp
$(BUILD_DIR)kernel/basicshell/op.o: $(SRC_DIR)kernel/basicshell/op.cpp

OBJ += \
	$(BUILD_DIR)kernel/basicshell/bs.o \
	$(BUILD_DIR)kernel/basicshell/shell_in.o \
	$(BUILD_DIR)kernel/basicshell/shell_vm.o \
	$(BUILD_DIR)kernel/basicshell/op.o


