#include "_cs.hpp"

inline void tcb_push(TCB *tcb, size_t data, int bytes_num = 4) {
    tcb->xsp -= bytes_num;
    *(size_t *)(tcb->xsp) = data;
}

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
        if (code & 0x80000000) {
            // exit bit set
            sche.exit();
        } else [[likely]] switch (code) {
        case 0:
            sche.yield();
            break;
        default:
            break;
        };
        sche.ScheduleDecision();
        sche.resume();
    }
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::ScheduleDecision() {
    // context has been changed and set nullptr
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

__attribute__((optimize("O3"), used))
inline void DefaultScheduler::run(
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

__attribute__((optimize("O3")))
inline void DefaultScheduler::yield() {
    TCB*& nextlist = up_is_running ? downlist : uplist;
    // yield this tcb
    context_local->remain_size += context_local->time_size;
    context_local->next = nextlist;
    nextlist = context_local;
    context_local = nullptr;
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::wait(TCB *tid) {
    if (sleeplist != nullptr) {
        sleeplist->prev = context_local;
    }
    context_local->next = sleeplist;
    sleeplist = context_local;
    context_local = nullptr;
    // construc frame
    tcb_push(tid, tid->xflag);
    tcb_push(tid, tid->xip);
    tcb_push(tid, (size_t)sleeplist);
    tid->xip = tid->wait_handler;
}

__attribute__((optimize("O3")))
void DefaultScheduler::exit() {
    // exit this
    operator delete ((void *)context_local->xsp, mm);
    operator delete (context_local, mm);
    context_local = nullptr;
}

void DefaultScheduler::cut(TCB *tid, size_t num, void *argv) {
    tcb_push(tid, tid->xflag);
    tcb_push(tid, tid->xip);
    tcb_push(tid, num);
    tcb_push(tid, (size_t)argv);
    tid->xip = tid->int_handler;
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::wake(TCB *tid, size_t wake_num) {
    if (tid->prev) {
        // is sleeping
        tid->prev->next = tid->next;
        tid->prev = nullptr;
    } else if (tid == sleeplist) {
        sleeplist = tid->next;
    }
    
    TCB*& nextlist = up_is_running ? downlist : uplist;
    tid->next = nextlist;
    nextlist = tid;
    tcb_push(tid, wake_num);
}

__attribute__((optimize("O3")))
inline void DefaultScheduler::set_time_size(TCB *tcb, u16 time) {
    tcb->time_size = time;
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

inline void DefaultScheduler::reg_int(TCB *tcb, void *int_handler) {
    tcb->int_handler = (size_t)int_handler;
}
inline void DefaultScheduler::reg_wait(TCB *tcb, void *wait_handler) {
    tcb->wait_handler = (size_t)wait_handler;
}
