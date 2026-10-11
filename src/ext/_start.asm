[bits 32]

global _start
extern kmain
extern __bss_start
extern __bss_end
extern __CTOR_LIST__
extern __CTOR_END__

jmp _start
nop

times 16 - ($ - $$) db 0
dd 0xFFFFFFFF
dd 'Riel'
dd 'WDOS'
dd 0xFFFFFFFF

_start:
    cli 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000 

    sti

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

    call kmain

    cli
.hang:
    hlt
    jmp .hang
