[org 0x7C00]
[bits 16]
jmp boot
nop

boot:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    mov [INFO.boot_device], dl

    cli 
    
    mov si, dap
    mov ah, 0x42
    int 0x13
    mov si, dap_1
    mov ah, 0x42
    int 0x13
    jnc .loader_done

    mov bx, 0x1000

    mov ah, 0x02
    mov al, 32
    mov ch, 0
    mov cl, 1
    mov dh, 0
    int 0x13

    mov bx, 0x1400

    mov ah, 0x02
    mov al, 31
    mov ch, 0
    mov cl, 33
    mov dh, 0
    int 0x13
.loader_done:

    mov ax, 0x0003
    int 0x10

    ;A20 address bus
    in al, 0x92
    or al, 2
    out 0x92, al

    lgdt [_GDT]

    ;protect mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:promode

[bits 32]
promode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ;jmp .hang
    jmp 0x10000

.hang:
    hlt
    jmp .hang

gdt:
    dq 0x0000000000000000
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff

_GDT:
    dw $ - gdt - 1
    dd gdt

times 320-($-$$) db 0
Display_mode:
    .attr db 0
Vedio_base:
    .buffer_base dd 0
    .bpp db 0
    .width dw 0
    .height dw 0
INFO:
    .boot_device db 0
    .mmap_size db 0

num:
    db '0123456789ABCDEF'
dap:
    db 0x10
    db 0
    .count: dw 32
    .offset: dw 0
    .segment: dw 0x1000
    .lba_low: dd 1
    .lba_high: dd 0
dap_1:
    db 0x10
    db 0
    .count: dw 32
    .offset: dw 0
    .segment: dw 0x1400
    .lba_low: dd 33
    .lba_high: dd 0

times 512-48-($-$$) db 0
PART:

times 510-($-$$) db 0
dw 0xAA55