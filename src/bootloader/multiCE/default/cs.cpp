#include "_cs.hpp"

extern "C" void _Ssche();

DefaultScheduler::DefaultScheduler():
context_local{new TCB {}}, uplist{nullptr}{
    //interrupt_distributor.reg_irq(&_Ssche, IRQ::time);
}

extern "C" {
    [[noreturn]] void sche_c() {
        CPU *cpu_ptr;
        unsigned code;
        asm volatile (
            "movl %%fs:0, %0\n"
            "movl %%eax, %1\n"
            : "=r"(cpu_ptr), "=r"(code)
            :
            : "memory"
        );
        auto &sche = cpu_ptr->scheduler;
        sche.ScheduleDecision();
        //cout << code << "<<\n";
        //switch(code) {
        //    case 0: // yield
        //    sche.yield();
        //    case 1: // schedule
        //    default:
        //}
    }
}

[[noreturn]] void DefaultScheduler::yield() {
    context_local->next = sleeplist;
    sleeplist = context_local;
    context_local = nullptr;
    ScheduleDecision();
}

[[noreturn]] void DefaultScheduler::ScheduleDecision() {
    TCB*& thislist = up_is_running ? uplist : downlist;
    TCB*& nextlist = up_is_running ? downlist : uplist;
    
loop:
    if (context_local != nullptr) {
        // yield this tcb
        context_local->next = nextlist;
        nextlist = context_local;
        context_local = nullptr;
    }
    cout << '<' << context_local << ", " << thislist << ", " << nextlist << ">\n";
    
    if (thislist != nullptr) {
        context_local = thislist;
        thislist = thislist->next;
        Switch();
    } else if (nextlist != nullptr) {
        context_local = nextlist;
        nextlist = nextlist->next;
        up_is_running = !up_is_running;
        Switch();
    }
    asm volatile("hlt\n");
    goto loop;
}

[[noreturn]] inline void DefaultScheduler::Switch() <%
    auto tag_stack = (size_t *)(context_local->context.xsp);
    tag_stack--;
    *tag_stack = context_local->xss;
    tag_stack--;
    *tag_stack = context_local->context.xsp;
    tag_stack--;
    *tag_stack = context_local->xflag;
    tag_stack--;
    *tag_stack = context_local->xcs;
    tag_stack--;
    *tag_stack = context_local->xip;
    //cout << "new stack = " << tag_stack << ", context = " << &context_local->context << '\n';
    //while(1);
    asm volatile (
        "mov %0, %%esp\n"
        "jmp _ScsSwitch\n"
        :
        : "r"(&context_local->context)
        : "memory"
    );
%>


void DefaultScheduler::run(void *func, size_t argc, void *argv, u8 ring, size_t time) {
    TCB* &next_run = up_is_running? downlist:uplist;
    next_run = new TCB{
        .next = next_run,
        .time_size = time,
        .context = { .xsp = (size_t)new char[4069],.xdx = (size_t)argv,  .xax = argc,  },
        .xip = (size_t)func,
        .xcs = (size_t)((ring==0)?0x08:0x1B),
        .xss = (size_t)((ring==0)?0x10:0x23),
    };
}

void DefaultScheduler::cut(size_t tid, size_t num) {}