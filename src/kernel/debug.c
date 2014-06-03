/**
 * @file debug.c
 *
 * @author fabian
 * @date   03.06.2014
 *
 * @brief 
 */

#include "kernel/debug.h"
#include "kernel/info.h"
#include "kernel/klibc/kstdio.h"

/**
 * @brief Writes the contents of the info structures to the screen.
 */
void debug_print_info() {
    kputs("\x1b[33m");

    kputs("info structure\n");
    kputs("==============\n");
    kprintf("MOD_TABLE  @ %llx\n", info_table.module_table);
    kprintf("#MODULES   = %x\n", info_table.module_count);

    for (unsigned int i = 0; i < info_table.module_count; i++) {
        kprintf("  [%s]\n", info_strings + info_modules[i].cmdline);
        kprintf("    addr_start: %llx\n", info_modules[i].paddr);
        kprintf("    length    : %x\n", info_modules[i].length);
    }
    kprintf("info_mmap @ %llx\n", info_table.mmap_table);
    kprintf("#MMAPS     = %x\n", info_table.mmap_count);
    uint64_t total_avail_mem = 0;
    for (unsigned int i = 0; i < info_table.mmap_count; i++) {
        const char* avail = info_mmap[i].available ? "  A" : "N/A";
        kprintf("  %llx %llx %s\n", info_mmap[i].base, info_mmap[i].length, avail);

        if (info_mmap[i].available)
            total_avail_mem += info_mmap[i].length;
    }
    kprintf("TOTAL MEM  = %llx\n", total_avail_mem);

    kputs("\x1b[0m");
}
