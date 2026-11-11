include $(SRC_DIR)lib/cppstdlib/cpp.mk
#include $(SRC_DIR)lib/cstdlib/c.mk
include $(SRC_DIR)lib/module/module.mk

$(BUILD_DIR)lib/libos.a: $(LIBOBJ)
	@ $(AR) rcs $@ $^
	@ echo [AR] libos.a