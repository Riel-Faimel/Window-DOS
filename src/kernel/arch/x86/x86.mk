x86BR = $(BUILD_DIR)kernel/arch/x86/
x86SR = $(SRC_DIR)kernel/arch/x86/

include $(SRC_DIR)kernel/arch/x86/cpu/cpu.mk
include $(SRC_DIR)kernel/arch/x86/start/start.mk
include $(SRC_DIR)kernel/arch/x86/sche/sche.mk
