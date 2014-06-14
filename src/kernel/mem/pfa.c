/**
 * @file alloc.c
 *
 * @author fabian
 * @date   05.06.2014
 *
 * @brief Implements a page frame allocator. Currently the watermark allocator is used.
 * @todo Implement a more sophisticated page frame allocator
 */

#include "kernel/mem/pfa.h"

#include "kernel/debug.h"
#include "kernel/helium.h"
#include "kernel/info.h"

static uintptr_t watermark = 0;
static size_t mmap_idx = 0;

/**
 * @brief Initialize page frame allocator.
 */
void pfa_init() {
    watermark = info_table.kernel_top_paddr;

    // search for memory region that contains the watermark
    for (mmap_idx = 0; mmap_idx < info_table.mmap_count; mmap_idx++) {
        he_mmap_t* region = &info_table.mmap_table[mmap_idx];
        if (watermark >= region->base
                && watermark <= region->base + region->length) {
            break;
        }
    }

    kassertf(mmap_idx < info_table.mmap_count, "watermark not inside any memory region");
}

/**
 * @brief Allocates a contiguous block of page frames.
 * @param num Number of page frames requested.
 * @param align Returned blocks are aligned to 2^align page frames.
 *
 * @return Returns the physical address of the first page in the allocated block.
 * Returns null if no block of the requested size is available.
 */
uintptr_t pfa_alloc_block(size_t num, size_t align) {
    kassertf(watermark, "[pfa_alloc_block] never called pfa_init");
    // cannot allocate empty block
    if (num == 0) {
        return 0;
    }
    // align watermark to requested alignment
    uintptr_t mask = (2 << (12 + align)) - 1;
    watermark = (watermark + mask) & ~mask;

    // allocate num 4K pages
    size_t size = num << 12;
    uintptr_t allocated = 0;

    // check mmap
    he_mmap_t* region = &info_table.mmap_table[mmap_idx];
    if (watermark >= region->base
            && watermark + size < region->base + region->length) {
        // allocated block fits in current memory region
        allocated = watermark;
    } else {
        // use next available memory region
        for (mmap_idx++; mmap_idx < info_table.mmap_count; mmap_idx++) {
            region = &info_table.mmap_table[mmap_idx];
            uintptr_t aligned_base = (region->base + mask) & ~mask;
            if (region->available
                    && aligned_base + size <= region->base + region->length) {
                watermark = aligned_base;
                allocated = watermark;
                break;
            }
        }
    }

    if (allocated) {
        watermark += size;
    }
    return allocated;
}

/**
 * @brief Frees a block previously allocated with #pfa_free_block
 */
void pfa_free_block(uintptr_t blockaddr, size_t npf) {
    UNUSED(blockaddr);
    UNUSED(npf);
    // a watermark allocator never frees
    return;
}

/**
 * @brief Allocates a single 4K page frame. The allocator never returns pages below the 1 MB mark.
 * @return The address of the allocated page frame.
 * Returns null if the allocation failed.
 */
uintptr_t pfa_alloc() {
    return pfa_alloc_block(1, 0);
}

/**
 * @brief Frees a page frame previously allocated with #pfa_alloc.
 * @param pageaddr The page frame address returned from #page_frame_alloc.
 */
void pfa_free(uintptr_t pageaddr) {
    pfa_free_block(pageaddr, 1);
}
