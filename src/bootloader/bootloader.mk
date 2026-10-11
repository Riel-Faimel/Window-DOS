
ifeq ($(ARCH), x86)
include $(SRC_DIR)bootloader/x86/x86.mk
endif
ifeq ($(ARCH), STM32H750)
include $(SRC_DIR)bootloader/ARM/ARM.mk
endif