[bits 32]
global _Syield, _Ssche, _Sexit, _Sint_stack, _Scall_stack
extern soff, sche_c

_Ssche:
    push dword 0
    push eax
    pushf
    mov eax, esp
    ; | eflag | eax | code | ...
    ; ^ eax
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
    cmp word[esp + 4], 0
    jbe .goto_sche
    ;return
    dec word[esp + 4]
    mov esp, eax
    pop eax
    popf
    add esp, 4
    iret

.goto_sche:
    xchg eax, esp
    popf
    xchg eax, esp
    jmp keep_context

_Syield:
    push dword 1
    ;foll through

get_tcb:
    push eax
    mov eax, esp
    ; | eax | code | ...
    ; ^ eax
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
keep_context:
    add esp, 40
    pusha
    ; | context | eip | cs |
    ; ^ esp

    add esp, 44 ;p t eflag
    add eax, 12
    ; | eax | code | eip | cs |
    ;                    ^ eax
    push dword[eax + 4] ;eflag
    push dword[eax] ;cs
    push dword[eax - 4] ;eip
    push dword[eax - 12] ;eax

    add esp, 24 ;p t ss
    mov ebx, [eax] ;cs
    and ebx, 3
    cmp ebx, 0
    jne .int_stack
.call_stack:
    ; | eip | cs | eflag |
    ;       ^ eax
    add eax, 8
    push 0x10
    push eax
    sub eax, 16
    jmp .done

.int_stack:
    ; | eip | cs | eflag | esp | ss |
    ;       ^ eax
    push dword[eax + 12]
    push dword[eax + 8]
    sub eax, 8

.done:
    ; | eax | code | ...
    ;       ^ eax
    mov esp, eax
    pop eax ;caller code

    ; interrupt context, we don't care 
    ; because we don't return
    mov ebp, esp ; fresh env for c func
    jmp sche_c

_Sexit:
    push dword 2
    jmp get_tcb 

_Sint_stack:
    popa
    iret

_Scall_stack:
    popa ;back
    pop eax ;eip
    pop edx ;cs
    pop ecx ;eflags
    pop esp ;back
    push ecx
    push edx
    push eax
    iret