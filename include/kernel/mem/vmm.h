/**
 * @file vmm.h
 *
 * @author fabian
 * @date   15.06.2014
 *
 * @brief 
 */
#ifndef VMM_H_
#define VMM_H_

/// virtual base address of kernel space
#define VMM_KERNEL_BASE 0xFFFFFFFF80000000

/// virtual base address of the first 4GiB of memory
#define VMM_PHYS4G_BASE 0xFFFFFF8000000000

/// fourth and currently highest level of page tables. (PML4T on x86_64)
#define VMM_LEVEL_4 3
/// third level of page tables. (PDPT on x86_64)
#define VMM_LEVEL_3 2
/// second level of page tables. (PDT on x86_64)
#define VMM_LEVEL_2 1
/// first level of page tables. (PT on x86_64)
#define VMM_LEVEL_1 0

/// size of normal pages (4K)
#define VMM_PAGE_SIZE 0x1000
/// size of "huge" pages (2M)
#define VMM_HUGE_PAGE_SIZE 0x200000

/**
 * @brief calculates the linear address of a page table.
 * @param pt physical address of the page table.s
 * @remark assumes, that the page table is allocated in the first 4GB of physical memory.
 */
#define VMM_LINEAR_PT(pt) ((uint64_t*)((pt) + VMM_PHYS4G_BASE))

/// page table entry "GLOBAL" flag
#define VMM_FLAG_GLOBAL 0x100
/// page table entry "SIZE" flag
#define VMM_FLAG_SIZE 0x80
/// page table entry "DIRTY" flag. Set by CPU on page writes.
#define VMM_FLAG_DIRTY 0x40
/// page table entry "ACCESSED" flag. Set by CPU on page access.
#define VMM_FLAG_ACCESSED 0x20
/// page table entry "USER" flag
#define VMM_FLAG_USER 0x4
/// page table entry "READ/WRITE" flag
#define VMM_FLAG_RW 0x2
// page table entry "PRESENT" flag
#define VMM_FLAG_PRESENT 0x1

/// Returns the reference count stored in the page table entry.
#define VMM_REFCOUNT_GET(entry) (((entry) & 0x7FC0000000000000) >> 54)
/// Returns the reference count stored in the page table entry.
#define VMM_REFCOUNT_SET(entry,count) (((entry) & ~0x7FC0000000000000) | ((count) & 0x1FF) << 54)
/// Increments the reference count stored in the page table entry.
#define VMM_REFCOUNT_INC(entry) ((entry) + 0x0040000000000000)
/// Increments the reference count stored in the page table entry.
#define VMM_REFCOUNT_DEC(entry) ((entry) - 0x0040000000000000)

#ifdef ASSERTIONS
/// asserts that the reference count in the entry can be safely decreased
#define VMM_REFCOUNT_CAN_DEC(entry) kassert(VMM_REFCOUNT_GET(entry) >= 1);
/// asserts that the reference count in the entry can be safely increased
#define VMM_REFCOUNT_CAN_INC(entry) kassert(VMM_REFCOUNT_GET(entry) < 0x7FF);
#else
/// assertion disabled
#define VMM_REFCOUNT_CAN_DEC(entry)
/// assertion disabled
#define VMM_REFCOUNT_CAN_INC(entry)
#endif

/// Returns the address from a page table entry.
#define VMM_PT_ADDR(entry) ((entry) & 0x000FFFFFFFFFF000)

/**
 * @brief Initializes the virtual memory manager.
 */
void vmm_init();

#endif /* VMM_H_ */
