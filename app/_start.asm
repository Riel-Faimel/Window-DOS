[bits 32]

global _start
extern main
extern __bss_start
extern __bss_end

extern __CTOR_LIST__
extern __CTOR_END__
section .sys
__sys_bit:
    dd 32
__sys_base_addr:
    dd 0
__sys_enter_point:
    dd _start
__sys_code_sec:
    dw 0
__sys_data_sec:
    dw 0

section .text
_start:
    cli 
    mov ax, [__sys_data_sec]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FFFE

    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi

    xor eax, eax
    cld
    rep stosb
    
    mov esi, __CTOR_LIST__
    add esi, 4
.ctor_loop:
    mov eax, [esi]
    test eax, eax
    jz .ctor_done

    call eax

    add esi, 4
    jmp .ctor_loop
.ctor_done:

    sti

    call main

    mov eax, 0x4C
    int 0x21
    cli
.hang:
    hlt
    jmp .hang
