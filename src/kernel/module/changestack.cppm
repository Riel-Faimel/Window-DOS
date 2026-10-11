export module changestack;
export class ChangeStack {
public:
    inline __attribute__((always_inline)) ChangeStack(void *new_stack) noexcept {
        asm volatile (
            "push %%eax\n"
            "mov %%esp, %%eax\n"
            "mov %0, %%esp\n"
            "push %%eax\n"
            "push %%ebp\n"
            "mov %%esp, %%ebp\n"
            : 
            : "r"(new_stack)
            : "eax", "memory"
        );
    }

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