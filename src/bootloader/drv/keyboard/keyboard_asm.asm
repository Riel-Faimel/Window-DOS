[bits 32]

global keyboard_handler
global keyboard_handler_go_back
extern keyboard_handler_c
extern Label_for_keyboard_interrput_goto
extern keyboard_this
Label_for_keyboard_interrput_goto:

    
keyboard_handler:
    call keyboard_handler_c
    jmp keyboard_handler_go_back

keyboard_handler_go_back:
    mov al, 0x20
    out 0x20, al
    mov dword[esp], Label_for_keyboard_interrput_goto
    iretd