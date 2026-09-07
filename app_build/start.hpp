#ifndef __runtime_start
#define __runtime_start

extern int main();
extern "C" void _start();

#pragma pack(push, 1)

namespace {

using __global_constructor = void (*)();
using __global_destructor = void (*)();

} // namespace 

extern "C" {
    extern __global_constructor __init_array_start[];
    extern __global_constructor __init_array_end[];
    extern __global_destructor __fini_array_start[];
    extern __global_destructor __fini_array_end[];
}

inline void *__dso_handle __asm__("__dso_handle") __attribute__((visibility("hidden"))) = &__dso_handle;

extern "C" {

__attribute__((used))
inline int __cxa_atexit(void (*) (void *), void *, void *) { return 0; }

__attribute__((used, section(".text"))) 
inline void _start() {
    for(auto ctor = __init_array_start; ctor != __init_array_end; ++ctor) {
        (*ctor)();
    }
    for(auto dtor = __fini_array_start; dtor != __fini_array_end; ++dtor) {
        (*dtor)();
    }
    int re = main();
};


}

#pragma pack(pop)

#endif