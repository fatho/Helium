#ifndef INFO_H_
#define INFO_H_

#include "kernel/helium.h"
#include "kernel/info/multiboot.h"

#include <stdint.h>
#include <stddef.h>

/// structure containing all information and pointers to other tables
extern he_info_t info_table;
/// table containing information about loaded modules
extern he_module_t info_modules[256];
/// table containing the memory map received from the bootloader
extern he_mmap_t info_mmap[256];
/// memory region containing null terminated strings
extern char info_strings[HE_STRING_TABLE_SIZE];

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
void info_parse_modules(multiboot_mod_t* modules, int modcount);

/**
 * @brief Parses the given multiboot memory map and stores the results
 * in the Helium info_mmap.
 *
 * All memory regions in info_mmap are page aligned to HE_PAGE_SIZE (4KiB by default).
 *
 * @param mmap_start 32 bit physical address of the first mmap table entry.
 * @param mmap_size size of the multiboot mmap table in bytes
 */
void info_parse_mmap(uintptr_t mmap_start, uint32_t mmap_size);
/**
 * @brief Allocates length+1 bytes in the string table.
 *
 * @param length the length of the string not including the '\0' character.
 *
 * @return physical address of allocated entry
 *
 * @remark Panics when no space is left.
 */
char* info_string_alloc(size_t length);

/**
 * @brief Initializes the Helium info tables.
 *
 * Performs the following tasks:
 *  - fills all fields of info_table
 *  - fills info_modules
 *  - fills info_mmap
 */
void info_init();

#endif
