[bits 32]

global DOScall_handler
extern DOScall_handler_c

DOScall_handler:
    call DOScall_handler_c
    iret