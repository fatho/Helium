ENTRY(kernel_main)

SECTIONS {

    .text 0xFFFFFF0000000000 : {
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data)
    }
    
    .bss : {
        *(.bss)
    }
    
    /DISCARD/ : {
        *(.comment)
    }
}
