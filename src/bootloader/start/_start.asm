[bits 32]

global _start
extern LoaderMain
extern __bss_start
extern __bss_end

extern __CTOR_LIST__
extern __CTOR_END__

section .start
_start:
    cli 
    mov ax, 0x10
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

    ;jmp .ctor_done
    
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

    call LoaderMain

    cli
.hang:
    hlt
    jmp .hang
