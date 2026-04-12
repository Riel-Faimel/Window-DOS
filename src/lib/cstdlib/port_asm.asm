section .text
[bits 32]

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