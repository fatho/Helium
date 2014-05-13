/*
 * paging.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef PAGING_H_
#define PAGING_H_

#define PAGE_SIZE 0x1000
#define PAGE_TABLE_SIZE 0x1000
#define PAGE_TABLE_ENTRIES (2<<9)

/// Returns the smallest page aligned integer value greater than or equal to \c x
#define PAGE_CEIL(x) (((uintptr_t)x + PAGE_SIZE-1) & ~(PAGE_SIZE-1))
/// Returns the greates page aligned integer value lesser than or equal to \c x
#define PAGE_FLOOR(x) ((uintptr_t)x & ~(PAGE_SIZE-1))

#define PAGE_FLAG_PRESENT  (1<<0)
#define PAGE_FLAG_WRITABLE (1<<1)

#define PAGE_IS_PRESENT(p)  (p&PAGE_FLAG_PRESENT)
#define PAGE_IS_WRITABLE(p) (p&PAGE_FLAG_WRITABLE)
#define PAGE_ENTRY_ADDR(p)  ((p>>12)&((1<<40)-1))

#define PAGE_TABLE_INDEX(vaddr,level) ((vaddr >> (12 + level * 9)) & (PAGE_TABLE_ENTRIES-1))

#define PAGE_LVL_PML4T 3
#define PAGE_LVL_PDPT  2
#define PAGE_LVL_PDT   1
#define PAGE_LVL_PT    0

#endif /* PAGING_H_ */
