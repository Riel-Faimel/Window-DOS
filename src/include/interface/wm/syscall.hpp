#pragma once

#include <global/type.hpp>

/**
 * Arch-neutral syscall arguments.
 *
 * number/arg0..arg3 are the logical call number and arguments; ret/error are
 * written by the handler.  The arch entry is responsible for filling them from
 * its register frame and writing ret/error back using its own return convention.
 */
struct SyscallArgs {
    size_t number;
    size_t arg0;
    size_t arg1;
    size_t arg2;
    size_t arg3;
    size_t ret;
    bool error;
};

class SyscallHandler {
public:
    virtual ~SyscallHandler() = default;
    virtual void (*handler())() = 0;
    virtual void handle(SyscallArgs &args) = 0;
};
