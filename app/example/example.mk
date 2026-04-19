$(BUILD_DIR)app/example/example.o: $(ROOT_DIR)/app/example/example.cpp
$(BUILD_DIR)app/example/example_asm.o: $(ROOT_DIR)/app/example/example_asm.asm

$(BUILD_DIR)app/example.bin:\
	$(BUILD_DIR)app/example/example.o\
	$(BUILD_DIR)app/_start.o\
	$(BUILD_DIR)app/example/example_asm.o

	$(LDCMD_F32) $@ $^