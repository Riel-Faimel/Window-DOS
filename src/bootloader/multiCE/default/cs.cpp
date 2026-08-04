#include "cs.hpp"

cs::cs(){}

cs::~cs() {}
void cs::init(TCB *tcbv, size_t tcvc) {
    if(tcbv != nullptr) {
        this->tcbv.init(tcbv, tcvc);
    }
}

extern "C" {
    __attribute__((regparm(2)))
    void _ScsSwitch(TCB* from, TCB* to);
}

void cs::Switch(size_t to) <%
    _ScsSwitch(&tcbv[run_id], &tcbv[to]);
%>

void cs::ScheduleDecision() <%
    ;
%>

#include <drv/screen/screen_srv.hpp>

void cs::run(void (*func)(size_t, void *), size_t argc, void *argv) {
    tcbv.append(
        TCB{
            .thread_ID = tcbv.get_size(),
            .xsp = (size_t)argv,
            .xip = (size_t)func,
            .state = TCB::State::Run,
            .time_size = 1,
        }
    );
    kprint("cs::run\n");
    Switch(tcbv.get_size() - 1);
}

void cs::cut(size_t tid, size_t num) {}