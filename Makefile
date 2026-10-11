ROOT_DIR := $(CURDIR)
MK_DIR = $(ROOT_DIR)/mkcfg/
BUILD_DIR = $(ROOT_DIR)/build/
SRC_DIR = $(ROOT_DIR)/src/
ARCH = x86

AR = ar
COPY = objcopy
QEMU = qemu-system-x86_64

ifeq ($(ARCH), x86)
include $(MK_DIR)/x86/x86.mk
endif
ifeq ($(ARCH), STM32H750)
include $(MK_DIR)/ARM/STM32/H750.mk
endif

# ======

define MKDIR_F
	@if not exist "$(dir $(1))" mkdir "$(dir $(1))"
endef

export MKDIR_F MOD ARCH

include $(SRC_DIR)kernel/kernel.mk
include $(SRC_DIR)bootloader/bootloader.mk
include $(SRC_DIR)lib/lib.mk
#include $(ROOT_DIR)/app/app.mk

MOD = $(FS_MOD)
MOD += $(DRV_MOD)

libs: libos.a
module: module_32

boot.img: $(BUILD_DIR)bootloader.bin $(BUILD_DIR)kernel.bin 
	copy /b $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 1, $^))) + $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 2, $^))) $@
kernel.exe:
result: boot.img kernel.exe

QEMUFLAGS = -serial stdio \
	-device ich9-ahci,id=sata\
	-drive format=raw,index=0,file=boot.img,media=disk \
	-drive format=raw,id=disk1,file=flp.vhd,if=none\
	-device ide-hd,drive=disk1,bus=sata.1

run: 
	$(QEMU) $(QEMUFLAGS)

clean:
	rmdir /s "$(BUILD_DIR)"
	rmdir /s "$(ROOT_DIR)/gcm.cache"
	del boot.img

debug:
	start "" gdb -ex "set architecture i386" -ex "target remote 127.0.0.1:1234" & qemu-system-i386 -s -S $(QEMUFLAGS)