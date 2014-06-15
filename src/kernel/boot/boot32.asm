%include "macros/macros.inc"
;;; Macros
%macro panic 1 
   mov esi,%1
   jmp boot32_panic
%endmacro
;;; Code
section .text.boot
bits 32

;; export 32 bits entry points
global boot32_bsp
global boot32_ap
global boot32_gdtr2

;; import 64 bit entry points
extern boot64_bsp
extern boot64_ap

;; symbols from linker.ld
extern page_pml4t
extern page_id_pdpt
extern page_id_pdt
extern page_high_pdpt
extern page_phys_pdt
extern page_kernel_pdt
extern gdt_data
extern stack_bottom

;;; Function: 
;;;   Initializes the BSP (bootstrap processor), switches to long mode and loads the kernel.
;;;   This function is invoked by the bootloader.
boot32_bsp:
    cli                         ; clear interrupts until we have IDT
    mov   esp, stack_bottom - KERNEL_VMA   ; setup stack (lower half)
    mov   ebp, esp              ; setup stack frame
    push ebx                    ; save multiboot pointer

    ;; check if cpuid-instruction is supported
    call  has_cpuid
    cmp   eax, 0
    jz    .no_cpuid
   
    ;; check if extended CPUID functions are supported
    mov   eax, 0x80000000       ; returns highest extended function id
    cpuid 
    cmp   eax, 0x80000001       ; is 0x80000001 supported?
    jb    .no_longmode

    ;; query AMD64 support
    mov   eax, 0x80000001       ; Extended Function 8000_0001h: 
    cpuid                       ;   Processor Signature and AMD Features
    test  edx, 1 << 29          ; edx bit 29: Long Mode
    jz    .no_longmode

    call boot32_copy_gdt
    call boot32_identity_map
    call boot32_prepare
    
    pop ebx
    ;; Setup data segment selectors
    mov ax, boot32_gdt_data.kernel_data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;; far jump to 64 bit code
    jmp boot32_gdt_data.kernel_code:boot64_bsp
    
    halt  
    
    ;; ERROR-Handlers
.no_cpuid:
    panic msg_nocpuid

.no_longmode:
    panic msg_nolongmode

.multiboot_save: dw 0

;;; Function:
;;;     32 bit entry point for application processors. 
;;;     Called by boot16_ap in boot16.s.
boot32_ap:
    ;; Assume that this AP supports long mode when the BSP does
    
    ;; TODO: Setup stack!!!

    ;; this uses the same GDT and the same PML4T as the BSP
    ;; the PML4T has to be changed after switch to long mode.
    call boot32_prepare
    
    ;; Setup data segment selectors
    mov ax, boot32_gdt_data.kernel_data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ;; far jump to 64 bit code
    jmp boot32_gdt_data.kernel_code:boot64_ap
    
    halt

;;; @brief copies the GDT from .rodata to \c gdt_data in \c .info
;;;
boot32_copy_gdt:
    mov esi, boot32_gdt_data
    mov edi, gdt_data
    mov ecx, boot32_gdt_data.end - boot32_gdt_data
    rep movsb
    ret

;;; Function:
;;;     Initializes 64 Bit page tables with identity mapping for the first 32 MiB minus 4 KiB.
;;;     The virtual address 0x1FFF000 is used as scratch space for temporary mappings.
;;;     Uses the space reserved in linker.ld for the page tables.
boot32_identity_map:
    ;; initializes first entries of the first three tables
    ;; set flags to 0x3 = 0b11 means: Read/Write and Present

    ;; low mapping
    mov   eax, page_id_pdpt
    or    eax, 0x3
    mov   DWORD [page_pml4t], eax

    mov   eax, page_id_pdt
    or    eax, 0x3
    mov   DWORD [page_id_pdpt], eax

    mov   DWORD [page_id_pdt], 0x83

    ;; high mapping
    mov   eax, page_high_pdpt
    or    eax, 0x3
    mov   DWORD [page_pml4t+0xFF8], eax

    ;; first 4GB to FFFFFF80 00000000
    mov   eax, page_phys_pdt
    or    eax, 0x3
    mov   edi, page_high_pdpt
    mov   ecx, 4
    cld
.set_high_pdpt_phys: ;; set entries for the 4 PDTs in PDPT
    mov   DWORD [edi], eax
    add   edi, 0x8
    add   eax, 0x1000
    loop .set_high_pdpt_phys

    mov   eax, 0x83
    mov   edi, page_phys_pdt
    mov   ecx, 4 * 512
.set_phys_pdts: ;; set entries of 4 PDTs
    mov   DWORD [edi], eax
    add   edi, 0x8
    add   eax, 0x200000 ;; 2 MiB pages
    loop .set_phys_pdts


    ;; kernel to FFFFFFFF 80000000
    mov   eax, page_kernel_pdt
    or    eax, 0x3
    mov   DWORD [page_high_pdpt+0xFF0], eax

    mov   eax, 0x83
    mov   edi, page_kernel_pdt
    mov   ecx, 1 ;; only 2 MiB kernel for now, but who knows...
