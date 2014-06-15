%include "macros/macros.inc"
section .text
bits 64

global vmm_pf_handler_asm
extern vmm_pf_handler

vmm_pf_handler_asm:
    magicbreak
    call vmm_pf_handler
    iretq
