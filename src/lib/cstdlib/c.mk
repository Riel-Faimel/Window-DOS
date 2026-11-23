$(BUILD_DIR)lib/cstdlib/port.o: $(SRC_DIR)lib/cstdlib/port.cpp
$(BUILD_DIR)lib/cstdlib/memop.o: $(SRC_DIR)lib/cstdlib/memop.cpp

LIBOBJ += \
	$(BUILD_DIR)lib/cstdlib/port.o\
	$(BUILD_DIR)lib/cstdlib/memop.o