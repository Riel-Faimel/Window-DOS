#$(BUILD_DIR)bootloader/CenterShell/cs.o: $(SRC_DIR)bootloader/CenterShell/cs.cpp
#$(BUILD_DIR)bootloader/CenterShell/cmd_shell.o: $(SRC_DIR)bootloader/CenterShell/cmd_shell.cpp
#$(BUILD_DIR)bootloader/CenterShell/cmd_shell_asm.o: $(SRC_DIR)bootloader/CenterShell/cmd_shell_asm.asm
$(BUILD_DIR)bootloader/CenterShell/string.o: $(SRC_DIR)bootloader/CenterShell/string.cpp

$(BUILD_DIR)bootloader/CenterShell.o: \
	$(BUILD_DIR)bootloader/CenterShell/string.o
#	$(BUILD_DIR)bootloader/CenterShell/cs.o\
	$(BUILD_DIR)bootloader/CenterShell/cmd_shell.o\
	$(BUILD_DIR)bootloader/CenterShell/cmd_shell_asm.o\


	$(LDCMD32) $@ $^