OUTPUT_FORMAT(elf64-x86-64)
ENTRY(boot32_bsp)

VIRT_BASE = 0xFFFFFFFF80000000;

SECTIONS
{   
    . = 4K;
    
    .boot16 ALIGN(4K) :
    {   
        *(.multiboot)
        *(.text16.boot)
        *(.rodata16.boot)
    }

    . = 1M;

    /* Bootcode at 1 MiB */
    .boot ALIGN(4K) :
    {
        boot_code_start = .;
        /* boot code may be overwritten after APs are booted */
        *(.text.boot)
        *(.data.boot)
        . = ALIGN(4K);
        boot_code_end = .;
        /* reserve space for GDT and IDT */
        gdt_data     = .; . += 4K;
        idt_data     = .; . += 4K;
        
        /* space for initial page tables */
        page_pml4t = .; . += 4K;
        /* for lower identity mapping */
        
        page_id_pdpt  = .; . += 4K;
        page_id_pdt   = .; . += 4K;
        
        /* Higher half at FFFFFFFF 80000000
        ==> 48 Bit Canonical address
            1111 1111 1111 1111 1000 0000 0000 0000 0000 | 0000 0000 0000
            PT-Index:    000000000 = 0
            PDT-Index:   000000000 = 0
            PDPT-Index:  111111110 = 510
            PML4T-Index: 111111111 = 511
            
            */
        /* for mappings above FFFFFF80 00000000 */
        page_high_pdpt = .; . += 4K;
        /* for mapping the physical memory up to 4GB to the range 
         * FFFFFF80 00000000 - FFFFFF81 00000000
         */ 
        page_phys_pdt = .; . += 4 * 4K;
        /* for kernel code mapping  
         * FFFFFFFF 80000000 - FFFFFFFF C0000000
         */ 
        page_kernel_pdt = .; . += 4K;
    }
    
    . += VIRT_BASE;

    .text ALIGN(4K) : AT(ADDR(.text) - VIRT_BASE) 
    {
        __TEXT_START = .;
        *(.text)
        *(.rodata)
        *(.rodata*)
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
        . = ALIGN(4K);
        stack_limit = .; . += 8K;
        stack_bottom = .;
        __BSS_END = .;
    }
    
    /* mark the first free page after the loader */
    . = ALIGN(4K);
    
    kernel_end = . - VIRT_BASE;
}
