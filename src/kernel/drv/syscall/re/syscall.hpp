#define ArchitectureDetail
#include <arch/cpu.hpp>
#include <stdio>
#include <multiCE/Multi-Channel_exec.hpp>

struct renewDOSsyscall {
    static void handle(SyscallArgs &args);
};
