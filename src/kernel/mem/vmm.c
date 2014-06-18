/**
 * @file vmm.c
 *
 * @author fabian
 * @date   15.06.2014
 *
 * @brief Virtual Memory Manager
 */

#include "kernel/debug.h"
#include "kernel/panic.h"

#include "kernel/klibc/kstdio.h"

#include "kernel/interrupts/idt.h"

extern uint8_t vmm_pf_handler_asm;

//static uintptr_t kernel_heap_base;
//static uintptr_t kernel_heap_top;

void vmm_pf_handler() {
    uintptr_t cr2;
    asm volatile ("movq %%cr2,%0" : "=a"(cr2));
    kpanicf("page fault while accessing %p\n", cr2);
}


/**
 * @brief Initializes the virtual memory manager.
 */
void vmm_init() {
    DEBUGF("[vmm_init]\n");
    DEBUGF("  install PF handler: %p\n", &vmm_pf_handler_asm);
    // install page fault handler, needed for virtual memory management
    idt_set_entry(IDT_VEC_PAGE_FAULT, IDT_INT_GATE, (uintptr_t)&vmm_pf_handler_asm, 0x8, 0);
}
