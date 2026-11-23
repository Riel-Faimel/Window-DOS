ROOT_DIR := $(CURDIR)
MK_DIR = $(ROOT_DIR)/mkcfg/
BUILD_DIR = $(ROOT_DIR)/build/
SRC_DIR = $(ROOT_DIR)/src/
ARCH = x86

AR = ar
COPY = objcopy
QEMU = qemu-system-x86_64


LIBOBJ =
OBJ =
KMOD =

ifeq ($(ARCH), x86)
include $(MK_DIR)x86/x86.mk
endif
ifeq ($(ARCH), STM32H750)
include $(MK_DIR)ARM/Core-M7/STM32/H750.mk
endif

# ======

define MKDIR_F
	@if not exist "$(dir $(1))" mkdir "$(dir $(1))"
endef

export MKDIR_F MOD ARCH

include $(SRC_DIR)lib/lib.mk
include $(SRC_DIR)bootloader/bootloader.mk
include $(SRC_DIR)kernel/kernel.mk
#include $(ROOT_DIR)/app/app.mk

# libos.a 用 order-only 依赖：必须先建好，但不能进 $^（链接靠 -los）
$(BUILD_DIR)kernel.bin:  $(OBJ) | $(BUILD_DIR)lib/libos.a
	@ $(LD) -o $@ $^ $(LDFLAGS_F)
	@ echo [LD] $@

lib: $(BUILD_DIR)lib/libos.a
module: module_32

result: $(BUILD_DIR)bootloader.bin $(BUILD_DIR)kernel.bin 

all: lib result run

DISK1 = -drive format=raw,index=0,file=boot.img,media=disk
DISK2 = \
	-device ich9-ahci,id=sata\
	-drive format=raw,id=disk1,file=disk.vhd,if=none\
	-device ide-hd,drive=disk1,bus=sata.1
#\
	-drive format=raw,index=1,file=flp.vhd,media=disk

QEMUFLAGS = -serial stdio $(DISK1) $(DISK2)

boot.img: $(BUILD_DIR)bootloader.bin $(BUILD_DIR)kernel.bin 
	@ copy /b $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 1, $^))) + $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 2, $^))) $@
	@ echo [COPY] $@
run: boot.img
	$(QEMU) $(QEMUFLAGS)

clean:
	rmdir /s /q "$(BUILD_DIR)"
	rmdir /s /q "$(ROOT_DIR)/gcm.cache"
	del .\boot.img

debug:
	start "" gdb -ex "set architecture i386" -ex "target remote 127.0.0.1:1234" & qemu-system-i386 -s -S $(QEMUFLAGS)