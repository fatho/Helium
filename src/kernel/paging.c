/**
 * @file
 * @author Fabian Thorand
 * @date May 13, 2014
 *
 * @brief This module contains basic paging functionality.
 */

#include "loader/debug.h"
#include "loader/paging.h"
#include "loader/pheap.h"
#include "loader/string.h"
#include "loader/screen.h"
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
 * @brief Returns physical address of the page table for the given virtual address on the specified level.
 *
 * @param vaddr the virtual address to use for the page table walk.
 * @param table_paddr receives the physical address of the requested page table on success.
 * @param level the level of the page table of interest
 * @param create if nonzero, the requested page table is created when it does not exist.
 *
 * @return
 *  - 0 on success
 *  - -1 if the virtual address has no page table on the requested level and neither PSE is enabled nor \c create is true
 *  - 1 or 2 (corresponding to PAGE_LVL_PDPT and PAGE_LVL_PDT) when there is no page table on
 *    the requested level because of PSE. In this case, the \c table_paddr is set to the last page table encountered on the page walk.
 * @remark This functions requires that the page tables are identity mapped.
 */
int paging_get_table(uintptr_t vaddr, uintptr_t* table_paddr, int level, int create) {
    DEBUGF("[paging_get_table] V: %llx L: %d C: %d\n", vaddr, level, create);
    if (level == PAGE_LVL_PML4T) {
        *table_paddr = paging_get_pml4t();
        DEBUGF("[paging_get_table] P: %llx L: %d\n", *table_paddr, level);
        return 0;
    }

    // get parent table
    int result = paging_get_table(vaddr, table_paddr, level + 1, create);

    if(result == 0) {

        // ATTENTION: assuming that it's identity mapped
        uint64_t* ptable = (uint64_t*)(*table_paddr);

        uint64_t offset = PAGE_TABLE_INDEX(vaddr, level+1);
        uint64_t entry = ptable[offset];

        DEBUGF("[paging_get_table] E: %llx L: %d I: %d\n", entry, level, offset);

        if (PAGE_IS_PRESENT(entry)) {
            if(PAGE_IS_PSE(entry)) {
                DEBUGF("[paging_get_table] PSE\n");
                // leave table_paddr unmodified
                result = level + 1;
            } else {
                *table_paddr = PAGE_MASK_PADDR(entry);
                result = 0;
                DEBUGF("[paging_get_table] P: %llx L: %d\n", *table_paddr, level);
            }
        } else if (create) {
            // ATTENTION: assuming that it's identity mapped
            uint64_t* newtable = pheap_page_alloc(0x1000);
            memset(newtable, 0, 0x1000);
            uintptr_t newtable_paddr = (uintptr_t) newtable;
            // build entry
            entry = PAGE_MASK_PADDR(newtable_paddr) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITABLE;
            // set entry
            ptable[offset] = entry;
            *table_paddr = newtable_paddr;
            DEBUGF("[paging_get_table] N: %llx V: %llx E: %llx L: %d\n", newtable, vaddr, entry, level);
        } else {
            // table does not exist
            result = -1;
        }

    }
    return result;
}

/**
 * @brief Returns the entry responsible for the given virtual address in the page table on the requested level.
 *
 * @param vaddr the virtual address to use for the page table walk.
 * @param entry receives the entry of the requested page table on success.
 * @param level the level of the page table of interest
 * @param create if nonzero, the requested page table is created when it does not exist.
 */
int paging_get_entry(uintptr_t vaddr, uintptr_t* entry, int level, int create) {
    uintptr_t table;
    int result = paging_get_table(vaddr, &table, level, create);

    if(result < 0) {
        *entry = 0;
        return result;
    } else {
        // ATTENTION: assuming that it's identity mapped
        uint64_t* ptable = (uint64_t*)table;

        uint64_t offset = PAGE_TABLE_INDEX(vaddr, level);
        *entry = ptable[offset];
        return result;
    }
}

/**
 * @brief Walks the page table and returns the physical address which \c vaddr is mapped to.
 */
uintptr_t paging_get_physical_addr(uintptr_t vaddr) {
    uintptr_t entry;
    int result = paging_get_entry(vaddr, &entry, PAGE_LVL_PT, 0);
    if(result >= 0 && PAGE_IS_PRESENT(entry)) {
        // round vaddr to nearest page
        uintptr_t vaddr_pa = vaddr & ~(PAGE_LVL_SIZE(result) - 1);
        uintptr_t paddr = PAGE_MASK_PADDR(entry) + (vaddr - vaddr_pa);
        return paddr;
    } else {
        return INVALID_PTR;
    }
}

/**
 * @brief Creates a mapping from the virtual to the physical address on the given level.
 * Levels other than 0 (PAGE_LVL_PT) are only valid if PSE is enabled.
 *
 * @param vaddr virtual address that should be mapped to \c paddr
 * @param paddr the physical address
 * @param level the level of the mapping (0 -> 4 KB, 1 -> 2 MiB, 2 -> 1 GiB)
 * @param flags the page table entry flags
 *
 * @return
 *  - -1 if the mapping already exists
 *  - 0 on success
 */
int paging_map(uintptr_t vaddr, uintptr_t paddr, int level, uint64_t flags) {
    kassertf(level >= 0 && level <= 2, "paging_map level out of range: %d", level);

    vaddr &= ~(PAGE_LVL_SIZE(level) - 1);
    paddr &= ~(PAGE_LVL_SIZE(level) - 1);

    DEBUGF("[paging_map] mapping %llx to %llx on level %d\n", vaddr, paddr, level);

    uintptr_t table_paddr;
    int result = paging_get_table(vaddr, &table_paddr, level, 1);
    if(result == 0) {
        DEBUGF("[paging_map] parent page table: %llx\n", table_paddr);
        // ATTENTION: assuming that it's identity mapped
        uint64_t* ptable = (uint64_t*)table_paddr;

        uint64_t offset = PAGE_TABLE_INDEX(vaddr, level);
        uint64_t* entry = &ptable[offset];
        DEBUGF("[paging_map] E: %llx I: %llx\n", *entry, offset);

        if(PAGE_IS_PRESENT(*entry)) {
            return -1;
        } else {
            *entry = PAGE_MASK_PADDR(paddr) | PAGE_MASK_FLAGS(flags) | PAGE_FLAG_PRESENT;
            paging_invalidate(vaddr);
            DEBUGF("[paging_map] new entry[%d] = %llx\n", (int)offset, *entry);
            return 0;
        }
    } else {
        return -1;
    }
}

/**
 * @brief Unmaps the page referenced by the given virtual address
 *
 * @param the virtual address to unmap
 *
 * @return
 *  - 0 on success
 *  - -1 if the virtual address was not mapped
 */
int paging_unmap(uintptr_t vaddr) {
    uintptr_t table_paddr;
    int result = paging_get_table(vaddr, &table_paddr, PAGE_LVL_PT, 0);
    if(result >= 0) {
        // ATTENTION: assuming that it's identity mapped
        uint64_t* ptable = (uint64_t*)table_paddr;

        uint64_t offset = PAGE_TABLE_INDEX(vaddr, result);
        ptable[offset] = 0;
        return 0;
    } else {
        return -1;
    }
}

/**
 * @brief invalidates the TLB entry for the virtual address
 * @param vaddr the invalidated virtual address
 */
void paging_invalidate(uintptr_t vaddr)
{
    vaddr &= ~0xFFF;
    asm volatile ("invlpg %0" : : "m" (vaddr));
}
