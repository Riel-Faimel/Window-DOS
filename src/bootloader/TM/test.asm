global _Shello, _Snext, _Sok
extern hello, next, _Cok, _Ssche

_Shello:
    call hello
    pushf
    push cs
    call _Ssche
    jmp _Shello

_Snext:
    call next
    int 48
    jmp _Snext

_Sok:
    call _Cok
    pushf
    push cs
    call _Ssche
    jmp _Sok