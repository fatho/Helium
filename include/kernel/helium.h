/*
 * @file
 * @date May 07, 2014
 * @author Fabian Thorand
 *
 * @brief General definitions that are part of the kernels public interface.
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

/// virtual base address of kernel space (highest 2 GB)
#define KERNEL_VMA 0xFFFFFFFF80000000L

/********************************************************************
 * TYPES
 ********************************************************************/

/// marker type for extern definitions
typedef struct {
} marker_t ;

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
    uintptr_t    kernel_top_paddr;  ///< physical address of first free page after the kernel image
    he_module_t* module_table;      ///< linear address of module table (see #he_module_t)
    he_mmap_t*   mmap_table;        ///< linear address of memory-map-table (see #he_mmap_t)
    char*        string_table;      ///< linear address of string table
    uint32_t  module_count;         ///< number of modules in module table
    uint32_t  mmap_count;           ///< number of entries in memory-map-table
    uint32_t  string_table_size;    ///< size of string table in bytes
}__attribute__((packed)) he_info_t;



#endif /* INFO_H_ */
