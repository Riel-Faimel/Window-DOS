CC = x86_64-elf-gcc
CPP = x86_64-elf-g++
LD = x86_64-elf-ld
AS = nasm

CBASE = -m32 -nostdlib -fno-builtin -fno-stack-protector \
        -Wall -Wextra -ffreestanding -MMD -MP\
		-c -I $(SRC_DIR)include/ -I $(SRC_DIR)include/lib/ \
		-I $(SRC_DIR)kernel/ -I $(SRC_DIR)kernel/ -O2 -D_DEBUG -D_x86 

CFLAGS = $(CBASE) -Werror=implicit-int
CPPBASE = $(CBASE) -nodefaultlibs -fno-rtti -fno-use-cxa-atexit -fno-function-sections  -fmodules-ts -mno-sse
CPPFLAGS = $(CPPBASE) -fno-exceptions
CPPEXCEPTION = $(CPPBASE) -fexceptions -frtti
CPPRTTI = $(CPPBASE) -fno-exceptions -frtti
	
LDFLAGS_B = -nostdlib -m elf_i386
LDFLAGS = $(LDFLAGS_B) -r
LDFLAGS_F = $(LDFLAGS_B) -static -T $(MK_DIR)/x86/x86.ld -Map=loader.map -L$(BUILD_DIR)lib -los
ASFLAGS = -f elf32

LDCMD = @ $(LD) $(LDFLAGS) -o
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
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPFLAGS) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CPP] $<

$(BUILD_DIR)%.o: $(SRC_DIR)%_exp.cpp
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPEXCEPTION) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CPP] ^<exception^> $<

$(BUILD_DIR)%.o: $(SRC_DIR)%_rtti.cpp
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPRTTI) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CPP] ^<rtti^> $<

$(BUILD_DIR)%.o: $(SRC_DIR)%.c 
	@ $(call MKDIR_F,$@)
	@ $(CC) -D_BITS_32 $(CFLAGS) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CC] $<

$(BUILD_DIR)%.o: $(SRC_DIR)%.asm
	@ $(call MKDIR_F,$@)
	@ $(AS) $(ASFLAGS) $< -o $@
	@ echo [AS] $<

$(BUILD_DIR)%.bin: $(SRC_DIR)%.asm
	@ $(call MKDIR_F,$@)
	@ $(AS) -f bin $< -o $@
	@ echo [AS] $<

$(BUILD_DIR)%.o: $(SRC_DIR)%.cppm
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPFLAGS) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CPP] ^<module^> $<

$(BUILD_DIR)%.o: $(SRC_DIR)%_exp.cppm
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPEXCEPTION) -c $< -o $@
	@ $(call STRIP_MODULE_DEPS,$(@:.o=.d))
	@ echo [CPP] ^<module^> ^<exception^> $<