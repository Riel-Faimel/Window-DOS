include $(SRC_DIR)kernel/drv/screen/screen.mk
include $(SRC_DIR)kernel/drv/disk/disk.mk
include $(SRC_DIR)kernel/drv/fs/fs.mk
include $(SRC_DIR)kernel/drv/bus/PCI/pci.mk
include $(SRC_DIR)kernel/drv/bus/USB/usb.mk
include $(SRC_DIR)kernel/drv/bus/ISA/isa.mk
include $(SRC_DIR)kernel/drv/keyboard/keyboard.mk
include $(SRC_DIR)kernel/drv/syscall/syscall.mk

$(BUILD_DIR)kernel/drv/kmod.o: $(SRC_DIR)kernel/drv/kmod_exp.cpp

OBJ += \
	$(BUILD_DIR)kernel/drv/kmod.o