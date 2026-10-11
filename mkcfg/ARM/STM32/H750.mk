CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
LD = arm-none-eabi-ld
AS = arm-none-eabi-gcc

CBASE = -mcpu=cortex-m7 -mthumb -g -Wall -ffreestanding \
        -fno-exceptions -fno-rtti -fno-use-cxa-atexit -std=c++20 \
		-MMD -MP -I $(SRC_DIR)include/ -I $(SRC_DIR)include/lib/ \
		-I $(SRC_DIR)kernel/ -I $(SRC_DIR)kernel/ -O2 -D_DEBUG\

CFLAGS = $(CBASE) -Werror=implicit-int
CPPBASE = $(CBASE) -nodefaultlibs -fno-rtti -fno-use-cxa-atexit -fno-function-sections -fmodules-ts
CPPFLAGS = $(CPPBASE) -fno-exceptions
CPPEXCEPTION = $(CPPBASE) -fexceptions
	
LDFLAGS_B = -nostdlib
LDFLAGS32 = $(LDFLAGS_B) -r
LDFLAGS_F = $(LDFLAGS_B) -static -T $(MK_DIR)M32H750.ld -Map=loader.map
ASFLAGS = -f elf32

LDCMD = $(LD) $(LDFLAGS32) -o
LDCMD_F = $(LD) $(LDFLAGS_F) -o

COPYFLAGS = -O binary --only-section=.start\
	--only-section=.text\
	--only-section=.data\
	--only-section=.rodata

export LDCMD LDCMD_F

# Because of -fmodules-ts the compiler appends module bookkeeping to the -MMD
# output, e.g.
#   <obj>: lib32.c++-module
#   lib32.c++-module: gcm.cache/lib32.gcm
#   .PHONY: lib32.c++-module
# Those <module>.c++-module names are not real files and they are declared phony
# by the compiler itself, so once the .d files are included every object that
# imports a module looks out of date on every single run.  Drop those lines
# (they are rewritten on each compile, so this stays correct).  The ordering
# <obj> -> gcm.cache/<module>.gcm -> <module>.o -> <module>.cppm is preserved by
# the remaining lines, which is all the module build order needs.
# Windows findstr reads forward slashes as switch prefixes, so the file name has
# to be handed over in backslash form.
define STRIP_MODULE_DEPS
	@if exist "$(1)" findstr /V /C:"c++-module" "$(subst /,\,$(1))" > "$(subst /,\,$(1)).tmp"
	@if exist "$(subst /,\,$(1)).tmp" move /Y "$(subst /,\,$(1)).tmp" "$(subst /,\,$(1))" > nul
endef

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	$(call MKDIR_F,$@)
	$(CPP) -D_BITS_32 -std=c++20 $(CPPFLAGS) -c $< -o $@
	$(call STRIP_MODULE_DEPS,$(@:.o=.d))

$(BUILD_DIR)%.o: $(SRC_DIR)%_exception.cpp
	$(call MKDIR_F,$@)
	$(CPP) -D_BITS_32 -std=c++20 $(CPPEXCEPTION) -c $< -o $@
	$(call STRIP_MODULE_DEPS,$(@:.o=.d))

$(BUILD_DIR)%.o: $(SRC_DIR)%.c 
	$(call MKDIR_F,$@)
	$(CC) -D_BITS_32 $(CFLAGS) -c $< -o $@
	$(call STRIP_MODULE_DEPS,$(@:.o=.d))

$(BUILD_DIR)%.o: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)%.bin: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) -f bin $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.cppm
	$(call MKDIR_F,$@)
	$(CPP) -D_BITS_32 -std=c++20 $(CPPFLAGS) -x c++ -c $< -o $@
	$(call STRIP_MODULE_DEPS,$(@:.o=.d))

$(BUILD_DIR)%.o: $(SRC_DIR)%_exception.cppm
	$(call MKDIR_F,$@)
	$(CPP) -D_BITS_32 -std=c++20 $(CPPEXCEPTION) -x c++ -c $< -o $@
	$(call STRIP_MODULE_DEPS,$(@:.o=.d))