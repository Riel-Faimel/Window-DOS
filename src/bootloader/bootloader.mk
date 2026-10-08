include $(MK_DIR)x86.mk

include $(SRC_DIR)bootloader/mm/mm.mk
include $(SRC_DIR)bootloader/arch/arch.mk
include $(SRC_DIR)bootloader/id/id.mk
include $(SRC_DIR)bootloader/drv/drv.mk
include $(SRC_DIR)bootloader/module/module.mk
include $(SRC_DIR)bootloader/registry/registry.mk
include $(SRC_DIR)bootloader/syscall/syscall.mk
include $(SRC_DIR)bootloader/CDM/DLS.mk
include $(SRC_DIR)bootloader/PDS/pds.mk
include $(SRC_DIR)bootloader/multiCE/multiCE.mk
include $(SRC_DIR)bootloader/TM/TM.mk
include $(SRC_DIR)bootloader/start/start.mk
include $(SRC_DIR)bootloader/GEH/geh.mk

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)bootloader/loader.o: $(SRC_DIR)bootloader/loader.cpp

$(BUILD_DIR)bootloader.bin: $(OBJECTS)\
	$(BUILD_DIR)bootloader/start.o\
	$(BUILD_DIR)bootloader/loader.o\
	$(BUILD_DIR)bootloader/mm.o\
	$(BUILD_DIR)bootloader/registry.o\
	$(BUILD_DIR)bootloader/drv.o\
	$(BUILD_DIR)bootloader/module.o\
	$(BUILD_DIR)bootloader/SSS.o\
	$(BUILD_DIR)bootloader/PDS.o\
	$(BUILD_DIR)bootloader/multiCE.o\
	$(BUILD_DIR)bootloader/syscall.o\
	$(BUILD_DIR)bootloader/TM.o\
	$(BUILD_DIR)bootloader/arch.o\
	$(BUILD_DIR)bootloader/id.o\
	$(BUILD_DIR)bootloader/GEH.o\

#
	
	$(LDCMD_F32) $@ $^

#$(BUILD_DIR)bootloader.bin: $(BUILD_DIR)bootloader.pe
#	$(COPY) $(COPYFLAGS) $< $@