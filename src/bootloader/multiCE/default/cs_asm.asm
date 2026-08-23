[bits 32]
global _Syield, _Ssche, _Sexit, _Sint_stack, _Scall_stack
extern soff, sche_c

_Ssche:
    push dword 0
    jmp keep_context

_Syield:
    push dword 1
    jmp keep_context

_Sexit:
    push dword 2
    ;jmp keep_context 
    ; through

keep_context:
    push eax
    mov eax, esp
    ; | eax | code | ...
    ; ^ eax
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
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