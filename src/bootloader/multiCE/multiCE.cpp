#include "_multiCE.hpp"

extern "C" { multiCE* Multi_Channel_executor; }

#pragma pack(push, 1)
struct _RZCTX {
    u16 CS;
    u32 EIP;
    u32 EAX;
    u32 ECX;
    u32 EDX;
    u32 EBX;
    u32 ESP;
    u32 EBP;
    u32 ESI;
    u32 EDI;
};
#pragma pack(pop)

__attribute__((naked))
void keep_context() {
    /**
     * int handler.
     * |------------------------------------->
     * CS EIP, EAX ECX EDX EBX ESP EBP ESI EDI
     */
    asm volatile (
        "pusha\n"
        "mov Multi_Channel_executor, %%eax\n"
        "mov %%esp, %%edx\n"
        "sub %0, %%edx\n"
        "jmp _RZmultiCE_yield\n"
        : 
        : "i"(sizeof(_RZCTX))
        : 
    );
}

multiCE::multiCE() {
    scheduler = new cs;
    scheduler->init(nullptr, 0);
    Multi_Channel_executor = this;
}

void multiCE::cut(size_t tid, size_t num) {
    scheduler->cut(tid, num);
}

void multiCE::run(void (*func)(size_t, void *), size_t argc, void *argv) {
    scheduler->run(func, argc, argv);
}

__attribute__((regparm(2)))
void multiCE::yield(_RZCTX *ctx_ptr) {
    (scheduler->*switch_ptr)(-2);
}