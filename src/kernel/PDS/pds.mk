include $(SRC_DIR)kernel/PDS/PCI/pci.mk
include $(SRC_DIR)kernel/PDS/USB/usb.mk
include $(SRC_DIR)kernel/PDS/ISA/isa.mk

$(BUILD_DIR)kernel/PDS/pds.o: $(SRC_DIR)kernel/PDS/pds.cpp

$(BUILD_DIR)kernel/PDS.o:\
	$(BUILD_DIR)kernel/PDS/pds.o\
	$(BUILD_DIR)kernel/PDS/pci.o\
	$(BUILD_DIR)kernel/PDS/usb.o\
	$(BUILD_DIR)kernel/PDS/isa.o

	$(LDCMD32) $@ $^