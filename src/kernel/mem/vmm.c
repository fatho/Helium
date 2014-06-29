/**
 * @file vmm.c
 *
 * @author fabian
 * @date   15.06.2014
 *
 * @brief Virtual Memory Manager
 */

#include "kernel/mem/vmm.h"
#include "kernel/mem/pfa.h"

#include "kernel/helium.h"

#include "kernel/bits.h"

#include "kernel/debug.h"
#include "kernel/panic.h"

#include "kernel/klibc/kstdio.h"
#include "kernel/klibc/string.h"

#include "kernel/interrupts/idt.h"

/// PF handler written in assembly
extern marker_t vmm_pf_handler_asm;
/// GPF handler written in assembly
extern marker_t vmm_gpf_handler_asm;

/**
 * @brief page fault handler called from assembly
 */
void vmm_pf_handler() {
    uintptr_t cr2;
    asm volatile ("movq %%cr2,%0" : "=g"(cr2));
    //kpanicf("page fault while accessing %p\n", cr2);
    DEBUGF("page fault while accessing %p\n", cr2);
    __halt__();
}

/**
 * @brief general protection fault handler called from assembly
 */
void vmm_gpf_handler() {
    uintptr_t cr2;
    asm volatile ("movq %%cr2,%0" : "=g"(cr2));
    //kpanicf("general protection fault while accessing %p\n", cr2);
    DEBUGF("general protection fault while accessing %p\n", cr2);
    __halt__();
}

/**
 * @brief returns the physical address of the current PML4T
 */
uintptr_t vmm_get_pml4t() {
    uintptr_t pml4t;
    asm volatile ("movq %%cr3,%0" : "=g"(pml4t));
    return pml4t;
}

/**
 * @brief Sets the current PML4T.
 *
 * @param Physical address of PML4T.
 * @remark Caution, only use a root page table where
 * kernel space is mapped to the same location.
 *
 * Calling this function also flushes the TLB.
 */
void vmm_set_pml4t(uintptr_t pml4t) {
    asm volatile ("movq %0,%%cr3" : "=g"(pml4t));
}

/**
 * @brief Invalidates the given page.
 * @param Virtual address of the invalidated page.
 */
void vmm_invalidate(uintptr_t page) {
    asm volatile ("invlpg %0" :: "g"(page));
}

/**
 * @brief Allocates a (cleared) page frame for a page table using the kernel's page fram allocator.
 * @return the physical address of the allocated page frame.
 */
static uintptr_t vmm_alloc_pt() {
    uintptr_t ptaddr = pfa_alloc();
    memset(VMM_LINEAR_PT(ptaddr), 0, VMM_PAGE_SIZE);
    return ptaddr;
}

/**
 * @brief
 *
 * @param pml4t_p Physical address of PML4T.
 * @param vaddr canonical virtual address for which the responsible page table should be returned
 * @param level Level of the page table to get.
 * @param createFlags Flags for newly created page tables. If this parameter is zero, no pages are created.
 * @param table_paddr Receives physical address of the page table.
 * @param parent_entry Receives pointer to the parent entry of the page table returned in \c table_paddr
 *
 * @return
 *      - -1 if the requested page table is not present
 *      - requested \c level or higher, in case a page size bit is set.
 */
int vmm_get_table(uintptr_t pml4t_p, uintptr_t vaddr, int level, uint64_t createFlags, uintptr_t* table_paddr, uint64_t** parent_entry) {
    int levelReached;
    uintptr_t ptphys = pml4t_p;
    // parent entry of current table
    uint64_t* parent = 0;

    // iterate over levels from PML4T to requested level
    for(levelReached = VMM_LEVEL_4; levelReached > level; levelReached--) {
        kassertf(ptphys < 0x100000000, "page table %p not allocated in the lower 4 GiB\n", ptphys);
        uint64_t* ptlin = VMM_LINEAR_PT(ptphys);
        // read entry
        int index = (vaddr >> (12 + 9 * levelReached)) & 0x1FF;
        uint64_t entry = ptlin[index];

        if(HAS_FLAG(entry, VMM_FLAG_PRESENT)) {
            if(HAS_FLAG(entry, VMM_FLAG_SIZE)) {
                // large pages are used, stop traversal here
                break;
            } else {
                // descend to the next level
                ptphys = VMM_PT_ADDR(entry);
            }
        } else {
            if(createFlags) {
                // allocate new page table
                uintptr_t ptnewphys = vmm_alloc_pt();
                if(ptnewphys == 0) {
                    kpanic("Failed to allocate new page table!\n");
                }
                // PML4T is not reference counted
                if(parent) {
                    // increase reference count in parent entry
                    VMM_REFCOUNT_CAN_INC(*parent);
                    *parent = VMM_REFCOUNT_INC(*parent);
                }
                // install entry in current PT
                ptlin[index] = VMM_PT_ADDR(ptnewphys) | createFlags;
            } else {
                // page not present
                levelReached = -1;
                break;
            }
        }
        parent = &ptlin[index];
    }
    *table_paddr = ptphys;
    if(parent_entry)
        *parent_entry = parent;
    return levelReached;
}

