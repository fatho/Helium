/**
 * @file
 * @author Fabian Thorand
 * @date May 13, 2014
 *
 * @brief This module contains the functions which collect and store information
 * about the hardware and memory.
 */

#include "kernel/info.h"
#include "kernel/helium.h"
#include "kernel/debug.h"
#include "kernel/info/multiboot.h"
#include "kernel/klibc/string.h"
#include "kernel/panic.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief pointer to multiboot info structure. set by #boot64_bsp in boot32.s
 */
multiboot_info_t* multiboot_info;

/// structure containing all information and pointers to other tables
he_info_t info_table;
/// table containing information about loaded modules
he_module_t info_modules[256];
/// table containing the memory map received from the bootloader
he_mmap_t info_mmap[256];
/// memory region containing null terminated strings
char info_strings[HE_STRING_TABLE_SIZE];


/**
 * @brief Parses the given multiboot module table and stores the results
 * in the Helium info_modules.
 *
 * Command line arguments are allocated in the string table.
 * Entries in the module table are sorted ascending by physical address.
 *
 * @param modules pointer to multiboot module table
 * @param modcount number of modules in the module table
 * @remark uses string_table_alloc(size_t), so this function may panic,
 * when there is no space left in the string table.
 */
void info_parse_modules(multiboot_mod_t* modules, int modcount) {
    // copy multiboot information to module table
    for (int i = 0; i < modcount; i++) {
        info_modules[i].paddr = modules[i].addr_start;
        info_modules[i].length = modules[i].addr_end;

        size_t namelen = strlen((char*) (uintptr_t) modules[i].name);
        char* str_tab_entry = info_string_alloc(namelen);
        memcpy(str_tab_entry, (char*) (uintptr_t) modules[i].name, namelen);

        info_modules[i].cmdline = (str_tab_entry - info_strings);
    }

    // sort entries by start address
    for (int i = 0; i < modcount - 1; i++) {
        int min_idx = i;
        for (uint32_t j = i + 1; i < modcount; i++) {
            if (info_modules[j].paddr < info_modules[min_idx].paddr) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            he_module_t tmp = info_modules[i];
            info_modules[i] = info_modules[min_idx];
            info_modules[min_idx] = tmp;
        }
    }

    info_table.module_count = modcount;
}

/**
 * @brief Parses the given multiboot memory map and stores the results
 * in the Helium info_mmap.
 *
 * All memory regions in info_mmap are page aligned to HE_PAGE_SIZE (4KiB by default).
 *
 * @param mmap_start 32 bit physical address of the first mmap table entry.
 * @param mmap_size size of the multiboot mmap table in bytes
 */
void info_parse_mmap(uintptr_t mmap_start, uint32_t mmap_size) {
    int eidx = 0;

    multiboot_mmap_t* entry = (multiboot_mmap_t*) mmap_start;
    while ((uintptr_t) entry < mmap_start + mmap_size) {
        // align memory map to 4K page boundaries
        uint64_t aligned_base = entry->base & ~0xFFF;
        uint64_t aligned_length = entry->length + (entry->base - aligned_base);
        aligned_length += 0xFFF;
        aligned_length &= ~0xFFF;

        info_mmap[eidx].base = aligned_base;
        info_mmap[eidx].length = aligned_length;
        info_mmap[eidx].available = entry->type == 1;

        // jump to next entry
        entry = (multiboot_mmap_t*) ((uintptr_t) entry + entry->size + sizeof(uint32_t));
        eidx += 1;
    }
    info_table.mmap_count = eidx;
}

/**
 * @brief Allocates length+1 bytes in the string table.
 *
 * @param length the length of the string not including the '\0' character.
 *
 * @return physical address of allocated entry
 *
 * @remark Panics when no space is left.
 */
char* info_string_alloc(size_t length) {
    static size_t remaining_size = HE_STRING_TABLE_SIZE - 1;
    // first entry remains 0, (offset 0 maps to zero-length string)
    static size_t offset = 1;

    // reserve size for terminator
    length += 1;

    if (length > remaining_size) {
        kpanicf("Not enough space in string_table.\n"
                "Requested: %llx\n"
                "Available: %llx", (uint64_t) length, (uint64_t) remaining_size);
    }

    char* allocated = info_strings + offset;
    offset += length;
    remaining_size -= length;

    return allocated;
}

/**
 * @brief Initializes the Helium info tables.
 *
 * Performs the following tasks:
 *  - fills all fields of info_table
 *  - fills info_modules
 *  - fills info_mmap
 */
void info_init() {
    // populate info table
    info_table.module_table = info_modules;
    info_table.mmap_table = info_mmap;

    info_parse_mmap(multiboot_info->mmap, multiboot_info->mmap_len);

    info_parse_modules((multiboot_mod_t*) (uintptr_t) multiboot_info->mods,
            multiboot_info->mods_count);

    extern uint8_t kernel_end;
    uintptr_t free_paddr_max = (uintptr_t) &kernel_end;
    for(unsigned int i = 0; i < info_table.module_count; i++) {
        uintptr_t mod_end = info_modules[i].paddr + info_modules[i].length;
        if(mod_end > free_paddr_max) {
            free_paddr_max = mod_end;
        }
    }
    // align to next page
    info_table.kernel_top_paddr = ((uintptr_t) &kernel_end + 0xFFF) & ~0xFFF;
}
