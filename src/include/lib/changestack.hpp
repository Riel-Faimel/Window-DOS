#ifndef __lib_changestack_hpp
#define __lib_changestack_hpp

/**
 * Switch to a temporary stack for the lifetime of this object.
 *
 * x86 stacks grow down, so `stack_top` must point one past the last usable
 * byte.  Prefer the (base, size) overload below: it computes the top here,
 * so a caller cannot pass the base by mistake.
 *
 * ecx is used to hold the new top: eax is busy saving the old ESP, so
 * constraining the operand keeps the compiler from putting it in eax
 * (which would be clobbered by the save).
 */
class ChangeStack {
    inline __attribute__((always_inline)) ChangeStack(void *stack_top) noexcept {
        asm volatile (
            "push %%eax\n"
            "mov %%esp, %%eax\n"
            "mov %%ecx, %%esp\n"
            "push %%eax\n"
            "push %%ebp\n"
            "mov %%esp, %%ebp\n"
            :
            : "c"(stack_top)
            : "eax", "memory"
        );
    }

public:
    inline __attribute__((always_inline)) ChangeStack(void *stack_base, unsigned size) noexcept
        : ChangeStack(static_cast<char *>(stack_base) + size) {}

    inline __attribute__((always_inline)) ~ChangeStack() noexcept {
        asm volatile (
            "pop %%ebp\n"
            "pop %%esp\n"
            "pop %%eax\n"
            :
            :
            : "eax", "memory"
        );
    }
};

#endif
