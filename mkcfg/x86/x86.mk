CC = x86_64-elf-gcc
CPP = x86_64-elf-g++
LD = x86_64-elf-ld
AS = nasm

CBASE = -m32 -nostdlib -fno-builtin -fno-stack-protector \
        -Wall -Wextra -ffreestanding -MMD -MP\
		-c -I $(SRC_DIR)include/ -I $(SRC_DIR)include/lib/ \
		-I $(SRC_DIR)kernel/ -I $(SRC_DIR)kernel/ -O2 -D_x86 

ifeq ($(COMDebug),1)
CBASE += -DCOMDebug
endif

CFLAGS = $(CBASE) -Werror=implicit-int

# 不再用 C++ 模块（cppm 已换成 hpp/cpp），所以去掉 -fmodules-ts 和 gcm.cache
# 那一套依赖修补。RTTI 全局打开：dynamic_cast 读的是对象自己的 vtable，
# 只有每个多态类型的 vtable 都带 typeinfo 才能跨 TU 正确识别。
CPPBASE = $(CBASE) -nodefaultlibs -frtti -fno-use-cxa-atexit -fno-function-sections -mno-sse
CPPFLAGS = $(CPPBASE) -fno-exceptions
CPPEXCEPTION = $(CPPBASE) -fexceptions
CPPRTTI = $(CPPBASE) -fno-exceptions

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

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPFLAGS) -c $< -o $@
	@ echo [CPP] $<

$(BUILD_DIR)%.o: $(SRC_DIR)%_exp.cpp
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPEXCEPTION) -c $< -o $@
	@ echo [CPP] ^<exception^> $<

$(BUILD_DIR)%.o: $(SRC_DIR)%_rtti.cpp
	@ $(call MKDIR_F,$@)
	@ $(CPP) -D_BITS_32 -std=c++20 $(CPPRTTI) -c $< -o $@
	@ echo [CPP] ^<rtti^> $<

$(BUILD_DIR)%.o: $(SRC_DIR)%.c 
	@ $(call MKDIR_F,$@)
	@ $(CC) -D_BITS_32 $(CFLAGS) -c $< -o $@
	@ echo [CC] $<

$(BUILD_DIR)%.o: $(SRC_DIR)%.asm
	@ $(call MKDIR_F,$@)
	@ $(AS) $(ASFLAGS) $< -o $@
	@ echo [AS] $<

$(BUILD_DIR)%.bin: $(SRC_DIR)%.asm
	@ $(call MKDIR_F,$@)
	@ $(AS) -f bin $< -o $@
	@ echo [AS] $<
