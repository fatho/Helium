%include "macros/macros.inc"
section .text
bits 64

;;; make ASM handlers available to @ref vmm.c
global vmm_pf_handler_asm
global vmm_gpf_handler_asm

;;; import C handlers from @ref vmm.c
extern vmm_pf_handler
extern vmm_gpf_handler

vmm_pf_handler_asm:
    call vmm_pf_handler
    iretq

vmm_gpf_handler_asm:
    call vmm_gpf_handler
    iretq
