[bits 32]

global keyboard_handler
extern keyboard_handler_c
extern Label_for_keyboard_interrput_goto
extern keyboard_this

keyboard_handler:
    call keyboard_handler_c
    mov al, 0x20
    out 0x20, al
    mov dword[esp], Label_for_keyboard_interrput_goto
    iretd