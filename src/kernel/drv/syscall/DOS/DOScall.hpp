#include <global/type.hpp>
#include <stdio>
#define ArchitectureDetail
#include <arch/cpu.hpp>
#include <id/id.hpp>
#include <syscall/thread>
#include <interface/hardev/drvhead.hpp>

struct DOScall {
    static void handle(SyscallArgs &args);
};
