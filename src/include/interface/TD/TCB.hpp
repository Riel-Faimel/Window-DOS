#ifndef __include_TD_TCB_hpp
#define __include_TD_TCB_hpp

#include <global/type.hpp>

/**
 * What a TCB frame push means.  The concrete TCB maps this to its own
 * architectural frame, so the scheduler never has to know where xflag/xip
 * live.
 */
enum class TCBField : u8 {
    flags,
    ip,
    word,
};

/**
 * Generic thread control block.
 *
 * TCB itself owns only TCB-local operations.  CPU context switching, idle
 * and destruction are ArchContext operations.
 */
class TCB {
public:
    virtual void tcb_push(TCBField field, size_t value = 0) = 0;
    virtual void tcb_set_ip(size_t ip) = 0;

    TCB *next = nullptr;
    TCB *prev = nullptr;
    u16 time_size = 1;
    u16 remain_size = 1;
    size_t int_handler = 0;
    size_t wait_handler = 0;

    TCB(const TCB &) = delete;
    TCB &operator=(const TCB &) = delete;

protected:
    TCB() = default;
    ~TCB() = default;   // non-virtual: destruction goes through ArchContext
};

static_assert(sizeof(TCB) == 24);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
static_assert(__builtin_offsetof(TCB, next) == 4);
static_assert(__builtin_offsetof(TCB, prev) == 8);
static_assert(__builtin_offsetof(TCB, remain_size) == 14);
static_assert(__builtin_offsetof(TCB, int_handler) == 16);
static_assert(__builtin_offsetof(TCB, wait_handler) == 20);
#pragma GCC diagnostic pop

#endif
