$(BUILD_DIR)kernel/drv/keyboard/keyboard.o: $(SRC_DIR)kernel/drv/keyboard/keyboard.cpp
$(BUILD_DIR)kernel/drv/keyboard/keyboard_asm.o: $(SRC_DIR)kernel/drv/keyboard/keyboard_asm.asm
$(BUILD_DIR)kernel/drv/keyboard/keyboard_c.o: $(SRC_DIR)kernel/drv/keyboard/keyboard_c.c

$(BUILD_DIR)kernel/drv/keyboard.o: \
	$(BUILD_DIR)kernel/drv/keyboard/keyboard.o\
	$(BUILD_DIR)kernel/drv/keyboard/keyboard_asm.o\
	$(BUILD_DIR)kernel/drv/keyboard/keyboard_c.o

	$(LDCMD) $@ $^