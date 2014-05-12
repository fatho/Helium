%include "macros.inc"
section .text
bits 64

;; export 64 bit entry points
global boot64_bsp
global boot64_ap

;; import C code entry points
extern main_bsp
extern main_ap

boot64_bsp:
    call main_bsp
    
    halt
    
boot64_ap:
    call main_ap
    
    halt