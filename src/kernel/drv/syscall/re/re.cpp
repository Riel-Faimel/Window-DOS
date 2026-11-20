#include "syscall.hpp"

SYSCALL_ENTRY(renewDOSsyscall, 0x2f, renewDOS_entry, NativeEax)

void renewDOSsyscall::handle(SyscallArgs &args) {
    switch (args.number) {
    case 0x01: { // print
        kout << (char *)(args.arg0);
    } break;
    case 0x02: { // run thread
        args.ret = (size_t)Multi_Channel_executor.run(
            (void (*)(size_t, void *))(args.arg0), args.arg1,
            (void *)(args.arg2), {}, args.arg3
        );
    } break;
    default:
        args.error = true;
        break;
    }
}
