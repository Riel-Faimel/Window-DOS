#ifndef __kernel_multiCE_sche_context_hpp
#define __kernel_multiCE_sche_context_hpp

struct TCB;

/**
 * CPU-level context operations.
 *
 * TCB-local operations (tcb_push/tcb_set_ip) live on TCB itself.  This
 * interface is deliberately limited to switching, idling and destruction.
 */
class ArchContext {
public:
    virtual ~ArchContext() = default;

    [[noreturn]] virtual void switch_to(TCB &next) = 0;
    virtual void idle() = 0;
    virtual void destroy(TCB *tcb) = 0;
};

#endif
