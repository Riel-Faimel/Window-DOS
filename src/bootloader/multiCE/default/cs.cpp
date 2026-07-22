#include "cs.hpp"

cs::~cs() {}
void cs::init(TCB *tcbv) {
    this->tcbv = tcbv;
}

void cs::kill() {
    tcbv[run_id].state = TCB::State::Die;
}
void cs::yield() {
    tcbv[run_id].state = TCB::State::Wait;
}
void cs::Ready() {
    tcbv[run_id].state = TCB::State::Run;
}

void cs::run(void (*)(size_t argc, void *argv)) {}

void cs::cut(size_t tid, size_t num) {}