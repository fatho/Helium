/*
 * info.h
 *
 *  Created on: 07.05.2014
 *      Author: fabian
 */

#ifndef HELIUM_H_
#define HELIUM_H_

#include <stdint.h>

/// Helium uses 4KB pages for most allocations
#define HE_PAGE_SIZE 0x1000

/// maximum number of entries in IDT
#define HE_IDT_MAX_ENTRIES 256
/// maximum number of entries in IDT
#define HE_GDT_MAX_ENTRIES 512
/// size of string table in bytes
#define HE_STRING_TABLE_SIZE HE_PAGE_SIZE

/********************************************************************
 * SYSTEM INFORMATION STRUCTURES
 ********************************************************************/

/**
 * Contains information about a module available at boot time.
 *
 * Size: 16 bytes
 */
typedef struct {
    uint64_t paddr;     ///< physical address of the module
    uint32_t length;    ///< length of the module
    uint16_t cmdline;   ///< offset into string_table to \0-terminated string
    uint16_t reserved;  ///< padding to 16B;
}__attribute__((packed)) he_module_t;


/**
 * Contains information about a region of memory.
 *
 * Size: 32 bytes
 */
typedef struct {
    uint64_t base;      ///< physical address of the beginning of this memory region
    uint64_t length;    ///< length of this memory region in bytes
    uint64_t available; ///< 1 if this region is free, 0 otherwise
    uint64_t reserved;
} __attribute__((packed)) he_mmap_t;


/**
 * Contains all information the loader is passing to the kernel.
 */
typedef struct {
    uintptr_t idt_paddr;            ///< physical address of IDT
    uintptr_t gdt_paddr;            ///< physical address of GDT
    uintptr_t free_paddr;           ///< physical address of first free page
    uintptr_t module_table_paddr;   ///< physical address of module table (see #he_module_t)
    uintptr_t mmap_table_paddr;     ///< physical address of memory-map-table (see #he_mmap_t)
    uintptr_t string_table_paddr;   ///< physical address of string table

    uint32_t  module_count;         ///< number of modules in module table
    uint32_t  mmap_count;           ///< number of entries in memory-map-table
    uint32_t  string_table_size;    ///< size of string table in bytes
    uint32_t  kernel_module;        ///< index of the kernel module in module table (see #module_table_paddr)
}__attribute__((packed)) he_info_t;



#endif /* INFO_H_ */
