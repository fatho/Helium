/**
 * @file
 * @date June 02, 2014
 * @author Fabian Thorand
 *
 * @brief Contains functions to manage the IDT.
 */


#include "kernel/interrupts/idt.h"
#include "kernel/helium.h"
#include "kernel/klibc/string.h"

#include <stddef.h>

static idt64_t* idt_base = NULL;

/**
 * @brief Fills an #idt64_t structure with the given values.
 *
 * @param type Either #IDT_INT_GATE or #IDT_TRAP_GATE.
 * The difference is that interrupt gates clear EFLAGS.IF to prevent further interrupts.
 * @param handler The virtual address of the ISR (interrupt service routine)
 * @param cs Code segment to use for executing the handler.
 * @param dpl Privilege level required for calling this interrupt.
 */
static void idt_fill_idt64_t(idt64_t* entry, uint8_t type, uintptr_t handler, uint16_t cs, uint8_t dpl) {
    entry->offset_low = handler & 0xFFFF;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;

    entry->zero1 = 0;
    entry->zero2 = 0;

    entry->selector = cs;

    entry->attr = (type & 0xF) | ((dpl & 0x3) << 5) | IDT_PRESENT_BIT;
}

/**
 * @brief Sets the entry with the given \c index in the current IDT.
 * @see idt_fill_idt64_t
 */
void idt_set_entry(int index, uint8_t type, uintptr_t handler, uint16_t cs, uint8_t dpl) {
    idt_fill_idt64_t(&idt_base[index], type, handler, cs, dpl);
}

/**
 * @brief Loads the IDT located at \c base with \c limit.
 *
 * @brief base Virtual address of the beginning of the IDT
 * @brief limit Address of last byte of IDT relative to the beginning.
 */
static void idt_load(idt64_t* base, uint16_t limit) {
    // load IDT
    idtr64_t idtr;
    idtr.limit = limit;
    idtr.offset = (uint64_t)base;
    asm volatile ("lidt %0" : : "m" (idtr));
}

/**
 * @brief Initializes the IDT for the first use.
 */
void idt_init() {
    // reference to IDT in lower half
    extern idt64_t idt_data[HE_IDT_MAX_ENTRIES];
    // calculate higher half address of IDT
    idt_base = (idt64_t*)((uintptr_t)idt_data + KERNEL_VMA);
    memset(idt_base, 0, HE_IDT_MAX_ENTRIES * 16);

    idt_load(idt_base, HE_IDT_MAX_ENTRIES * 16 - 1);
}
