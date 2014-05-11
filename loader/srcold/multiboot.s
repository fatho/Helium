section .multiboot
ALIGN 4

MB_MAGIC        EQU 0x1BADB002  ; Multiboot Magic Value

MB_FLAG_ALIGN   EQU 0x0001      ; align modules on page boundaries (4 KB)
MB_FLAG_MEMINFO EQU 0x0002      ; request memory information from bootloader

MB_FLAGS        EQU (MB_FLAG_ALIGN | MB_FLAG_MEMINFO)
MB_CHECKSUM     EQU -(MB_MAGIC + MB_FLAGS)

Header:
   dd MB_MAGIC
   dd MB_FLAGS
   dd MB_CHECKSUM
