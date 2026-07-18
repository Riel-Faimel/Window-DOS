ROOT_DIR := $(CURDIR)
MK_DIR = $(ROOT_DIR)/mkcfg/
BUILD_DIR = $(ROOT_DIR)/build/
SRC_DIR = $(ROOT_DIR)/src/

CC = gcc
CPP = g++
AS = nasm
AR = ar
LD = ld
COPY = objcopy
QEMU = qemu-system-x86_64

CC-ELF = x86_64-elf-gcc
CPP-ELF = x86_64-elf-g++
LD-ELF = x86_64-elf-ld

define MKDIR_F
	@if not exist "$(dir $(1))" mkdir "$(dir $(1))"
endef

export MKDIR_F MOD

include $(SRC_DIR)bootloader/bootloader.mk
include $(SRC_DIR)kernel/kernel.mk
include $(SRC_DIR)lib/lib.mk
#include $(ROOT_DIR)/app/app.mk

MOD = $(FS_MOD)
MOD += $(DRV_MOD)

libs: libos.a
module: module_32

MBR.bin: 
boot.img: $(BUILD_DIR)bootloader/start/boot.bin $(BUILD_DIR)bootloader.bin
	copy /b $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 1, $^))) + $(subst /,\,$(patsubst $(ROOT_DIR)/%, %, $(word 2, $^))) $@
kernel.exe:
result: boot.img kernel.exe
mod:
QEMUFLAGS = -serial stdio -drive format=raw,index=0,file=boot.img,media=disk \
	-drive format=raw,index=1,file=disk.vhd,media=disk

run: 
	$(QEMU) $(QEMUFLAGS)

clean:
	rmdir /s "$(BUILD_DIR)"
	rmdir /s "$(ROOT_DIR)/gcm.cache"
	del boot.img