$(BUILD_DIR)bootloader/drv/disk/AHCI/ahci.o: $(SRC_DIR)bootloader/drv/disk/AHCI/ahci.cpp


$(BUILD_DIR)bootloader/drv/disk/AHCI.o:\
	$(BUILD_DIR)bootloader/drv/disk/AHCI/ahci.o\


	$(LDCMD32) $@ $^