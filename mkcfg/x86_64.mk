CFLAGS64 = -m64 -nostdlib -fno-builtin -fno-stack-protector \
        -Wall -Wextra -ffreestanding \
		-c -I $(SRC_DIR)include/ -I $(SRC_DIR)include/lib/ \
		-I $(SRC_DIR)kernel/ -I $(SRC_DIR)bootloader/

CPPFLAGS64 = $(CFLAGS64) -nodefaultlibs -fno-rtti -fno-exceptions -fno-use-cxa-atexit
LDFLAGS_B64 = -nostdlib -m i386pep
LDFLAGS64 = $(LDFLAGS_B64) -r
LDFLAGS_F64 = $(LDFLAGS_B64) -static
ASFLAGS64 = -f win64 

LDCMD64 = $(LD) $(LDFLAGS64) -o
LDCMD_F64 = $(LD) $(LDFLAGS64) -o

export LDCMD64 LDCMD_F64

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	$(call MKDIR_F,$@)
	$(CPP) $(CPPFLAGS64) -c $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.c 
	$(call MKDIR_F,$@)
	$(CC) $(CFLAGS64) -c $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) $(ASFLAGS64) $< -o $@

$(BUILD_DIR)%.bin: $(SRC_DIR)%.asm
	$(call MKDIR_F,$@)
	$(AS) -f bin $< -o $@