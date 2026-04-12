$(BUILD_DIR)bootloader/drv/PCI/pci.o: $(SRC_DIR)bootloader/drv/PCI/pci.cpp

$(BUILD_DIR)bootloader/drv/pci.o:\
	$(BUILD_DIR)bootloader/drv/PCI/pci.o

	$(LDCMD32) $@ $^