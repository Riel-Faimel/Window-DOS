global outb
global outw
global outl
global inb
global inw
global inl
global io_wait

outb:
    out dx, al
    ret
outw:
    out dx, ax
    ret
outl:
    out dx, eax
    ret
inb:
    mov dx, ax
    xor eax, eax
    in al, dx
    ret
inw:
    mov dx, ax
    xor eax, eax
    in ax, dx
    ret
inl:
    mov dx, ax
    in eax, dx
    ret
io_wait:
    push ax
    push dx
    xor al, al
    mov dx, 0x80
    out dx, al
    out dx, al
    pop dx
    pop ax
    ret


global change_stack
global change_stack_back
change_stack:
    push ebx
    mov ebx, esp ;保护旧栈顶
    mov esp, eax ;切栈
    push ebx
    push ebp
    mov ebp, esp ;切帧

change_stack_back:
    pop ebp ;回帧
    pop esp ;回栈
    pop ebx ;旧ebx
