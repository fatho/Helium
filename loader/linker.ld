OUTPUT_FORMAT(elf32-i386)
ENTRY(boot_bsp)

SECTIONS
{
    . = 1M;

    /* First put the multiboot header, as it is required to be put very early
       early in the image or the bootloader won't recognize the file format.
       Next we'll put the .text section. */
    .text ALIGN(4K) :
    {
        *(.multiboot)
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
        *(COMMON)
        *(.bss)
    }
    
    .extra ALIGN(4K) : 
    {
       /* space for idt and gdt */
       idt_data   = .; . += 4K;
       gtd_data   = .; . += 4K;
       /* space for identity mapping page tables */
       page_pml4t = .; . += 4K;
       page_pdpt  = .; . += 4K;
       page_pdt   = .; . += 4K;
       page_pt    = .; . += 4K;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
    
    /* mark the first free page after the loader */
    . = ALIGN(4K);
    loader_top = . ;
}
