%include "macros/macros.inc"
section .text.boot
bits 64

;; export 64 bit entry points
global boot64_bsp
global boot64_ap

extern multiboot_info
extern stack_bottom
extern boot32_gdtr2
;; import C code entry points
extern main_bsp
extern main_ap

;;; @brief 64 bit entry point for the bootstrap processor.
;;;
;;; @param ebp Pointer to the multiboot structure.
boot64_bsp:
    magicbreak

    ;; load 64 Bit higher half GDT
    ;; no need to reload segment registers, because GDT contents are unchanged
    mov eax, boot32_gdtr2
    lgdt [eax]

    ;; use higher half stack mapping
    add rsp, [rel kernel_vma_offset]

    ;; user higher half mapping of multiboot structure
    add rbx, [rel kernel_vma_offset]
    ;; store in multiboot_info var
    mov rax, qword multiboot_info
    mov [rax], rbx

    ;; 64 bit call
    mov rax, qword main_bsp
    call rax
    
    halt
    
boot64_ap:
    mov rax, qword main_ap
    call rax
    
    halt

kernel_vma_offset: dq KERNEL_VMA