.set_kernel_pdts
    mov   DWORD [edi], eax
    add   eax, 0x200000 ;; 2 MiB pages
    add   edi, 0x8
    loop .set_kernel_pdts

    ret

;;; Function:
;;;     * enables PAE
;;;     * sets CR3 to location of PML4T
;;;     * loads GDT
;;;     * enables long mode
;;;     * enables paging
boot32_prepare:
    ;; enable the following features in CR4
    ;;  * PGE (bit 7)
    ;;  * PAE (bit 5)
    ;;  * PSE (bit 4)
    ;; ==> 0b 1011 0000 = 0x B0
    mov   eax, cr4
    or    eax, 0x000000B0
    mov   cr4, eax

    ;; set Long Mode bit
    mov   ecx, 0xC0000080       ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                       ; Read from the model-specific register.
    or    eax, 1 << 8           ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                       ; Write to the model-specific register.

    ;; tell CPU the location of the PML4T
    mov   eax, page_pml4t
    mov   cr3, eax
    
    ;; enable paging
    mov   eax, cr0
    or    eax, 1 << 31          ; Set the PG-bit, which is the 32nd bit (bit 31).
    mov   cr0, eax
    
    ;; load 64 Bit lower half GDT
    mov eax, boot32_gdtr1
    lgdt [eax]

    ret

;;; Description:
;;;    Checks for presence of the CPUID instruction. Recommended by the AMD64
;;;    instruction reference.
;;;
;;; Parameters: none
;;; Returns: 0 or 1 in EAX
has_cpuid:
    pushfd                      ; save EFLAGS
    pop   eax                   ; store EFLAGS in EAX
    mov   ebx, eax              ; save in EBX for later testing
    xor   eax, 00200000h        ; toggle bit 21
    push  eax                   ; push to stack
    popfd                       ; save changed EAX to EFLAGS
    pushfd                      ; push EFLAGS to TOS
    pop   eax                   ; store EFLAGS in EAX
    cmp   eax, ebx              ; see if bit 21 has changed
    jz    .no_cpuid             ; if no change, no CPUID
    mov   eax, 1                ; return true
    ret   
.no_cpuid:
    mov   eax, 0                ; return false
    ret   

;;; Description:
;;;    Outputs an error message and halts.
;;;
;;; Parameters:
;;;    ESI: pointer to error message
boot32_panic:
    mov   edi, 0xB8000          ; write to VGA memory
    mov   eax, 0x1F00           ; blue background
    mov   ecx, 80*25            ; 80*25 word-entries in VGA text memory
    cld   
    rep   stosw                 ; clear VGA memory
    mov   edi, 0xB8000
    cld   
.copy:
    lodsb                       ; load source char in al
    mov   ah, 0x1F              ; set colors as above
    stosw                       ; write ax to vga mem
    cmp   al, 0                 ; copy until \0 terminator is reached
    jne   .copy
    
    halt  

section .data.boot

msg_notimplemented: db "*** NOT IMPLEMENTED ***",0
msg_nocpuid:        db "*** CPUID not supported ***",0
msg_nolongmode:     db "*** 64 Bit (LONG MODE) not supported ***",0

align 8
boot32_gdt_data:
    .null_entry:    
        dq 0x0 
    .kernel_code:   equ $ - boot32_gdt_data
        dw 0x0          ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 10011000b    ; access (Present, Ring0, Segment, Execute) 
        db 00100000b    ; flags (long mode) and limit (16..19)
        db 0x00         ; base (24..31)
    .kernel_data:   equ $ - boot32_gdt_data
        dw 0x0          ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 10010010b    ; access (Present, Ring0, Segment, Write) 
        db 00000000b    ; no flags and limit (16..19)
        db 0x00         ; base (24..31)
    .user_code:   equ $ - boot32_gdt_data
        dw 0x0          ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 11111000b    ; access (Present, Ring3, Segment, Execute) 
        db 00100000b    ; flags (long mode) and limit (16..19)
        db 0x00         ; base (24..31)
    .user_data:   equ $ - boot32_gdt_data
        dw 0x0          ; limit (0..15)
        dw 0x0          ; base  (0..15)
        db 0x0          ; base  (16..23)
        db 11110010b    ; access (Present, Ring3, Segment, Write) 
        db 00000000b    ; no flags and limit (16..19)
        db 0x00         ; base (24..31)
    .end:
    
align 8
boot32_gdtr1:
    dw boot32_gdt_data.end - boot32_gdt_data - 1 ; limit
    dq gdt_data         ; base
boot32_gdtr2:
    dw boot32_gdt_data.end - boot32_gdt_data - 1 ; limit
    dq gdt_data + KERNEL_VMA ; base in higher half
