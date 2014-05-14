/**
 * @file
 * @author Fabian Thorand
 * @date May 13, 2014
 *
 * @brief This module contains basic paging functionality.
 */

#include "loader/paging.h"
#include "loader/pheap.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief reads \c CR3 which contains the physical address of the PML4T.
 *
 * @return physical address of the PML4T
 */
uintptr_t paging_get_pml4t() {
    uintptr_t pml4t;
    asm volatile ("mov %%cr3, %0" : "=a" (pml4t));
    return pml4t;
}

/**
 * @brief writes physical address of the PML4T to \c CR3.
 *
 * @param pml4t physical address of the PML4T
 */
void paging_set_pml4t(uintptr_t pml4t) {
    asm volatile ("mov %0, %%cr3" : : "a" (pml4t));
}

/**
 * @brief Returns the page table for the given virtual address on the specified level.
 *
 * @param vaddr the virtual address to use for the page table walk.
 * @param level the level of the page table of interest
 * @param create if nonzero, the requested page table is created when it does not exist.
 */
uintptr_t paging_get_table(uintptr_t vaddr, int level, int create) {
    if(level == PAGE_LVL_PML4T)
        return paging_get_pml4t();

    // parent table
    uint64_t* parent = paging_get_table(vaddr, level + 1, create);

    uint64_t offset = PAGE_TABLE_INDEX(vaddr, level);
    uint64_t entry = parent[offset];

    if(PAGE_IS_PRESENT(entry)) {
        return PAGE_ENTRY_ADDR_4K(entry);
    } else {
        if(create) {
            // allocate table
        }

    }

}


