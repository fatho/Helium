/*
 * paging.h
 *
 *  Created on: 09.05.2014
 *      Author: fabian
 */

#ifndef PAGING_H_
#define PAGING_H_

#define PAGE_SIZE 0x1000

#define PAGE_CEIL(x) (((uintptr_t)x + PAGE_SIZE-1) & ~(PAGE_SIZE-1))
#define PAGE_FLOOR(x) ((uintptr_t)x & ~(PAGE_SIZE-1))

#define PAGE_FLAG_PRESENT  (1<<0)
#define PAGE_FLAG_WRITABLE (1<<1)

#define PAGE_IS_PRESENT(p)  (p&PAGE_FLAG_PRESENT)
#define PAGE_IS_WRITABLE(p) (p&PAGE_FLAG_WRITABLE)

#define PAGE_LVL_PML4T 3
#define PAGE_LVL_PDPT  2
#define PAGE_LVL_PDT   1
#define PAGE_LVL_PT    0

void setup_page_tables();

#endif /* PAGING_H_ */
