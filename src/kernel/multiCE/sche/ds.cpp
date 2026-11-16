#include "_ds.hpp"

DefaultScheduler::DefaultScheduler(ArchContext &ctx_, TCB *initial)
    : context_local{initial}, ctx{ctx_} {}

void DefaultScheduler::enqueue(TCB *tcb, u16 time) {
    tcb->time_size = time;
    tcb->remain_size = time;

    TCB *&next = up_is_running ? downlist : uplist;
    tcb->next = next;
    next = tcb;
}

__attribute__((optimize("O3")))
void DefaultScheduler::ScheduleDecision() {
    TCB *&thislist = up_is_running ? uplist : downlist;
    TCB *&nextlist = up_is_running ? downlist : uplist;

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

    ctx.idle();
    goto loop;
}

__attribute__((optimize("O3")))
[[noreturn]] void DefaultScheduler::resume() {
    ctx.switch_to(*context_local);
    __builtin_unreachable();
}

__attribute__((optimize("O3")))
[[noreturn]] void DefaultScheduler::handle_trap(unsigned code) {
    if ((code & 0x80000000u) || code == 2) {
        // exit bit set, or the existing _Sexit code 2
        exit();
    } else if (code == 0) {
        yield();
    }

    ScheduleDecision();
    resume();
}

__attribute__((optimize("O3")))
void DefaultScheduler::yield() {
    TCB *&nextlist = up_is_running ? downlist : uplist;
    // yield this tcb
    context_local->remain_size += context_local->time_size;
    context_local->next = nextlist;
    nextlist = context_local;
    context_local = nullptr;
}

__attribute__((optimize("O3")))
void DefaultScheduler::wait(TCB *tid) {
    if (sleeplist != nullptr) {
        sleeplist->prev = context_local;
    }
    context_local->next = sleeplist;
    sleeplist = context_local;
    context_local = nullptr;

    // construct the frame on the target TCB
    tid->tcb_push(TCBField::flags);
    tid->tcb_push(TCBField::ip);
    tid->tcb_push(TCBField::word, (size_t)sleeplist);
    tid->tcb_set_ip(tid->wait_handler);
}

__attribute__((optimize("O3")))
void DefaultScheduler::exit() {
    ctx.destroy(context_local);
    context_local = nullptr;
}

void DefaultScheduler::cut(TCB *tid, size_t num, void *argv) {
    tid->tcb_push(TCBField::flags);
    tid->tcb_push(TCBField::ip);
    tid->tcb_push(TCBField::word, num);
    tid->tcb_push(TCBField::word, (size_t)argv);
    tid->tcb_set_ip(tid->int_handler);
}

__attribute__((optimize("O3")))
void DefaultScheduler::wake(TCB *tid, size_t wake_num) {
    if (tid->prev) {
        // is sleeping
        tid->prev->next = tid->next;
        tid->prev = nullptr;
    } else if (tid == sleeplist) {
        sleeplist = tid->next;
    }

    TCB *&nextlist = up_is_running ? downlist : uplist;
    tid->next = nextlist;
    nextlist = tid;
    tid->tcb_push(TCBField::word, wake_num);
}

__attribute__((optimize("O3")))
void DefaultScheduler::set_time_size(TCB *tcb, u16 time) {
    tcb->time_size = time;
}

void DefaultScheduler::reg_int(TCB *tcb, void *int_handler) {
    tcb->int_handler = (size_t)int_handler;
}

void DefaultScheduler::reg_wait(TCB *tcb, void *wait_handler) {
    tcb->wait_handler = (size_t)wait_handler;
}

extern "C" [[noreturn]] void sche_c(
    DefaultScheduler *sche, unsigned code
) {
    sche->handle_trap(code);
}