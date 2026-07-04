    .section .text.start
    .global _start
    .extern main

_start:
    /* ここに来た時点で user mode、USP 設定済み */

    lea     _bss_start, %a0
    lea     _bss_end, %a1

clear_bss:
    cmpa.l  %a1, %a0
    bhs     bss_done
    clr.b   (%a0)+
    bra     clear_bss

bss_done:
    jsr     main

exit_to_monitor:
    illegal                 /* opcode 0x4AFC: monitor break */

halt:
    bra     halt

    .section .note.GNU-stack,"",@progbits
