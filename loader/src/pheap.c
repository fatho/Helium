/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains a basic heap allocation system, which simply increases the heap top
 * with each allocation and does not handle freeing of memory blocks.
 */

#include "loader/pheap.h"
#include "loader/kassert.h"
#include "loader/paging.h"
#include "loader/util.h"

#include <stddef.h>
#include <stdint.h>


// the current top of the heap. not available directly to other modules, access only possible via
// the functions defined in this module.
static uintptr_t pheap_top = INVALID_PTR;

/**
 * @brief Allocates as many consecutive physical pages as needed to hold at least \c size bytes.
 *
 * @param size the minimum size of the returned memory block.
 * @return the (page aligned) address to the first page of the allocated memory block. \c INVALID_PTR on failure.
 * @remark Behavior for zero sized allocations is not specified.
 * @todo verify in MMAP that the allocation is possible
 */
void* pheap_page_alloc(size_t size) {
    kassert(pheap_top != INVALID_PTR,
            "physical heap not initialized");

    size = PAGE_CEIL(size);
    void* allocated = (void*) pheap_top;
    pheap_top += size;
    return allocated;
}

/**
 * @brief Returns the current top of the heap.
 */
uintptr_t pheap_get_top() {
    return pheap_top;
}

/**
 * @brief Initializes the basic physical memory heap.
 *
 * @param free_paddr The address of the first page after the loader image.
 *
 * @return one of the following exit codes:
 *
 * @remark Although it should not fail, it may raise a kernel panic on failure.
 */
void pheap_init(uintptr_t free_paddr) {
    kassertf(free_paddr != INVALID_PTR && free_paddr != 0,
            "invalid pointer passed to page_alloc_init: %llx", free_paddr);

    pheap_top = PAGE_CEIL(free_paddr);
}
