$(BUILD_DIR)bootloader/multiCE/multiCE.o: $(SRC_DIR)bootloader/multiCE/multiCE.cpp
$(BUILD_DIR)bootloader/multiCE/default/cs.o: $(SRC_DIR)bootloader/multiCE/default/cs.cpp
$(BUILD_DIR)bootloader/multiCE/default/cs_asm.o: $(SRC_DIR)bootloader/multiCE/default/cs_asm.asm

$(BUILD_DIR)bootloader/multiCE.o: \
	$(BUILD_DIR)bootloader/multiCE/multiCE.o\
	$(BUILD_DIR)bootloader/multiCE/default/cs.o\
	$(BUILD_DIR)bootloader/multiCE/default/cs_asm.o

	$(LDCMD32) $@ $^