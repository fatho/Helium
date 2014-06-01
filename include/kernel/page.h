/*
 * paging.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef PAGING_H_
#define PAGING_H_

#include <stdint.h>

/// Small page size (4KB)
#define PAGE_SIZE 0x1000
/// size of page table (4KB)
#define PAGE_TABLE_SIZE 0x1000
/// number of entries in page table (#PAGE_TABLE_SIZE / \c 8)
#define PAGE_TABLE_ENTRIES (1<<9)

#define PAGE_SIZE 0x1000
/// Returns the smallest page aligned integer value greater than or equal to \c x
#define PAGE_CEIL(x) (((uintptr_t)(x) + PAGE_SIZE-1) & ~(PAGE_SIZE-1))
/// Returns the greates page aligned integer value lesser than or equal to \c x
#define PAGE_FLOOR(x) ((uintptr_t)(x) & ~(PAGE_SIZE-1))

/// PRESENT bit in page table entry
#define PAGE_FLAG_PRESENT  (1<<0)
/// WRITABLE bit in page table entry
#define PAGE_FLAG_WRITABLE (1<<1)
/// USER bit in page table entry
#define PAGE_FLAG_USER (1<<2)
/// WRITE-THROUGH bit in page table entry
#define PAGE_FLAG_WRITE_TROUGH (1<<3)
/// CACHE-DISASBLE bit in page table entry
#define PAGE_FLAG_CACHE_DISABLE (1<<4)
/// USED bit in page table entry. Set by the CPU.
#define PAGE_FLAG_USED (1<<5)
/// PSE bit in page table entry.
#define PAGE_FLAG_PSE (1<<7)
/// GLOBAL bit in page table entry.
#define PAGE_FLAG_GLOBAL (1<<8)

/// checks if the given page table entry is present
#define PAGE_IS_PRESENT(p)  ((p)&PAGE_FLAG_PRESENT)
/// checks if the given page table entry is writable
#define PAGE_IS_WRITABLE(p) ((p)&PAGE_FLAG_WRITABLE)
/// checks if the given page table entry is accessible from usermode
#define PAGE_IS_USER(p) ((p)&PAGE_FLAG_USER)
/// checks if the given page table entry uses PSE.
#define PAGE_IS_PSE(p) ((p)&PAGE_FLAG_PSE)
/// returns the physical address referenced in the given page table entry.
#define PAGE_MASK_PADDR(p)  ((p) & 0x000FFFFFFFFFF000L)
/// returns the flags of the given page table entry
#define PAGE_MASK_FLAGS(p)  ((p) & 0xFFF0000000000FFFL)

/// calculates the page table index for the given \c vaddr and \c level.
#define PAGE_TABLE_INDEX(vaddr,level) (((vaddr) >> (12 + (level) * 9)) & 0x1FFL)
/// calculates the size of a page when addressed by the given level
#define PAGE_LVL_SIZE(lvl) (1<<(12 + (lvl) * 9))

/// Page Map Level 4 Table
#define PAGE_LVL_PML4T 3
/// Page Directory Pointer Table
#define PAGE_LVL_PDPT  2
/// Page Directory Table
#define PAGE_LVL_PDT   1
/// Page Table
#define PAGE_LVL_PT    0

/**
 * @brief reads \c CR3 which contains the physical address of the PML4T.
 *
 * @return physical address of the PML4T
 */
uintptr_t paging_get_pml4t();
/**
 * @brief writes physical address of the PML4T to \c CR3.
 *
 * @param pml4t physical address of the PML4T
 */
void paging_set_pml4t(uintptr_t pml4t);
/**
 * @brief Returns physical address of the page table for the given virtual address on the specified level.
 */
int paging_get_table(uintptr_t vaddr, uintptr_t* table_paddr, int level, int create);
/**
 * @brief Returns the entry responsible for the given virtual address in the page table on the requested level.
 */
int paging_get_entry(uintptr_t vaddr, uintptr_t* entry, int level, int create);
/**
 * @brief Returns the entry responsible for the given virtual address in the page table on the requested level.
 */
int paging_get_entry(uintptr_t vaddr, uintptr_t* entry, int level, int create);
/**
 * @brief Walks the page table and returns the physical address which \c vaddr is mapped to.
 */
uintptr_t paging_get_physical_addr(uintptr_t vaddr);
/**
 * @brief Creates a mapping from the virtual to the physical address on the given level.
 */
int paging_map(uintptr_t vaddr, uintptr_t paddr, int level, uint64_t flags);
/**
 * @brief Unmaps the page referenced by the given virtual address
 */
int paging_unmap(uintptr_t vaddr);
/**
 * @brief invalidates the TLB entry for the virtual address
 */
void paging_invalidate(uintptr_t vaddr);

#endif /* PAGING_H_ */
