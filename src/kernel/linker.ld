OUTPUT_FORMAT(elf64-x86-64)
ENTRY(boot32_bsp)

VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS
{   
    . = 4K;
    
    .boot16 ALIGN(4K) :
    {   
        *(.multiboot)
        *(.text16)
        *(.rodata16)
    }

    . = 1M;

    /* Bootcode at 1 MiB */
    .boot ALIGN(4K) :
    {
        *(.boot)
        /* space for initial page tables */
        page_id_pml4t = .; . += 4K;
        page_id_pdpt  = .; . += 4K;
        page_id_pdt   = .; . += 4K;
        page_id_pt    = .; . += 4K * 16;
        /* space for IDT and GDT */
        idt_data     = .; . += 4K;
        gdt_data     = .; . += 4K;
    }
    
    . += VIRT_BASE;

    .text ALIGN(4K) : AT(ADDR(.text) - VIRT_BASE) 
    {
        __TEXT_START = .;
        *(.text)
        *(.rodata)
        *(.rodata*)
        main_bsp_base = main_bsp - VIRT_BASE;
        main_ap_base = main_ap - VIRT_BASE;
        __TEXT_END = .;
    }
    
    /* Read-write data (initialized) */
    .data ALIGN(4K) : AT(ADDR(.data) - VIRT_BASE) 
    {
        __DATA_START = .;
        *(.data)
        __DATA_END = .;
    }

    /* Read-write data (uninitialized) */
    .bss ALIGN(4K) : AT(ADDR(.bss) - VIRT_BASE)
    {
        __BSS_START = .;
        *(.bss)
        *(COMMON)
        __BSS_END = .;
    }
    
    /* mark the first free page after the loader */
    . = ALIGN(4K);
    
    
    loader_end = . - VIRT_BASE;
}
