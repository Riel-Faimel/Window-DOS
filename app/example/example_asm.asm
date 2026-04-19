[bits 32]
global print_call

print_call:
    mov eax, 2
    int 0x21