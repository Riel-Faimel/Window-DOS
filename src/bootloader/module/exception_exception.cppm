module;

#include <global/type.hpp>
#include <drv/screen/screen_srv.hpp>

export module exception;

export extern "C" {

extern void *__eh_frame_start __asm__("__eh_frame_start");
extern void *__eh_frame_end __asm__("__eh_frame_end");

struct __cxa_exception { 
    using terminate_handler = void (*)();

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
void eh_frame_tranveser(__cxa_exception *, void *obj) {
    cout << "start tranvesing\n";
    cout << "eh_frame in: " << __eh_frame_start << " : " << __eh_frame_end << '\n';
    cout << *(const char **)obj;
    while(1);
}

__attribute__((used, noreturn))
void __cxa_throw(void * thrown_exception, void * /*all catch*/, void (* /*dest*/) (void *)) {
    __cxa_exception *header = ((__cxa_exception *) thrown_exception - 1);
    eh_frame_tranveser(header, thrown_exception);
}

__attribute__((used))
void *__cxa_begin_catch (void *) noexcept {
    return nullptr;
}
__attribute__((used))
void __cxa_end_catch () noexcept {}

__attribute__((used))
void __gxx_personality_v0() {}

__attribute__((used))
void _Unwind_Resume() {}

}

asm (
    ".globl _ZTIi\n"
    ".globl _ZTIPKc\n"
    ".globl _ZTIPc\n"
    ".globl _ZTIPi\n"
    ".globl _ZTIl\n"
    ".globl _ZTIPl\n"
    ".globl _ZTIj\n"
    ".globl _ZTIPj\n"
    ".globl _ZTIb\n"
    ".globl _ZTIPb\n"
    ".globl _ZTIPv\n"
    "_ZTIi:\n"
    "_ZTIPKc:\n"
    "_ZTIPc:\n"
    "_ZTIPi:\n"
    "_ZTIl:\n"
    "_ZTIPl:\n"
    "_ZTIj:\n"
    "_ZTIPj:\n"
    "_ZTIb:\n"
    "_ZTIPb:\n"
    "_ZTIPv:\n"
);