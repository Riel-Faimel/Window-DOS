.syntax unified
.thumb

.global PendSV_Handler
.extern current_tcb
.extern next_tcb

.equ TCB_FRAME_SP, 24

/**
 * Cortex-M7 PendSV context switch.
 *
 * Thread context:
 *   psp -> r4..r11 (saved by this handler)
 *           -> r0..r3/r12/lr/pc/xpsr (saved by hardware on exception entry)
 *
 * current_tcb / next_tcb are TCB pointers.  TCB_FRAME_SP is the offset of
 * M32H750Frame::sp inside M32H750TCB (TCB is 24 bytes, frame follows).
 */
PendSV_Handler:
    mrs     r0, psp

    ldr     r1, =current_tcb
    ldr     r2, [r1]
    cbz     r2, 1f

    stmdb   r0!, {r4-r11}
    str     r0, [r2, #TCB_FRAME_SP]

1:
    ldr     r3, =next_tcb
    ldr     r2, [r3]
    cbz     r2, 2f

    str     r2, [r1]                 @ current_tcb = next_tcb
    ldr     r0, [r2, #TCB_FRAME_SP]
    ldmia   r0!, {r4-r11}
    msr     psp, r0

2:
    bx      lr
