include $(SRC_DIR)kernel/KOM/builtin_conv/bc.mk
$(BUILD_DIR)kernel/kom/kom.o: $(SRC_DIR)kernel/kom/kom.cpp

OBJ += \
	$(BUILD_DIR)kernel/kom/kom.o