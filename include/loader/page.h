/*
 * paging.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef PAGING_H_
#define PAGING_H_

/// Small page size (4KB)
#define PAGE_SIZE 0x1000
/// size of page table (4KB)
#define PAGE_TABLE_SIZE 0x1000
/// number of entries in page table (#PAGE_TABLE_SIZE / \c 8)
#define PAGE_TABLE_ENTRIES (2<<9)

/// Returns the smallest page aligned integer value greater than or equal to \c x
#define PAGE_CEIL(x) (((uintptr_t)x + PAGE_SIZE-1) & ~(PAGE_SIZE-1))
/// Returns the greates page aligned integer value lesser than or equal to \c x
#define PAGE_FLOOR(x) ((uintptr_t)x & ~(PAGE_SIZE-1))

/// PRESENT bit in page table entry
#define PAGE_FLAG_PRESENT  (1<<0)
/// WRITABLE bit in page table entry
#define PAGE_FLAG_WRITABLE (1<<1)

/// checks if the given page table entry is present
#define PAGE_IS_PRESENT(p)  (p&PAGE_FLAG_PRESENT)
/// checks if the given page table entry is writable
#define PAGE_IS_WRITABLE(p) (p&PAGE_FLAG_WRITABLE)
/// returns the physical address referenced in the given page table entry.
#define PAGE_ENTRY_ADDR_4K(p)  (((p>>12)&((1L<<40)-1)) << 12)

/// calculates the page table index for the given \c vaddr and \c level.
#define PAGE_TABLE_INDEX(vaddr,level) ((vaddr >> (12 + level * 9)) & (PAGE_TABLE_ENTRIES-1))

/// Page Map Level 4 Table
#define PAGE_LVL_PML4T 3
/// Page Directory Pointer Table
#define PAGE_LVL_PDPT  2
/// Page Directory Table
#define PAGE_LVL_PDT   1
/// Page Table
#define PAGE_LVL_PT    0

#endif /* PAGING_H_ */
