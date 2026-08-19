[bits 32]
global DE_handler
global OF_handler
global UD_handler
global NM_handler
global DF_handler
global GP_handler
global basic_time_handler

extern DE_handler_c
extern OF_handler_c
extern UD_handler_c
extern NM_handler_c
extern DF_handler_c
extern GP_handler_c
extern basic_time_handler_c

extern print_program_status

code2str:
    db "0123456789ACBDEF"

qemucomoutput:
    push ebx
    push dx
    mov dx, 0x3F8
    mov ebx, code2str   ; 查表基址
.next_nibble:
    rol eax, 4           ; 让最高4位先到最低4位
    push eax
    and eax, 0x0F        ; 取最低4位
    xlatb                ; al = hex_table[eax]
    out dx, al        ; 输出一个字符
    pop eax
    loop .next_nibble
    pop dx
    pop ebx
    ret

DE_handler:
    pusha
    call DE_handler_c
    mov al, 0x20
    out 0x20, al
    popa
    iretd
OF_handler:
    ;pusha
    call OF_handler_c
    mov al, 0x20
    out 0x20, al
    popa
    iretd
UD_handler:
    push eax
    mov eax, esp
    add eax, 4
    push eax
    
    call UD_handler_c
    pop eax
    pop eax
    mov al, 0x20
    out 0x20, al
    iretd

NM_handler:
    pusha
    call NM_handler_c
    mov al, 0x20
    out 0x20, al
    popa
    iretd

DF_handler:
    pusha
    call DF_handler_c
    mov al, 0x20
    out 0x20, al
    popa
    iretd

;|frame|eax|errcode|eip|cs|
GP_handler:
    push eax
    mov eax, esp
    add eax, 4
    push eax

    call GP_handler_c
    pop eax
    pop eax
    iretd

basic_time_handler:
    pusha
    call basic_time_handler_c
    mov al, 0x20
    out 0x20, al
    popa
    iretd

global catch_program
catch_program:
.hang:
    hlt
    jmp .hang

