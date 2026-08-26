[bits 32]

global DOScall_handler
extern DOScall_handler_c

DOScall_handler:
    pusha
    cli
    mov eax, esp
    call DOScall_handler_c
    popa
    iret