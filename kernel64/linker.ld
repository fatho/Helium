OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernel_main)

SECTIONS {
    . = -2M;

    .text ALIGN(4K) : {
        *(.text)
        *(.rodata)
    }

    .data ALIGN(4K) : {
        *(.data)
    }
    
    .bss ALIGN(4K) : {
        *(.bss)
    }
    
    /DISCARD/ : {
        *(.comment)
    }
}
