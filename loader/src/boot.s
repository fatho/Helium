;;; Macros
%define magicbreak xchg bx, bx
%define halt jmp $
%macro panic 1 
   mov   esi,%1
   call  loader32_panic
%endmacro
;;; Code
section .text

global boot_bsp
extern multiboot_info
extern multiboot_init
extern kernel_msg

;;; Description: 
;;;   Initializes the BSP (bootstrap processor), switches to long mode and loads the kernel.
;;;   This function is invoked by the bootloader.
boot_bsp:
    cli                         ; clear interrupts until we have IDT
    magicbreak 
    mov   dword [multiboot_info], ebx ; save multiboot pointer
    mov   esp, stack_top        ; setup stack

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

    call  multiboot_init        ; init multiboot
   

    ;; TODO: halt for now
    panic msg_notimplemented

    cli   
    halt  
    
    ;; ERROR-Handlers
.no_cpuid:
    panic msg_nocpuid

.no_longmode:
    panic msg_nolongmode

;;; Description:
;;;    Initializes 64 Bit page tables with identity mapping for the first 2 MiB.
;;;    PML4T[0] (Page Map Level 4 Table):      0x1000
;;;    PDPT[0] (Page Directory Pointer Table): 0x2000
;;;    PDT[0] (Page Directory Table):          0x3000
;;;    PT[0] (Page Table):                     0x4000
setup_identity_paging:
    ;; clear tables (from 0x1000 to 0x4FFF)
    mov   edi, 0x1000           ; Set the destination index to 0x1000.
    mov   cr3, edi               ; Set control register 3 to the PML4T
    xor   eax, eax               ; Nullify the A-register.
    mov   ecx, 4096              ; 4096 double words = all 4 tables
    rep   stosd                  ; Clear the memory.

    mov   edi, cr3               ; retrieve PML4T address

    ;; initializes first entries of the first three tables
    ;; 0x3 = 0b11 means: Read/Write and Present
    mov   DWORD [edi], 0x2003    ; PML4T[0] = 0x2003
    add   edi, 0x1000
    mov   DWORD [edi], 0x3003    ; PDPT[0]  = 0x3003
    add   edi, 0x1000
    mov   DWORD [edi], 0x4003    ; PDT[0]   = 0x4003
    add   edi, 0x1000

    ;; initialize identity mapping in first PT
    mov   ebx, 0x00000003        ; Set the B-register to 0x00000003.
    mov   ecx, 512               ; Set the C-register to 512.

.SetEntry:
    mov   DWORD [edi], ebx       ; Set the double word at the destination index to the B-register.
    add   ebx, 0x1000            ; Add 0x1000 to the B-register.
    add   edi, 8                 ; Add eight to the destination index.
    loop  .SetEntry              ; Setd asd athe next entry.
    
    ;; enable PAE
    mov   eax, cr4
    or    eax, 1 << 5           ; Set the PAE-bit, which is the 6th bit (bit 5).
    mov   cr4, eax

    ;; set Long Mode bit
    mov   ecx, 0xC0000080       ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                       ; Read from the model-specific register.
    or    eax, 1 << 8           ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                       ; Write to the model-specific register.

    ;; enable paging
    mov   eax, cr0
    or    eax, 1 << 31          ; Set the PG-bit, which is the 32nd bit (bit 31).
    mov   cr0, eax
    
    

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
loader32_panic:
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

section .rodata

    msg_notimplemented db "*** NOT IMPLEMENTED ***",0
    msg_nocpuid db "*** CPUID not supported ***",0
    msg_nolongmode db "*** 64 Bit (LONG MODE) not supported ***",0

section .data

section .bss

;;; temporary stack used in loader
stack_bottom:
    resb  0x4000                ; 16 KiB
stack_top:
