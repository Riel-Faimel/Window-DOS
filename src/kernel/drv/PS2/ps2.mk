$(BUILD_DIR)kernel/drv/ps2/ps2.o: $(SRC_DIR)kernel/drv/ps2/ps2.cpp
$(BUILD_DIR)kernel/drv/ps2/ps2_asm.o: $(SRC_DIR)kernel/drv/ps2/ps2_asm.asm
$(BUILD_DIR)kernel/drv/ps2/ps2_c.o: $(SRC_DIR)kernel/drv/ps2/ps2_c.c

OBJ += \
	$(BUILD_DIR)kernel/drv/ps2/ps2.o\
	$(BUILD_DIR)kernel/drv/ps2/ps2_asm.o\
	$(BUILD_DIR)kernel/drv/ps2/ps2_c.o