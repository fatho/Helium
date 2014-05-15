OUTPUT_FORMAT(elf64-x86-64)
ENTRY(boot32_bsp)

SECTIONS
{
    .text16 0x1000 /* 4 KiB */ : 
    {
        *(.text16)
        *(.rodata16)
    }

    .text 0x100000 /* 1 MiB */ : 
    {
        __TEXT_START = .;
        *(.multiboot)
        *(.text)
        *(.rodata)
        *(.rodata*)
        __TEXT_END = .;
    }
    
    /* Read-write data (initialized) */
    .data :
    {
        __DATA_START = .;
        *(.data)
        __TEXT_END = .;
    }

    /* Read-write data (uninitialized) */
    .bss :
    {
        __BSS_START = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(4K);
        /* stack used in loader */
        stack_bottom = .; . += 8K;
        stack_top = .;
        __BSS_END = .;
    }
    
    .info 0x110000 /* 1 MiB + 64 KiB */ : 
    {
        __INFO_START = .;
        
        /* start marker */
        info_start = .;
        /* space for IDT and GDT */
        idt_data     = .; . += 4K;
        gdt_data     = .; . += 4K;
        /* info structures */
        info_table   = .; . += 4K;
        info_modules = .; . += 4K;
        info_mmap    = .; . += 4K;
        info_strings = .; . += 4K;
        /* space for initial page tables */
        page_id_pml4t = .; . += 4K;
        page_id_pdpt  = .; . += 4K;
        page_id_pdt   = .; . += 4K;
        page_id_pt    = .; . += 4K * 16;
        
        /* end marker */
        info_end = .;
        
        __INFO_END = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
    
    /* mark the first free page after the loader */
    . = ALIGN(4K);
    loader_end = . ;
}
