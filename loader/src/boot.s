;;; Macros
%define magicbreak xchg bx, bx
%define halt jmp $
%macro panic 1 
   mov esi,%1
   jmp loader32_panic
%endmacro
;;; Code
section .text

global boot_bsp
extern multiboot_info
extern ldr_main
extern kputs

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

    call  ldr_main              ; enter C code

    halt  
    
    ;; ERROR-Handlers
.no_cpuid:
    panic msg_nocpuid

.no_longmode:
    panic msg_nolongmode

;;; Description:
;;;    Enables the newer APIC and disables the PIC
enable_apic:
    ;; check for APIC support
    mov eax, 0x01       ; CPUID Processor Signature and Standard Features
    cpuid
    test edx, 1<<9      ; Bit 9 indicates APIC support
    jz .no_apic
    
    ;; disable PIC
    mov al, 0xff
    out 0xa1, al        ; PIC2_DATA port
    out 0x21, al        ; PIC1_DATA port

    ret
.no_apic:
    panic msg_noapic

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
    msg_noapic db "*** CPU HAS NO APIC ***",0

section .data

section .bss

;;; temporary stack used in loader
stack_bottom:
    resb  0x4000                ; 16 KiB
stack_top:
