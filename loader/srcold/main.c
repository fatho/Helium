#include <stdint.h>
#include <stddef.h>
#include "screen.h"
#include "string.h"
#include "multiboot.h"
#include "helium.h"
#include "idt.h"
#include "gdt.h"
#include "util.h"
#include "paging.h"
#include "elf64.h"

multiboot_info_t* multiboot_info;

#define EXTRA_SECTION __attribute__((section(".extra")))

// start marker of .extra section
extern uint8_t extra_start;
// end marker of .extra section
extern uint8_t extra_end;
// end marker of loader
extern uint8_t loader_end;

extern idt64_t idt_data[HE_IDT_MAX_ENTRIES];
extern uint64_t gdt_data[HE_GDT_MAX_ENTRIES];
extern he_info_t info_table;
extern he_module_t module_table[256];
extern he_mmap_t mmap_table[256];
extern char string_table[HE_STRING_TABLE_SIZE];

/**
 * Allocates length+1 bytes in the string table.
 * Panics when no space is left.
 *
 * @param length: the length of the string not including the '\0' character.
 *
 * Returns: physical address of allocated entry
 */
char* string_table_alloc(size_t length) {
    static size_t remaining_size = HE_STRING_TABLE_SIZE - 1;
    // first entry remains 0, (offset 0 maps to zero-length string)
    static size_t offset = 1;

    // reserve size for terminator
    length += 1;

    if (length > remaining_size) {
        kpanic("*** NO MORE SPACE IN STRING TABLE ***");
    }

    char* allocated = string_table + offset;
    offset += length;
    remaining_size -= length;

    return allocated;
}

/**
 * Parses the given multiboot module table and stores the results
 * in the Helium module_table. Command line arguments are allocated
 * in the string table.
 *
 * Entries in the module table are sorted ascending by physical address.
 */
void parse_modules(multiboot_mod_t* modules, uint32_t modcount) {
    // copy multiboot information to module table
    for (uint32_t i = 0; i < modcount; i++) {
        module_table[i].paddr = modules[i].addr_start;
        module_table[i].length = modules[i].addr_end;

        size_t namelen = strlen((char*) modules[i].name);
        char* str_tab_entry = string_table_alloc(namelen);
        memcpy(str_tab_entry, (char*) modules[i].name, namelen);

        module_table[i].cmdline = (str_tab_entry - string_table);
    }

    // sort entries by start address
    for (uint32_t i = 0; i < modcount - 1; i++) {
        uint32_t min_idx = i;
        for (uint32_t j = i + 1; i < modcount; i++) {
            if (module_table[j].paddr < module_table[min_idx].paddr) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            he_module_t tmp = module_table[i];
            module_table[i] = module_table[min_idx];
            module_table[min_idx] = tmp;
        }
    }

    info_table.mod_count = modcount;
}

/**
 * Parses the given multiboot memory map and stores the results
 * in the Helium mmap_table. All memory regions in mmap_table
 * are page aligned to HE_PAGE_SIZE (4KiB by default).
 */
void parse_mmap(uint32_t mmap_start, uint32_t mmap_size) {
    int eidx = 0;

    multiboot_mmap_t* entry = (multiboot_mmap_t*) mmap_start;
    while ((uintptr_t) entry < mmap_start + mmap_size) {
        // align memory map to page boundaries
        uint64_t aligned_base = PAGE_FLOOR(entry->base);
        uint64_t aligned_length = entry->length + (entry->base - aligned_base);

        mmap_table[eidx].base = aligned_base;
        mmap_table[eidx].length = aligned_length;
        mmap_table[eidx].available = entry->type == 1;

        // jump to next entry
        entry = (multiboot_mmap_t*) ((uintptr_t) entry + entry->size + sizeof(uint32_t));
        eidx += 1;
    }
    info_table.mmap_count = eidx;
}

/**
 * Allocates physical memory at the end of the loader by returning
 * info_table.free_addr and increasing that value by the page-aligned size.
 *
 * Return: page aligned address to a memory region of at least size bytes.
 */
void* phys_alloc(size_t size) {
    // round size to next page
    size = PAGE_CEIL(size);

    // TODO: check memory map

    uintptr_t allocated = (uintptr_t) info_table.free_paddr;
    info_table.free_paddr += size;

    return (void*) allocated;
}

/**
 * Moves all boot modules to the beginning of the address space.
 */
