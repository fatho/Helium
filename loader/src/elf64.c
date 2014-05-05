/*
 * elf64.c
 *
 *  Created on: 05.05.2014
 *      Author: fabian
 */

#include <elf64.h>

/**
 * calculates a has value for an ELF symbol name to be used with
 * the ELF symbol hash table.
 */
unsigned long elf64_hash(const unsigned char *name) {
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + *name++;
        if ( (g = (h & 0xf0000000)) )
            h ^= g >> 24;
        h &= 0x0fffffff;
    }
    return h;
}
