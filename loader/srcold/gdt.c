/*
 * gdt.c

 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */
#include "gdt.h"
#include <stdint.h>

void set_gdt_entry(uint64_t* gdt_data, int index, uint32_t base, uint32_t limit,
        uint8_t access, uint8_t flags) {
    gdt_data[index] = limit & 0xFFFF;
    gdt_data[index] |= (uint64_t) (base & 0xFFFFFF) << 16;
    gdt_data[index] |= (uint64_t) (access & 0xFF) << 40;
    gdt_data[index] |= (uint64_t) ((limit >> 16) & 0xF) << 48;
    gdt_data[index] |= (uint64_t) (flags & 0xF) << 52;
    gdt_data[index] |= (uint64_t) ((base >> 24) & 0xFF) << 48;
}
