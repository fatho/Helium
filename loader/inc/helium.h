/*
 * info.h
 *
 *  Created on: 07.05.2014
 *      Author: fabian
 */

#ifndef HELIUM_H_
#define HELIUM_H_

// maximum number of entries in IDT
#define HE_IDT_MAX_ENTRIES 256
// maximum number of entries in IDT
#define HE_GDT_MAX_ENTRIES 512
// size of string table in bytes
#define HE_STRING_TABLE_SIZE 4096

/**
 * Contains information about a module available at boot time.
 *
 * Size: 16 bytes
 */
typedef struct {
    uint64_t paddr;     // physical address of the module
    uint32_t length;    // length of the module
    uint16_t cmdline;   // offset into string_table to \0-terminated string
    uint16_t reserved;  // padding to 16B;
}__attribute__((packed)) he_module_t;


/**
 * Contains information about a region of memory.
 *
 * Size: 32 bytes
 */
typedef struct {
    uint64_t base;      // physical address of the beginning of this memory region
    uint64_t length;    // length of this memory region in bytes
    uint32_t available; // if this region is free to use or not
    uint32_t padding;
    uint64_t reserved;
} __attribute__((packed)) he_mmap_t;


/**
 * Contains all information the loader is passing to the kernel.
 */
typedef struct {
    uint64_t idt_paddr;  // physical address of IDT
    uint64_t gdt_paddr;  // physical address of GDT
    uint64_t free_paddr; // physical address of first free page
    uint16_t mod_table;  // offset to module table from the beginning of this structure
    uint16_t mod_count;  // number of modules in module table
    uint16_t cmdline;    // kernel command line (does not include the "kernel64" part)
    uint16_t mmap_table; // offset to memory-map-table
    uint16_t mmap_count; // number of entries in memory-map-table
}__attribute__((packed)) he_info_t;

#endif /* INFO_H_ */
