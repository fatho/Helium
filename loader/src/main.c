#include <stdint.h>
#include <stddef.h>
#include "screen.h"
#include "string.h"
#include "multiboot.h"
#include "helium.h"
#include "idt.h"
#include "util.h"

multiboot_info_t* multiboot_info;

#define EXTRA_SECTION __attribute__((section(".extra")))

// start address of .extra section
extern uint32_t     extra_start;
// end address of .extra section
extern uint32_t     extra_end;

extern idt64_t      idt_data[HE_IDT_MAX_ENTRIES] EXTRA_SECTION;
extern uint64_t     gdt_data[HE_GDT_MAX_ENTRIES] EXTRA_SECTION;
extern he_info_t    info_table;
extern he_module_t  module_table[256];
extern he_mmap_t    mmap_table[256];
extern char*        string_table;

char* string_table_alloc(size_t length) {
    static size_t remaining_size = 4096 - 1;
    // first entry remains 0, (offset 0 maps to zero-length string)
    static size_t offset         = 1;

    // reserve size for terminator
    length += 1;

    if(length > remaining_size) {
        kpanic("*** NO MORE SPACE IN STRING TABLE ***");
    }

    char* allocated = string_table + offset;
    offset += length;
    remaining_size -= length;

    return allocated;
}

void parse_modules(multiboot_mod_t* modules, uint32_t modcount) {
    for(uint32_t i = 0; i < modcount; i++) {
        if(strncmp((char*)modules[i].name, "kernel64", 8) == 0) {
            kputs("=== KERNEL FOUND ===\n");
        } else {
            kputs("=== MODULE FOUND ===\n");
        }
        kputs("cmdline   : "); kputs((char*)modules[i].name); kputs("\n");
        kputs("addr_start: "); kputi(modules[i].addr_start); kputs("\n");
        kputs("addr_end  : "); kputi(modules[i].addr_end); kputs("\n");
    }
}

void parse_mmap(uint32_t mmap_start, uint32_t mmap_size) {
    info_table.mmap_count = 0;

    multiboot_mmap_t* entry = (multiboot_mmap_t*)mmap_start;
    while((uintptr_t)entry < mmap_start + mmap_size) {
        kputi(entry->base); kputs(" "); kputi(entry->length);
        if(entry->type == 1) {
            kputs("   A\n");
        } else {
            kputs(" N/A\n");
        }

        info_table.mmap_count += 1;
        // jump to next entry
        entry = (multiboot_mmap_t*)((uintptr_t)entry + entry->size + sizeof(uint32_t));
    }
}

void ldr_main() {
    // zero extra section
    memset((void*)extra_start, 0, extra_end - extra_start);

    // populate info table
    info_table.idt_paddr = (uint32_t)idt_data;
    info_table.gdt_paddr = (uint32_t)gdt_data;
    info_table.mod_table = (uint32_t)module_table - (uint32_t)&info_table;
    info_table.mmap_table = (uint32_t)mmap_table - (uint32_t)&info_table;

    kputs("populating info structure\n");
    kputs("IDT @ "); kputi(info_table.idt_paddr); kputs("\n");
    kputs("GDT @ "); kputi(info_table.gdt_paddr); kputs("\n");

    kputs("Parsing memory map...\n");
    kputs("#entries: "); kputi(multiboot_info->mmap_len); kputs("\n");
    parse_mmap(multiboot_info->mmap, multiboot_info->mmap_len);

    kputs("Parsing multiboot info...\n");
    parse_modules((multiboot_mod_t*)multiboot_info->mods, multiboot_info->mods_count);

    return;
}
