[bits 32]

global keyboard_handler
extern keyboard_handler_c
    
keyboard_handler:
    call keyboard_handler_c
    mov al, 0x20
    out 0x20, al
    iretd