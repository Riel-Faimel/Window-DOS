$(BUILD_DIR)kernel/drv/screen/vga.o: $(SRC_DIR)kernel/drv/screen/vga.cpp
$(BUILD_DIR)kernel/drv/screen/screen_srv.o: $(SRC_DIR)kernel/drv/screen/screen_srv.cpp

$(BUILD_DIR)kernel/drv/screen.o: \
	$(BUILD_DIR)kernel/drv/screen/vga.o\
	$(BUILD_DIR)kernel/drv/screen/screen_srv.o\

	$(LDCMD) $@ $^