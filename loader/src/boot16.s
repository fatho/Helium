%include "macros.inc"
section .text16
bits 16

;; import 32 bit entry point
extern boot32_ap

;;; Function:
;;;     16 bit entry point for application processors. 
;;;     The bootstrap processor entry point is implemented in boot32.s,
;;;     as multiboot compliant bootloaders switch the BSP to 32 bit protected mode.
;;;     
boot16_ap:
    cli          ; disable interrupts
    mov eax, boot16_gdt_data.pointer
    lgdt [eax]  ; load GDT register with start address of Global Descriptor Table
    mov eax, cr0 
    or al, 1     ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax
    
    call boot16_fast_a20
 
    ;; Far jump to 32 bit trampoline using the kernel_code-entry in the GDT
    ;; (0x00000000 - 0xffffffff, protected mode) 
 
    JMP boot16_gdt_data.kernel_code:boot32_ap_trampoline

boot16_fast_a20:
    ;; Enable A20 gate using fast method
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ret
    
bits 32
;;; Function:
;;;     Trampoline to 32 bit code, because 16 bit code cannot address the code above 0x100000 (1 MiB).
;;;     Jumped to by boot16_ap, jumping to boot32_ap
boot32_ap_trampoline:
    jmp boot32_ap
    
section .rodata16
 
boot16_gdt_data:
    .null:
        dq 0x0 
    .kernel_code:   equ $ - boot16_gdt_data
        dw 0xFFFF       ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 0b10011000   ; access (Present, Ring0, Segment, Execute) 
        db 0b11001111   ; flags (4 KiB limit, protected mode) and limit (16..19)
        db 0x00         ; base (24..31)
    .kernel_data:   equ $ - boot16_gdt_data
        dw 0xFFFF       ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 0b10010010   ; access (Present, Ring0, Segment, Write) 
        db 0b11001111   ; flags (4 KiB limit, protected mode) and limit (16..19)
        db 0x00         ; base (24..31)
    .pointer:
        dw $ - boot16_gdt_data - 1
        dd boot16_gdt_data
    