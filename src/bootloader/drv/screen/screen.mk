$(BUILD_DIR)bootloader/drv/screen/vga.o: $(SRC_DIR)bootloader/drv/screen/vga.cpp
$(BUILD_DIR)bootloader/drv/screen/vbe.o: $(SRC_DIR)bootloader/drv/screen/vbe.cpp
$(BUILD_DIR)bootloader/drv/screen/screen_srv.o: $(SRC_DIR)bootloader/drv/screen/screen_srv.cpp

$(BUILD_DIR)bootloader/drv/screen.o: \
	$(BUILD_DIR)bootloader/drv/screen/vga.o\
	$(BUILD_DIR)bootloader/drv/screen/vbe.o\
	$(BUILD_DIR)bootloader/drv/screen/screen_srv.o

	$(LDCMD32) $@ $^