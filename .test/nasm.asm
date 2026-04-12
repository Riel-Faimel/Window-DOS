[org 0x7C00]
[bits 16]
jmp _start
nop

_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    mov [INFO.boot_device], dl

    cli 

.VBE_mode:
    mov ax, 0x4F00 ;controller info
    mov di, 0x9000
    int 0x10
    cmp ax, 0x004F
    jne .VGA_mode
    ;0x9000 all mode
    ;+0x0E for section
    ;+0x10 for offset

    mov ax, [0x9000+0x0E]
    mov es, ax
    test ax, 0x01
    jz .mode_local

    xor di, di
    jmp .find_mode
.mode_local:
    mov di, [0x9000+0x10]

.find_mode:
    add di, 2
    mov cx, [di-2]
    cmp cx, 0xFFFF
    je .find_done

    mov ax, 0x4F01 ;mode info
    push di
    mov di, 0x9000
    int 0x10
    pop di

    cmp ax, 0x004F
    jne .find_mode

    test word[es:0x9000], 0x80
    jz .find_mode

.find_done:
    cmp cx, 0xFFFF
    je .VGA_mode

    mov bx, cx
    or bx, 0x4000 ;mode with mmap
    mov ax, 0x4F02
    int 0x10 ;open mode

    mov eax,  [0x9000+0x28]
    mov dword[Vedio_base.buffer_base], eax
    mov ax, [0x9000+0x12]
    mov word[Vedio_base.width], ax
    mov ax, [0x9000+0x14]
    mov word[Vedio_base.height], ax
    mov al, [0x9000+0x19]
    mov byte[Vedio_base.bpp], al

    mov al, [Display_mode.attr]
    or al, 0x01
    mov [Display_mode.attr], al
    jmp .screen_done

.VGA_mode:
    mov ax, 0x0013
    int 0x10
    jmp .screen_done

.screen_done:
    
    mov dl, [INFO.boot_device]
    mov si, dap
    mov ah, 0x42
    int 0x13

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
    lidt [_IDT]

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

_IDT:
    dw 255
    dd 0x8000

times 320-($-$$) db 0
Display_mode:
    .attr db 0
Vedio_base:
    .buffer_base dd 0
    .width dw 0
    .height dw 0
    .bpp db 0
INFO:
    .boot_device db 0
    .mmap_size db 0

num:
    db '0123456789ABCDEF'
dap:
    db 0x10
    db 0
    .count: dw 20
    .offset: dw 0
    .segment: dw 0x1000
    .lba_low: dd 1
    .lba_high: dd 0

times 510-($-$$) db 0
dw 0xAA55