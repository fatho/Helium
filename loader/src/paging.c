#include <stdint.h>
#include "string.h"


// memory for page tables is reserverd in linker script
extern uint64_t page_pml4t[512];
extern uint64_t page_pdpt[512];
extern uint64_t page_pdt[512];
extern uint64_t page_pt[512];

// get starting address of page containing physical address x
#define PAGE_FLOOR(x) (((uint32_t)x) & 0x000FFFFFFFFFF000)

#define PAGE_FLAG_PRESENT  (1<<0)
#define PAGE_FLAG_WRITABLE (1<<1)

#define PAGE_LVL_PML4T 3
#define PAGE_LVL_PDPT  2
#define PAGE_LVL_PDT   1
#define PAGE_LVL_PT    0

/**
 * Initializes paging by enabling PAE and setting CR3 to the PML4T.
 * Identity maps the first 2 MB.
 */
void setup_paging() {
    // clear page tables
    memset(page_pml4t, 0, 4096);
    memset(page_pdpt, 0, 4096);
    memset(page_pdt, 0, 4096);
    memset(page_pt, 0, 4096);

    // link first three tables to next one
    page_pml4t[0] = PAGE_FLOOR(page_pdpt) | PAGE_FLAG_WRITABLE | PAGE_FLAG_PRESENT;
    page_pdpt[0] = PAGE_FLOOR(page_pdt) | PAGE_FLAG_WRITABLE | PAGE_FLAG_PRESENT;
    page_pdt[0] = PAGE_FLOOR(page_pt) | PAGE_FLAG_WRITABLE | PAGE_FLAG_PRESENT;
    // fill first PT with identity mapping
    for(int i = 0; i < 0x200; i += 1) {
        page_pt[i] = PAGE_FLOOR(i << 12) | PAGE_FLAG_WRITABLE | PAGE_FLAG_PRESENT;
    }

    // load PML4T in cr3
    asm volatile ("movl %%edi, %%cr3" : : "D" (page_pml4t) : );

    // enable PAE
    asm volatile (
            "movl %%cr4, %%eax\n"
            "or 1<<5, %%eax\n"
            "movl %%eax, %%cr4" : : :
            );
}

uint64_t* get_page_entry(uint64_t virtual, uint8_t level) {

}
