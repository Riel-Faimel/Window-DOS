$(BUILD_DIR)bootloader/x86/MBR/boot.bin: $(SRC_DIR)bootloader/x86/MBR/boot.asm

$(BUILD_DIR)bootloader.bin:\
	$(BUILD_DIR)bootloader/x86/MBR/boot.bin

	copy /b $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 1, $^))) $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $@))