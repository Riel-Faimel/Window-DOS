
struc TCB
    .next resd 1
    .time_size resw 1
    .remain_size resw 1

    .context resb 0
    .xdi resd 1
    .xsi resd 1
    .xbp resd 1
    ._rxsp resd 1
    .xbx resd 1
    .xdx resd 1
    .xcx resd 1
    .xax resd 1
    .context_end resb 0

    .xip resd 1
    .xcs resd 1
    .xflag resd 1
    .xsp resd 1
    .xss resd 1
    .int_frame_end resb 0

    .xfs resd 1
    .xds resd 1
    .xes resd 1
    .xgs resd 1
    .cr3 resd 1
    .ldtr resd 1
endstruc

struc Frame
    .xax resd 1
    .code resd 1
    .int_frame resb 0
    .xip resd 1
    .xcs resd 1
    .xflag resd 1
    .xsp resd 1
    .xss resd 1
endstruc
