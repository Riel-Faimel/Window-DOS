include $(MK_DIR)x86.mk

include $(SRC_DIR)bootloader/mm/mm.mk
include $(SRC_DIR)bootloader/DescripTable/DescripTable.mk
include $(SRC_DIR)bootloader/drv/drv.mk
include $(SRC_DIR)bootloader/module/module.mk
include $(SRC_DIR)bootloader/registry/registry.mk
include $(SRC_DIR)bootloader/DOSsyscall/DOSsyscall.mk
include $(SRC_DIR)bootloader/SSS/DLS.mk
include $(SRC_DIR)bootloader/PDS/pds.mk
include $(SRC_DIR)bootloader/multiCE/multiCE.mk
include $(SRC_DIR)bootloader/TM/TM.mk

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)bootloader/loader.o: $(SRC_DIR)bootloader/loader.cpp
$(BUILD_DIR)bootloader/start/_start.o: $(SRC_DIR)bootloader/start/_start.asm
$(BUILD_DIR)bootloader/start/boot.bin: $(SRC_DIR)bootloader/start/boot.asm

$(BUILD_DIR)bootloader.bin: $(OBJECTS)\
	$(BUILD_DIR)bootloader/start/_start.o\
	$(BUILD_DIR)bootloader/loader.o\
	$(BUILD_DIR)bootloader/mm.o\
	$(BUILD_DIR)bootloader/DescripTable.o\
	$(BUILD_DIR)bootloader/drv.o\
	$(BUILD_DIR)bootloader/module.o\
	$(BUILD_DIR)bootloader/registry.o\
	$(BUILD_DIR)bootloader/SSS.o\
	$(BUILD_DIR)bootloader/PDS.o\
	$(BUILD_DIR)bootloader/multiCE.o\
	$(BUILD_DIR)bootloader/TM.o

#	$(BUILD_DIR)bootloader/test.o\
	$(BUILD_DIR)bootloader/CenterShell.o\
	$(BUILD_DIR)bootloader/part-fs.o\
	$(BUILD_DIR)bootloader/DOSsyscall.o\
	
	$(LDCMD_F32) $@ $^

#$(BUILD_DIR)bootloader.bin: $(BUILD_DIR)bootloader.pe
#	$(COPY) $(COPYFLAGS) $< $@