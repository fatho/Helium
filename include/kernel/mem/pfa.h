/**
 * @file pfa.h
 *
 * @author Fabian Thorand
 * @date   05.06.2014
 *
 * @brief Public interface of page frame allocator.
 */
#ifndef PFA_H_
#define PFA_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize page frame allocator.
 */
void pfa_init();

/**
 * @brief Allocates a contiguous block of page frames.
 * @param num Number of page frames requested.
 * @param align Returned blocks are aligned to 2^align page frames.
 *
 * @return Returns the physical address of the first page in the allocated block.
 * Returns null if no block of the requested size is available.
 */
uintptr_t pfa_alloc_block(size_t num, size_t align);

/**
 * @brief Frees a block previously allocated with #pfa_free_block
 */
void pfa_free_block(uintptr_t blockaddr, size_t npf);

/**
 * @brief Allocates a single 4K page frame. The allocator never returns pages below the 1 MB mark.
 * @return The address of the allocated page frame.
 * Returns null if the allocation failed.
 */
uintptr_t pfa_alloc();

/**
 * @brief Frees a page frame previously allocated with #pfa_alloc.
 * @param pageaddr The page frame address returned from #page_frame_alloc.
 */
void pfa_free(uintptr_t pageaddr);

#endif /* PFA_H_ */
