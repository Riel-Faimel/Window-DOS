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

SOURCES = $(wildcard $(SRC_DIR)kernel/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)%.cpp,$(BUILD_DIR)%.o,$(SOURCES))

# Every object is compiled with -MMD -MP, so the compiler drops a .d file next
# to it under $(BUILD_DIR).  Those files have to be included, otherwise a .hpp
# change is invisible to make and the .cpp files including it never rebuild.
# GNU make has no recursive wildcard, so the levels of build/kernel are walked
# explicitly (add one level here if a deeper source directory is ever added).
DEP_LEVELS = * */* */*/* */*/*/* */*/*/*/*
DEPENDS = $(foreach level,$(DEP_LEVELS),$(wildcard $(BUILD_DIR)kernel/$(level).d))
-include $(DEPENDS)

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
	
	$(LDCMD_F) $@ $^

#$(BUILD_DIR)kernel.bin: $(BUILD_DIR)kernel.pe
#	$(COPY) $(COPYFLAGS) $< $@