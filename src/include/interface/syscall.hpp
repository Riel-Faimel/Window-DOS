#pragma once

class SyscallHandler {
    using Handle_f = void (*)();
public:
    virtual Handle_f handler() = 0;
};