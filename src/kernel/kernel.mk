include $(MK_DIR)x86.mk

include $(SRC_DIR)kernel/mm/mm.mk
include $(SRC_DIR)kernel/arch/arch.mk
include $(SRC_DIR)kernel/id/id.mk
include $(SRC_DIR)kernel/drv/drv.mk
include $(SRC_DIR)kernel/module/module.mk
include $(SRC_DIR)kernel/syscall/syscall.mk
include $(SRC_DIR)kernel/CDM/DLS.mk
include $(SRC_DIR)kernel/PDS/pds.mk
include $(SRC_DIR)kernel/multiCE/multiCE.mk
include $(SRC_DIR)kernel/TM/TM.mk
include $(SRC_DIR)kernel/GEH/geh.mk

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)kernel/loader.o: $(SRC_DIR)kernel/loader.cpp

$(BUILD_DIR)kernel.bin: $(OBJECTS)\
	$(BUILD_DIR)kernel/loader.o\
	$(BUILD_DIR)kernel/mm.o\
	$(BUILD_DIR)kernel/drv.o\
	$(BUILD_DIR)kernel/module.o\
	$(BUILD_DIR)kernel/SSS.o\
	$(BUILD_DIR)kernel/PDS.o\
	$(BUILD_DIR)kernel/multiCE.o\
	$(BUILD_DIR)kernel/TM.o\
	$(BUILD_DIR)kernel/arch.o\
	$(BUILD_DIR)kernel/id.o\
	$(BUILD_DIR)kernel/GEH.o\

#\
	$(BUILD_DIR)kernel/syscall.o\
	
	$(LDCMD_F32) $@ $^

#$(BUILD_DIR)kernel.bin: $(BUILD_DIR)kernel.pe
#	$(COPY) $(COPYFLAGS) $< $@