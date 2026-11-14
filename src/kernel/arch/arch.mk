include $(SRC_DIR)kernel/arch/x86/x86.mk

$(BUILD_DIR)kernel/arch/arch.o: $(SRC_DIR)kernel/arch/arch.cpp

OBJ += \
	$(BUILD_DIR)kernel/arch/arch.o