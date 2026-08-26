[bits 32]
global _Sint_sche, _Scall_yield, _Sint_yield, _Sring0_back, _Sring3_back, _Sexit
extern soff, sche_c

%include "E:\\texmanim\\cpp\\Window-DOS\\src\\bootloader\\multiCE\\default\\tcb.asm"

_Sint_sche:
    cli
    push dword 0
    push eax
    mov eax, 0x30
    mov fs, eax
    mov eax, esp
    ; | eax | code | ...
    ; ^ eax
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
    ;p t TCB base
    cmp word[esp + TCB.remain_size], 0
    jbe keep_context ;.goto_sche
    ;return
    dec word[esp + TCB.remain_size]
    mov esp, eax
    mov al, 0x20
    out 0x20, al
    pop eax
    add esp, 4
    iret

;thread no need to call just for sche
;so only _Sint_sche

_Scall_yield:
    ; | eip |
    ; ^ esp
    sub esp, Frame.xflag - Frame.int_frame
    push dword 1
    push eax
    mov eax, 0x30
    mov fs, eax
    mov eax, [esp + Frame.xflag]
    mov [esp + Frame.xip], eax
    add esp, Frame.xflag
    pushf
    push cs
    jmp get_tcb 

_Sint_yield:
    push dword 1
    push eax
    mov eax, 0x30
    mov fs, eax
    mov eax, esp
    ; | eax | code | ...
    ; ^ eax
    ;foll through

get_tcb:
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
keep_context:
    add esp, TCB.context_end
    pusha
    ; | context | eip | cs |
    ; ^ esp

    add esp, (TCB.ldtr - TCB.context) ;p t ldtr
    sldt word[esp]
    mov edi, cr3
    push edi
    push gs
    push es
    push ds
    sub esp, (TCB.xds - TCB.xsp) ;skip ss and esp
    push dword[eax + Frame.xflag] ;eflag
    push dword[eax + Frame.xcs] ;cs
    push dword[eax + Frame.xip] ;eip
    push dword[eax + Frame.xax] ;eax

    add esp, (TCB.int_frame_end - TCB.xax)
    ;next push is xss
    mov ebx, [eax + Frame.xcs] ;cs
    and ebx, 3
    cmp ebx, 0
    jne .ring3_stack
.ring0_stack:
    ; | eip | cs | eflag |
    ; ^ eax
    push dword 0x10
    add eax, Frame.xsp
    push eax
    sub eax, (Frame.xsp - Frame.code) ;p t code
    jmp .done

.ring3_stack:
    ; | eip | cs | eflag | esp | ss |
    ; ^ eax
    push dword[eax + Frame.xss] ;ss
    push dword[eax + Frame.xsp] ;esp
    add eax, Frame.code ;p t code

.done:
    ; | eax | code | ...
    ;       ^ eax
    mov esp, eax
    pop eax ;caller code

    ; interrupt context, we don't care 
    ; because we don't return
    jmp sche_c

_Sring0_back:
    push dword[eax + TCB.xflag] ;eflag
    push dword[eax + TCB.xcs] ;cs
    push dword[eax + TCB.xip] ;eip
    mov esp, eax
    add esp, TCB.xds ;p t ds
    pop ds
    pop es
    pop gs
    pop edi
    mov cr3, edi
    lldt word[esp]
    sub esp, (TCB.ldtr - TCB.context) ;p t context
    mov al, 0x20
    out 0x20, al
    popa ;resume context
    mov esp, [esp - (TCB.context_end - TCB.xsp)] ;back
    sub esp, (Frame.xsp - Frame.xip) ;frame
    iret

_Sring3_back:
    pop ds
    pop es
    pop gs
    pop edi
    mov cr3, edi
    lldt word[esp] ;p t ldtr
    sub esp, (TCB.ldtr - TCB.context) ;p t context
    mov al, 0x20
    out 0x20, al
    popa
    iret

_Sexit:
    mov eax, 2
    jmp sche_c