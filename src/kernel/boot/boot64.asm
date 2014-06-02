%include "macros/macros.inc"
section .boot
bits 64

;; export 64 bit entry points
global boot64_bsp
global boot64_ap

;; import C code entry points
extern main_bsp_base
extern main_ap_base

virt_base: dq 0xFFFFFFFF80000000

boot64_bsp:
    mov rax, main_bsp_base
    add rax, virt_base
    call rax
    
    halt
    
boot64_ap:
    mov rax, main_ap_base
    add rax, virt_base
    call rax
    
    halt
