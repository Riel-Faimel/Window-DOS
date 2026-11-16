[bits 32]
global _Sint_sche, _Scall_yield, _Sint_yield, _Sring0_back, _Sring3_back, _Sexit
global sche_entry
extern sche_c

%include ".\\src\\kernel\\arch\\x86\\sche\\tcb.asm"

sche_entry:
    mov ecx, [fs:4]
    push eax
    push ecx
    call sche_c
    ud2

_Sint_sche:
    cli
    push dword 0
    push eax
    mov eax, esp
    ; | eax | code | ...
    ; ^ eax
    mov esp, [fs:8]
    ;p t TCB base
    test esp, esp
    jz .iret_only
    cmp word[esp + TCB.remain_size], 0
    jbe keep_context ;.goto_sche
    ;return
    dec word[esp + TCB.remain_size]
.iret_only:
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
    mov esp, [fs:8]
keep_context:
    add esp, X86TCB.context_end
    pusha
    ; | context | eip | cs |
    ; ^ esp

    add esp, (X86TCB.ldtr - X86TCB.context) ;p t ldtr
    sldt word[esp]
    mov edi, cr3
    push edi
    push gs
    push es
    push ds
    sub esp, (X86TCB.xds - X86TCB.xsp) ;skip ss and esp
    push dword[eax + Frame.xflag] ;eflag
    push dword[eax + Frame.xcs] ;cs
    push dword[eax + Frame.xip] ;eip
    push dword[eax + Frame.xax] ;eax

    add esp, (X86TCB.int_frame_end - X86TCB.xax)
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
    pop eax ; caller code (0 = timer, 1 = yield)
    jmp sche_entry

_Sring0_back:
    push dword[eax + X86TCB.xflag] ;eflag
    push dword[eax + X86TCB.xcs] ;cs
    push dword[eax + X86TCB.xip] ;eip
    mov esp, eax
    add esp, X86TCB.xds ;p t ds
    pop ds
    pop es
    pop gs
    pop edi
    mov cr3, edi
    lldt word[esp]
    sub esp, (X86TCB.ldtr - X86TCB.context) ;p t context
    mov al, 0x20
    out 0x20, al
    popa ;resume context
    mov esp, [esp - (X86TCB.context_end - X86TCB.xsp)] ;back
    sub esp, (Frame.xsp - Frame.xip) ;frame
    iret

_Sring3_back:
    pop ds
    pop es
    pop gs
    pop edi
    mov cr3, edi
    lldt word[esp] ;p t ldtr
    sub esp, (X86TCB.ldtr - X86TCB.context) ;p t context
    mov al, 0x20
    out 0x20, al
    popa
    iret

_Sexit:
    mov eax, 2
    jmp sche_entry
