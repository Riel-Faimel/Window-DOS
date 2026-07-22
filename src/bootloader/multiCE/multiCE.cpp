#include "_multiCE.hpp"

multiCE::multiCE() {
    sechduler = new cs;
    sechduler->init(nullptr);
}


void multiCE::cut(size_t tid, size_t num) {
    sechduler->cut(tid, num);
}

void multiCE::run(void (*func)(size_t, void *)) {
    sechduler->run(func);
}