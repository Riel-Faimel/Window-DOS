[bits 32]

global fresh_gdt

fresh_gdt:
    lgdt [eax]

    jmp 0x08:.next_step
.next_step:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret