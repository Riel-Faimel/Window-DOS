[bits 32]

global ps2_handler
extern ps2_handler_c
    
ps2_handler:
    pusha
    cld
    call ps2_handler_c
    popa
    mov al, 0x20
    out 0x20, al
    iretd