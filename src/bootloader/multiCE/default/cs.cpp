#include "_cs.hpp"

extern "C" {
    __attribute__((optimize("O3")))
    [[noreturn]] void sche_c() {
        CPU *cpu_ptr;
        unsigned code;
        //void *frame;
        asm volatile (
            "movl %%fs:0, %0\n"
            "movl %%eax, %1\n"
            : "=r"(cpu_ptr), "=r"(code)
            :
            : "eax", "memory"
        );
        auto &sche = cpu_ptr->scheduler;
        switch (code) {
            case 0:
            sche.change();
            break;
            case 1:
            sche.yield();
            break;
            case 2:
            default:
            sche.exit();
            break;
        };
        sche.ScheduleDecision();
        sche.resume();
    }
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::change() {
    TCB*& thislist = up_is_running ? uplist : downlist;
    TCB*& nextlist = up_is_running ? downlist : uplist;
    
    if (context_local != nullptr) {
        // yield this tcb
        context_local->remain_size = context_local->time_size;
        context_local->next = nextlist;
        nextlist = context_local;
        context_local = nullptr;
    }
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::yield() {
    context_local->next = sleeplist;
    sleeplist = context_local;
    context_local = nullptr;
}

__attribute__((optimize("O3")))
void DefaultScheduler::exit(TCB *thread) {
    return;
    if (thread == nullptr) {
        // exit this
        operator delete (context_local, mm);
        context_local = 0;
    }
    auto p = thread->next;
    thread->next = p->next;
    delete p;
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::ScheduleDecision() {
    TCB*& thislist = up_is_running ? uplist : downlist;
    TCB*& nextlist = up_is_running ? downlist : uplist;
    
loop:    
    if (thislist != nullptr) {
        context_local = thislist;
        thislist = thislist->next;
        return;
    } else if (nextlist != nullptr) {
        context_local = nextlist;
        nextlist = nextlist->next;
        up_is_running = !up_is_running;
        return;
    }
    asm volatile("hlt\n");
    goto loop;
}

__attribute__((optimize("O3")))
[[noreturn]] void DefaultScheduler::resume() <%
    if ((context_local->xcs & 3) == 0) {
        //ring0 stack
        asm volatile (
            "mov %0, %%esp\n"
            "mov %1, %%eax\n"
            "jmp _Sring0_back\n"
            :
            : "r"(context_local->xsp), "r"(context_local)
            : "eax", "memory"
        );
    }
    else {
        //ring3 stack
        asm volatile (
            "mov %0, %%esp\n"
            "jmp _Sring3_back\n"
            : 
            : "r"(&context_local->xds)
            : "memory"
        );
    }
%>

__attribute__((optimize("O3")))
void DefaultScheduler::run(
    void *func, size_t argc, void *argv, size_t time, 
    size_t code_seg, size_t data_seg, size_t gs, size_t fs
) {
    TCB* &next_run = up_is_running? downlist:uplist;
    auto new_run = new (mm) TCB{};
    new_run->next = next_run;
    new_run->time_size = (u16)time;
    new_run->remain_size = (u16)time;
    new_run->context.xdx = (size_t)argv;
    new_run->context.xax = argc;
    new_run->xip = (size_t)func;
    new_run->xcs = code_seg & 0xFFFF;
    new_run->xsp = (size_t)new (mm) char[4069];
    new_run->xss = data_seg & 0xFFFF;
    new_run->xfs = fs & 0xFFFF;
    new_run->xds = data_seg & 0xFFFF;
    new_run->xes = data_seg & 0xFFFF;
    new_run->xgs = gs & 0xFFFF;
    next_run = new_run;
}

DefaultScheduler::DefaultScheduler(MemoryManager& mm_, IDT& idt):mm{mm_}{
    new (mm) TCB {};
    idt.regist(&_Sexit, 0x20);
}

void DefaultScheduler::start_preemption(IDT &idt) {
    idt.regist(&_Sint_sche, 48);
}
void DefaultScheduler::close_preemption(IDT &idt) {
    idt.regist(&basic_time_handler_c, 48);
}
void DefaultScheduler::cut(TCB *tid, size_t num) {}