/**
 * @brief Maps a virtual address to a physical page.
 *
 * @param pml4t_p physical address of PML4T
 * @param paddr Physical address of the page frame.
 * @param vaddr Virtual address where the page frame should be mapped
 * @param level The level of the page mapping
 *  - \c VMM_LEVEL_1: 4 KiB
 *  - \c VMM_LEVEL_2: 2 MiB
 *  - \c VMM_LEVEL_3: 1 GiB
 *  - \c VMM_LEVEL_4: not supported
 * @param createFlags Flags for mapping newly created page tables. If this parameter is zero, no new page tables are created.
 * @param mapFlags Flags of the mapping.
 *
 * @return
 *  - \c -1 when a page table in the traversal does not exist
 *  - \c -2 when the virtual address is already mapped
 *  - \c -3 when an invalid argument was supplied
 *  - \c 0 on success
 *
 * @remark The physical address must be aligned to page size associated with the requested level.
 *
 * @todo Revisit when swapping is implemented, because currently, only the present bit is checked.
 */
int vmm_map(uintptr_t pml4t_p, uintptr_t paddr, uintptr_t vaddr, int level, uint64_t createFlags, uint64_t mapFlags) {
    if(level == VMM_LEVEL_4) {
        return -3;
    }
    // get page table
    uintptr_t table_paddr;
    uint64_t* parent;
    int ret = vmm_get_table(pml4t_p, vaddr, level, createFlags, &table_paddr, &parent);
    if(ret == level) {
        // page table exists on the requested level
        uint64_t* table = VMM_LINEAR_PT(table_paddr);
        int index = (vaddr >> (12 + 9 * level)) & 0x1FF;
        if(HAS_FLAG(table[index], VMM_FLAG_PRESENT)) {
            // already mapped
            return -2;
        }
        // PML4T is not reference counted
        if(parent) {
            // increase reference count in parent entry
            VMM_REFCOUNT_CAN_INC(*parent);
            *parent = VMM_REFCOUNT_INC(*parent);
        }
        // create entry
        table[index] = VMM_PT_ADDR(paddr) | mapFlags;
        return 0;
    } else {
        // page table does not exist on the requested level
        return -1;
    }
}

/**
 * @brief Unmaps a virtual address previously mapped with #vmm_map
 *
 * @param pml4t_p Physical address of the PML4T.
 * @param vaddr Virtual address to unmap.
 *
 * @todo Revisit when implementing swapping because unmapping must take into account
 * that the page to unmap is currently swapped out.
 */
int vmm_unmap(uintptr_t pml4t_p, uintptr_t vaddr) {
    // get page table
    uintptr_t table_paddr;
    uint64_t* parent;
    int ret = vmm_get_table(pml4t_p, vaddr, VMM_LEVEL_1, 0, &table_paddr, &parent);
    if(ret >= VMM_LEVEL_1) {
        // page table exists, remove mapping on lowest level
        // proceed upwards when reference count drops to zero
        uint64_t* table = VMM_LINEAR_PT(table_paddr);
        int index = (vaddr >> (12 + 9 * ret)) & 0x1FF;
        if(HAS_FLAG(table[index], VMM_FLAG_PRESENT)) {
            // PML4T is not reference counted
            if(parent) {
                // increase reference count in parent entry
                VMM_REFCOUNT_CAN_DEC(*parent);
                *parent = VMM_REFCOUNT_DEC(*parent);
            }
            // clear entry
            table[index] = 0;
        }
    } else {
        // page table does not exist on the requested level
        return -1;
    }
    return 0;
}

/**
 * @brief Initializes the virtual memory manager.
 */
void vmm_init() {
    DEBUGF("[vmm_init]\n");
    DEBUGF("  PF handler:  %p\n", &vmm_pf_handler_asm);
    DEBUGF("  GPF handler: %p\n", &vmm_gpf_handler_asm);
    // install page fault handler, needed for virtual memory management
    idt_set_entry(IDT_VEC_PAGE_FAULT, IDT_INT_GATE, (uintptr_t)&vmm_pf_handler_asm, 0x8, 0);
    idt_set_entry(IDT_VEC_GEN_PROT_FAULT, IDT_INT_GATE, (uintptr_t)&vmm_gpf_handler_asm, 0x8, 0);

    // find kernel page tables
    uintptr_t pml4t = vmm_get_pml4t();
    DEBUGF("  kernel mappings:\n");
    for(int level = VMM_LEVEL_4; level >= 0; level--) {
        uintptr_t table;
        int ret = vmm_get_table(pml4t, 0xFFFFFFFF80100000, level, 0, &table, 0);
        DEBUGF("  %d (%d): %p\n", level, ret, table);
    }
}
