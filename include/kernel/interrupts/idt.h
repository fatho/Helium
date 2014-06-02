/*
 * idt.h
 *
 *  Created on: 05.05.2014
 *      Author: fabian
 */

#ifndef IDT_H_
#define IDT_H_

#include <stdint.h>

/// Type code for interrupt gate entry in IDT.
#define IDT_INT_GATE 0b110
/// Type code for trap gate entry in IDT.
#define IDT_TRAP_GATE 0b111
/// This bit is set in idt64_t::attr when the entry is present.
#define IDT_PRESENT_BIT 0x80

/**
 * @brief An entry in the 64 bit Interrupt Descriptor Table
 */
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero1;
    uint8_t attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero2;
}__attribute__((packed)) idt64_t;

/**
 * @brief Interrupt Descriptor Table Register
 */
typedef struct {
    uint16_t limit;  ///< maximum addressable byte in IDT
    uint64_t offset; ///< linear base address of IDT
}__attribute__((packed)) idtr64_t;

/**
 * @brief Sets the entry with the given \c index in the current IDT.
 * @see idt_fill_idt64_t
 */
void idt_set_entry(int index, uint8_t type, uintptr_t handler, uint16_t cs, uint8_t dpl);

/**
 * @brief Initializes the IDT for the first use.
 */
void idt_init();

#endif
