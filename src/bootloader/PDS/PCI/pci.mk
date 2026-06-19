$(BUILD_DIR)bootloader/PDS/PCI/pci.o: $(SRC_DIR)bootloader/PDS/PCI/pci.cpp

$(BUILD_DIR)bootloader/PDS/pci.o:\
	$(BUILD_DIR)bootloader/PDS/PCI/pci.o

	$(LDCMD32) $@ $^