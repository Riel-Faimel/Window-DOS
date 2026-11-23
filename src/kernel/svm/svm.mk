$(BUILD_DIR)kernel/svm/svm.o: $(SRC_DIR)kernel/svm/svm_exp.cpp
$(BUILD_DIR)kernel/svm/vmop.o: $(SRC_DIR)kernel/svm/vmop.cpp

OBJ += \
	$(BUILD_DIR)kernel/svm/svm.o \
	$(BUILD_DIR)kernel/svm/vmop.o
