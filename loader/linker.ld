OUTPUT_FORMAT(elf32-i386)
ENTRY(boot32_bsp)

SECTIONS
{
    . = 1K;
    
    /* place multiboot header in 16 bit code section. the entry point called
     * by GRUB is in .text.
     */
    .text16 ALIGN(4K) :
    {
        *(.multiboot)
        *(.text16)
    }
    
    .rodata16 ALIGN(4K) :
    {
        *(.rodata16)
    }

    . = 1M;

    .text ALIGN(4K) :
    {
        *(.text)
    }

    /* Read-only data. */
    .rodata ALIGN(4K) :
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) :
    {
        *(.data)
    }

    /* Read-write data (uninitialized) */
    .bss ALIGN(4K) :
    {
        /* stack used in loader */
        stack_bottom = .; . += 16K;
        stack_top = .;
        *(COMMON)
        *(.bss)
    }
    
    .info ALIGN(4K) : 
    {
        /* start marker */
        info_start = .;
        /* space for IDT and GDT */
        idt_data     = .; . += 4K;
        gdt_data     = .; . += 4K;  /* defined in boot32.s */
        /* space for initial page tables */
        page_pml4t = .; . += 4K;
        page_pdpt  = .; . += 4K;
        page_pdt   = .; . += 4K;
        page_pt    = .; . += 4K;
        /* info structures */
        info_table   = .; . += 4K;
        info_modules = .; . += 4K;
        info_mmap    = .; . += 4K;
        info_strings = .; . += 4K;
        
        /* end marker */
        info_end = .;
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
