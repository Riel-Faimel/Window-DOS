[bits 32]

global DOScall_handler
extern DOScall_handler_c

DOScall_handler:
    cli
    pusha
    mov eax, esp
    call DOScall_handler_c
    popa
    sti
    iret