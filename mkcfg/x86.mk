CFLAGS32 = -m32 -nostdlib -fno-builtin -fno-stack-protector \
        -Wall -Wextra -ffreestanding -Werror=implicit-int -MMD -MP\
		-c -I $(SRC_DIR)include/ -I $(SRC_DIR)include/lib/ \
		-I $(SRC_DIR)kernel/ -I $(SRC_DIR)bootloader/

CPPFLAGS32 = $(CFLAGS32) -nodefaultlibs -fno-rtti -fno-exceptions -fno-use-cxa-atexit\
	-fno-function-sections -O2 -fmodules-ts -mno-sse
LDFLAGS_B32 = -nostdlib -m elf_i386
LDFLAGS32 = $(LDFLAGS_B32) -r
LDFLAGS_F32 = $(LDFLAGS_B32) -static -T $(MK_DIR)x86.ld -Map=loader.map
ASFLAGS32 = -f elf32

LDCMD32 = $(LD-ELF) $(LDFLAGS32) -o
LDCMD_F32 = $(LD-ELF) $(LDFLAGS_F32) -o

COPYFLAGS = -O binary --only-section=.start\
	--only-section=.text\
	--only-section=.data\
	--only-section=.rodata

export LDCMD32 LDCMD_F32

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	$(call MKDIR_F,$@)
	$(CPP-ELF) $(CPPFLAGS32) -c $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.c 
	$(call MKDIR_F,$@)
	$(CC-ELF) $(CFLAGS32) -c $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) $(ASFLAGS32) $< -o $@

$(BUILD_DIR)%.bin: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) -f bin $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.cppm
	$(call MKDIR_F,$@)
	$(CPP-ELF) $(CPPFLAGS32) -c $< -o $@