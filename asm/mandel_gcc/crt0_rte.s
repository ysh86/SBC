    .section .text.start
    .global _start
    .extern main

_start:
    move.l  #_ssp_top, %sp      /* supervisor stack */

    lea     _bss_start, %a0
    lea     _bss_end, %a1

clear_bss:
    cmpa.l  %a1, %a0
    bhs     bss_done
    clr.b   (%a0)+
    bra     clear_bss

bss_done:
    move.l  #_usp_top, %a0
    move.l  %a0, %usp           /* user stack pointer */

    move.w  #0x0000, -(%sp)     /* user mode SR */
    move.l  #user_start, -(%sp) /* user mode PC */
    rte

user_start:
    jsr     main

exit_to_monitor:
    illegal                 /* opcode 0x4AFC: monitor break */

halt:
    bra     halt

    .section .note.GNU-stack,"",@progbits
