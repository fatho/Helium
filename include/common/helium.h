/*
 * info.h
 *
 *  Created on: 07.05.2014
 *      Author: fabian
 */

#ifndef HELIUM_H_
#define HELIUM_H_

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
    uint32_t  mod_count;            ///< number of modules in module table
    uint32_t  mmap_count;           ///< number of entries in memory-map-t
    uintptr_t mod_table;            ///< physical address of module tableable
    uintptr_t mmap_table;           ///< physical address of memory-map-table
    uint32_t  string_table_size;    ///< size of string table in bytes
    uintptr_t string_table;         ///< physical address of string table
    uint16_t  cmdline;              ///< kernel command line (does not include the "kernel64" part)
}__attribute__((packed)) he_info_t;


/********************************************************************
 * KERNEL INFORMATION STRUCTURES
 ********************************************************************/

typedef struct {

}__attribute__((packed)) he_kernel_header_t;


#endif /* INFO_H_ */
