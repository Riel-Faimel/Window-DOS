[bits 32]

global DOScall_handler
extern DOScall_handler_c

DOScall_handler:
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    call DOScall_handler_c
    add esp, 24
    iret