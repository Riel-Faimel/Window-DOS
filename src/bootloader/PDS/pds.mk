include $(SRC_DIR)bootloader/PDS/PCI/pci.mk

$(BUILD_DIR)bootloader/PDS/pds.o: $(SRC_DIR)bootloader/PDS/pds.cpp

$(BUILD_DIR)bootloader/PDS.o:\
	$(BUILD_DIR)bootloader/PDS/pds.o\
	$(BUILD_DIR)bootloader/PDS/PCI/pci.o

	$(LDCMD32) $@ $^