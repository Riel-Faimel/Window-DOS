[bits 32]
global _ScsSwitch

STRUC TCB
    .thread_ID resd 1

;=== reg ===
    .xax resd 1
    .xbx resd 1
    .xcx resd 1
    .xdx resd 1

    .xsi resd 1
    .xdi resd 1

    .xss resd 1
    .xsp resd 1
    .xbp resd 1

    .xcs resd 1
    .xip resd 1

    .xds resd 1
    .xes resd 1

    .cr3 resd 1
    .xflag resd 1
;=== other ===

    .state resb 1
    .time_size resb 4
ENDSTRUC

;eax: from
;edx: to
_ScsSwitch:
    hlt
    jmp _ScsSwitch