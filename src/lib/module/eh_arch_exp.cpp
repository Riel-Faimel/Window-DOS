#include "eh_frame.hpp"

// 与架构绑定的两段入口：保存当前上下文、跳到 landing pad。

extern "C" {
u32 eh_landing = 0;
u32 eh_exc = 0;
u32 eh_sel = 0;
}

__attribute__((naked, noreturn)) void eh_install(EhContext *) {
    asm(
        "mov 4(%esp), %eax\n"
        "mov 12(%eax), %esi\n"
        "mov 16(%eax), %edi\n"
        "mov 4(%eax), %ebp\n"
        "mov 8(%eax), %ebx\n"
        "mov eh_landing, %ecx\n"
        "mov 0(%eax), %esp\n"
        "mov eh_exc, %eax\n"
        "mov eh_sel, %edx\n"
        "jmp *%ecx\n"
    );
}

extern "C" __attribute__((naked)) void eh_capture(void *) {
    asm (
        "mov 4(%esp), %eax\n"
        "lea 4(%esp), %ecx\n"
        "mov %ecx, 0(%eax)\n"
        "mov %ebp, 4(%eax)\n"
        "mov %ebx, 8(%eax)\n"
        "mov %esi, 12(%eax)\n"
        "mov %edi, 16(%eax)\n"
        "mov (%esp), %ecx\n"
        "mov %ecx, 20(%eax)\n"
        "ret\n"
    );
}
