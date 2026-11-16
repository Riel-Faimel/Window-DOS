# H750 Cortex-M7 context switch sketch

This directory is an architecture sketch, not a wired build target.

## Model

- `M32H750TCB : TCB` adds `M32H750Frame`.
- `M32H750Frame::sp` is the PSP anchor:
  - `sp -> r4..r11`
  - `sp + 32 -> r0..r3/r12/lr/pc/xpsr` (hardware exception frame)
- `M32H750Context : ArchContext` implements only the CPU-level operations.
- `DefaultScheduler` still sees only `TCB*` and `ArchContext&`.

## Switch

`switch_to()` sets `next_tcb` and pends PendSV. PendSV:

1. reads PSP
2. saves `r4-r11` on the current thread stack
3. stores the new PSP in `current_tcb->frame.sp`
4. loads `next_tcb`
5. restores `r4-r11`
6. sets PSP to the hardware frame
7. `bx lr` — exception return pops `r0-r3/r12/lr/pc/xpsr`

The first thread has no `current_tcb`; the handler skips the save and starts
by restoring `next_tcb`.

## Not finished

- `SysTick_Handler` does not call the generic scheduler tick yet.
- `tcb_push` is a placeholder; Cortex-M `wait/cut` need a hardware-frame
  builder rather than x86-style stack pushes.
- FPU context (`s0-s15`, `fpscr`) is not saved.
- Privilege/MPU setup and the initial PSP/MSP handoff are not included.