void move_modules() {
    kprintf("Relocating modules...\n");
    for (size_t i = 0; i < info_table.mod_count; i++) {
        if (module_table[i].paddr == info_table.free_paddr) {
            // module already at the right place
            info_table.free_paddr += PAGE_CEIL(module_table[i].length);
            kprintf("module %x is at the right place\n", i);
        } else {
            void* target = phys_alloc(module_table[i].length);
            memmove(target, (void*) (uintptr_t) module_table[i].paddr, module_table[i].length);
            kprintf("moved module %x from %x to %x\n", i, (uint32_t) module_table[i].paddr, target);
            module_table[i].paddr = (uintptr_t) target;
        }
    }
}

/**
 * Prepares a Global Descriptor Table (GDT) for long mode.
 */
void setup_gdt() {
    set_gdt_entry(gdt_data, 1, 0, 0, GDT_ACCESS_EXECUTE | GDT_ACCESS_PRESENT,
    GDT_FLAGS_LONGMODE);
    set_gdt_entry(gdt_data, 1, 0, 0, GDT_ACCESS_READWRITE | GDT_ACCESS_PRESENT, 0);
    // TODO: add segments for user code when needed
}

void ldr_main() {
    // zero extra section
    memset(&extra_start, 0, &extra_end - &extra_start);

    // populate info table
    info_table.idt_paddr = (uintptr_t) idt_data;
    info_table.gdt_paddr = (uintptr_t) gdt_data;
    info_table.mod_table = (uintptr_t) module_table - (uintptr_t) &info_table;
    info_table.mmap_table = (uintptr_t) mmap_table - (uintptr_t) &info_table;
    // align to next page
    info_table.free_paddr = ((uintptr_t) &loader_end + 0xFFF) & ~0xFFF;

    kputs("Parsing memory map...\n");
    parse_mmap(multiboot_info->mmap, multiboot_info->mmap_len);

    kputs("Parsing modules...\n");
    parse_modules((multiboot_mod_t*) multiboot_info->mods, multiboot_info->mods_count);

    // === BEGIN DEBUG
    kputs("info structure\n");
    kputs("==============\n");
    kprintf("IDT        @ %llx\n", info_table.idt_paddr);
    kprintf("GDT        @ %llx\n", info_table.gdt_paddr);
    kprintf("MOD_TABLE  @ %llx\n", info_table.mod_table);
    kprintf("#MODULES   = %llx\n", info_table.mod_count);
    for (int i = 0; i < info_table.mod_count; i++) {
        kprintf("  [%s]\n", string_table + module_table[i].cmdline);
        kprintf("    addr_start: %llx\n", module_table[i].paddr);
        kprintf("    length    : %llx\n", module_table[i].length);
    }
    kprintf("MMAP_TABLE @ %llx\n", info_table.mmap_table);
    kprintf("#MMAPS     = %llx\n", info_table.mmap_count);
    uint64_t total_avail_mem = 0;
    for (int i = 0; i < info_table.mmap_count; i++) {
        const char* avail = mmap_table[i].available ? "  A" : "N/A";
        kprintf("  %llx %llx %s\n", mmap_table[i].base, mmap_table[i].length, avail);

        if (mmap_table[i].available)
            total_avail_mem += mmap_table[i].length;
    }
    kprintf("TOTAL MEM  = %llx\n", total_avail_mem);
    // === END DEBUG

    // consolidate module space
    move_modules();

    // prepare GDT
    setup_gdt();

    // setup identity mapping
    setup_page_tables();

    kprintf("searching for kernel binary...\n");
    int kernelidx = -1;
    for (size_t i = 0; i < info_table.mod_count; i++) {
        if (strncmp("kernel64", string_table + module_table[i].cmdline, 8) == 0) {
            kernelidx = i;
            break;
        }
    }
    if (kernelidx == -1)
        kpanic("*** KERNEL64 NOT FOUND ***");

    kprintf("kernel found...\n");

    void* kernel = (void*) (uintptr_t) module_table[kernelidx].paddr;
    Elf64_Ehdr* kernel_hdr = (Elf64_Ehdr*) kernel;
    if (kernel_hdr->e_ident[EI_CLASS] != ELFCLASS64)
        kpanic("*** KERNEL IS NOT 64 BIT ***");

    kprintf("kernel segments\n");
    kprintf("===============\n");
    for (size_t i = 0; i < kernel_hdr->e_phnum; i++) {
        uintptr_t phdr_addr = (uintptr_t) kernel + (uintptr_t) kernel_hdr->e_phoff
                + kernel_hdr->e_phentsize * i;
        Elf64_Phdr* phdr = (Elf64_Phdr*)phdr_addr;

        kprintf(" %llx %llx\n %llx %llx\n\n", phdr->p_offset, phdr->p_filesz,phdr->p_vaddr, phdr->p_memsz);
        MAGIC_BREAK;

    }
    // TODO: map kernel

    // TODO: run kernel

    return;
}
