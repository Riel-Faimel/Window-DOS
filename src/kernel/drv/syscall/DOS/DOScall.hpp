#ifndef __kernel_syscall_DOS_DOScall_hpp
#define __kernel_syscall_DOS_DOScall_hpp

#include <interface/syscall.hpp>

class DOScall : public SyscallHandler {
public:
    DOScall() = default;
    ~DOScall() = default;

    void (*handler())() override;
};

#endif