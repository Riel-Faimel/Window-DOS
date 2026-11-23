#pragma GCC optimize("Os")

#include "eh_frame.hpp"
#include <GEH/geh.hpp>
#include <exception>
#include <global/new.hpp>

// Itanium C++ ABI 的最小实现：异常对象、__cxa_throw、以及驱动展开器的循环。

void *eh_thrown_type = nullptr;

extern "C" {

struct __cxa_exception {
    using terminate_handler = void (*)(GlobalExceptionType);

    void (*exceptionDestructor) (void *);
    terminate_handler	terminateHandler;
    __cxa_exception *	nextException;

    int	handlerCount;
    int	handlerSwitchValue;
    const char *actionRecord;
    const char *languageSpecificData;
    void *catchTemp;
    void *adjustedPtr;
};

__attribute__((used))
void *__cxa_allocate_exception(size_t) noexcept {
    return (void *)0x7c00;
}
__attribute__((used))
void __cxa_free_exception(void *) noexcept {}

__attribute__((used, noreturn))
void eh_frame_tranveser(__cxa_exception *header, void *obj) {
    EhContext ctx;
    eh_capture(&ctx);

    EhContext probe = ctx;
    for (u32 i = 0; i < 64; ++i) {
        u32 pc = probe.eip - 1;

        u32 lsda = 0;
        u32 func = 0;
        if (eh_lookup(pc, lsda, func) && lsda) {
            u32 landing = 0;
            u32 selector = 0;
            s32 offset = 0;
            if (eh_lsda_match((const u8 *)lsda, func, pc, eh_thrown_type,
                              landing, selector, offset)) {
                eh_landing = landing;
                eh_exc = (u32)obj + offset;
                eh_sel = selector;
                eh_install(&probe);
            }
        }

        if (!eh_unwind_step(probe)) break;
    }

    // 展开失败（走到没有 FDE 的栈顶）：交给异常自己的 terminate_handler
    if (header && header->terminateHandler)
        header->terminateHandler(GlobalExceptionType::UnwindBreak);
    while(1);
}

__attribute__((used, noreturn))
void __cxa_throw(void * thrown_exception, void * tinfo, void (* /*dest*/) (void *)) {
    __cxa_exception *header = ((__cxa_exception *) thrown_exception - 1);
    eh_thrown_type = tinfo;
    header->terminateHandler = terminate;
    eh_frame_tranveser(header, thrown_exception);
}

__attribute__((used))
void *__cxa_begin_catch (void *exc) noexcept {
    return exc;
}
__attribute__((used))
void __cxa_end_catch () noexcept {}

__attribute__((used))
void __gxx_personality_v0() {}

__attribute__((used))
void _Unwind_Resume() {}

[[noreturn]]
void __cxa_throw_bad_array_new_length() { __builtin_unreachable(); }

}

namespace std {

bad_cast::~bad_cast() {}
bad_typeid::~bad_typeid() {}

} // namespace std

extern "C" [[noreturn]] void __cxa_bad_cast() {
    void *mem = __cxa_allocate_exception(sizeof(std::bad_cast));
    std::bad_cast *e = new (mem) std::bad_cast();
    __cxa_throw(e, (void *)&typeid(std::bad_cast), nullptr);
}

extern "C" [[noreturn]] void __cxa_bad_typeid() {
    void *mem = __cxa_allocate_exception(sizeof(std::bad_typeid));
    std::bad_typeid *e = new (mem) std::bad_typeid();
    __cxa_throw(e, (void *)&typeid(std::bad_typeid), nullptr);
}
