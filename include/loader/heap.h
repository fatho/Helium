/*
 * memory.h
 *
 *  Created on: 12.05.2014
 *      Author: fabian
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <stdint.h>
#include <stddef.h>

#define INVALID_PTR 0xFFFFFFFFFFFFFFFF

/**
 * Allocates as many consecutive physical pages as needed to hold at least \c size bytes.
 *
 * @param size the minimum size of the returned memory block.
 * @return the (page aligned) address to the first page of the allocated memory block. \c INVALID_PTR on failure.
 * @remark Behavior for zero sized allocations is not specified.
 */
void* physical_page_alloc(size_t size);

/**
 * @brief Returns the current top of the heap.
 */
uintptr_t get_physical_heap_top();

/**
 * @brief Initializes the basic physical memory heap.
 * @param free_paddr The address of the first page after the loader image.
 */
void physical_heap_init(uintptr_t free_paddr);

#endif /* HEAP_H_ */
