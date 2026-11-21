include $(SRC_DIR)kernel/KrnlObjMgr/builtin_conv/bc.mk
$(BUILD_DIR)kernel/KrnlObjMgr/kom.o: $(SRC_DIR)kernel/KrnlObjMgr/kom_exp.cpp
$(BUILD_DIR)kernel/KrnlObjMgr/knlobj.o: $(SRC_DIR)kernel/KrnlObjMgr/knlobj_exp.cpp

OBJ += \
	$(BUILD_DIR)kernel/KrnlObjMgr/kom.o \
	$(BUILD_DIR)kernel/KrnlObjMgr/knlobj.o
