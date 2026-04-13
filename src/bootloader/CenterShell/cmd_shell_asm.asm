[bits 32]

global do_int_with_params

int_instr:
    db 0xCD 
.int_code:
    db 0x00
    db 0xC3

section .text
; void do_int_with_params(u8 vector, u32 eax_val, u32 ebx_val, u32 ecx_val, u32 edx_val, u32 esi_val, u32 edi_val)
do_int_with_params:
    push ebp
    mov ebp, esp

    ; Load parameters (assuming C calling convention)
    ; ebp+8: vector
    ; ebp+12: eax_val
    ; ebp+16: ebx_val
    ; ebp+20: ecx_val
    ; ebp+24: edx_val
    ; ebp+28: esi_val
    ; ebp+32: edi_val

    mov al, [ebp+8]    ; vector
    ; Self-modifying code for int instruction
    mov byte [int_instr.int_code], al
    
    mov eax, [ebp+12]  ; set eax
    mov ebx, [ebp+16]
    mov ecx, [ebp+20]
    mov edx, [ebp+24]
    mov esi, [ebp+28]
    mov edi, [ebp+32]

    call int_instr

    mov esp, ebp
    pop ebp
    ret
