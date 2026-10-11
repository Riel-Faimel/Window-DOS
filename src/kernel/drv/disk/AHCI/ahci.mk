$(BUILD_DIR)kernel/drv/disk/AHCI/ahci.o: $(SRC_DIR)kernel/drv/disk/AHCI/ahci.cpp


$(BUILD_DIR)kernel/drv/disk/AHCI.o:\
	$(BUILD_DIR)kernel/drv/disk/AHCI/ahci.o\


	$(LDCMD) $@ $^