[bits 32]
global _ScsSwitch, _Syield, _Ssche
extern soff, sche_c

keep_context:
    push esp
    push eax
    mov eax, esp
    mov esp, [fs:0]
    add esp, [soff]
    mov esp, [esp]
    add esp, 40
    pusha
    add esp, 12
    mov [esp], eax
    add esp, 16
    mov eax, [eax]
    push eax
    sub esp, 20
    mov esp, [esp] ;back

    pop eax ;eax
    pop eax ;esp
    pop eax ;caller code

    jmp sche_c

_Syield:
    push dword 0
    jmp keep_context

_Ssche:
    push dword 1
    jmp keep_context

;eax: tcb ptr
_ScsSwitch:
    popa ;back
    sub esp, 20
    mov esp, [esp]
    sub esp, 20
    iretd