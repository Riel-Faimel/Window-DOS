$(BUILD_DIR)bootloader\ARM\Core-M7\STM32\H750.bin: $(SRC_DIR)bootloader\ARM\Core-M7\STM32\H750.asm

$(BUILD_DIR)bootloader.bin:\
	$(BUILD_DIR)bootloader/ARM/Core-M7/STM32/H750.bin

	copy /b $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 1, $^))) $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $@))