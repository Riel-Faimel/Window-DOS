#ifndef __kernel_arch_x86_sche_x86_context_hpp
#define __kernel_arch_x86_sche_x86_context_hpp

#include <global/type.hpp>
#include <interface/TD/context.hpp>

struct TCB;

namespace arch {

class X86Context final : public ArchContext {
public:
    X86Context();

    TCB *create_thread(
        void *func, size_t argc, void *argv, size_t cr3, u16 ldtr,
        size_t code_seg, size_t data_seg, size_t gs, size_t fs
    );

    [[noreturn]] void switch_to(TCB &next) override;
    void idle() override;
    void destroy(TCB *tcb) override;

    void start_preemption();
    void close_preemption();
};

} // namespace arch

#endif
