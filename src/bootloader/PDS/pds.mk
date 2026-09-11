include $(SRC_DIR)bootloader/PDS/PCI/pci.mk
include $(SRC_DIR)bootloader/PDS/USB/usb.mk
include $(SRC_DIR)bootloader/PDS/ISA/isa.mk

$(BUILD_DIR)bootloader/PDS/pds.o: $(SRC_DIR)bootloader/PDS/pds.cpp

$(BUILD_DIR)bootloader/PDS.o:\
	$(BUILD_DIR)bootloader/PDS/pds.o\
	$(BUILD_DIR)bootloader/PDS/pci.o\
	$(BUILD_DIR)bootloader/PDS/usb.o\
	$(BUILD_DIR)bootloader/PDS/isa.o

	$(LDCMD32) $@ $^