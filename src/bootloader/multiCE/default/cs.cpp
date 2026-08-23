#include "_cs.hpp"

DefaultScheduler::DefaultScheduler():
context_local{new TCB {}}, uplist{nullptr}{
    //cout << "sche in: " << this << '\n';
    //interrupt_distributor.reg_irq(&_Ssche, IRQ::time);
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
        //cout << code << "<<\n";
        switch (code) {
            case 0:
            sche.change();
            sche.ScheduleDecision();
            sche.resume();
            case 1:
            sche.yield();
            sche.ScheduleDecision();
            sche.resume();
            case 2:
            sche.exit();
            sche.ScheduleDecision();
            sche.resume();
            default:
            // WTF!?
            ;
        };
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
    if (thread == nullptr) {
        // exit this
        delete context_local;
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
    if (context_local->xcs & 3) {
        //int stack
        asm volatile (
            "mov %0, %%esp\n"
            "jmp _Sint_stack\n"
            : 
            : "r"(&context_local->context)
            : "memory"
        );
    }
    else {
        //call stack
        //cout << "new stack = " << context_local->xsp << ", context = " << &context_local->context << '\n';
        asm volatile (
            "mov %0, %%esp\n"
            "jmp _Scall_stack\n"
            :
            : "r"(&context_local->context)
            : "memory"
        );
    }
%>

__attribute__((optimize("O3")))
void DefaultScheduler::run(void *func, size_t argc, void *argv, u8 ring, size_t time) {
    TCB* &next_run = up_is_running? downlist:uplist;
    next_run = new TCB{
        .next = next_run,
        .time_size = (u16)time,
        .context = { .xdx = (size_t)argv,  .xax = argc,  },
        .xip = (size_t)func,
        .xcs = (size_t)((ring==0)?0x08:0x1B),
        .xsp = (size_t)new char[4069],
        .xss = (size_t)((ring==0)?0x10:0x23),
    };
}

void DefaultScheduler::cut(TCB *tid, size_t num) {}