$(BUILD_DIR)kernel/PDS/PCI/pci.o: $(SRC_DIR)kernel/PDS/PCI/pci.cpp

$(BUILD_DIR)kernel/PDS/pci.o:\
	$(BUILD_DIR)kernel/PDS/PCI/pci.o

	$(LDCMD32) $@ $^