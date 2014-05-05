/*
 * idt.h
 *
 *  Created on: 05.05.2014
 *      Author: fabian
 */

#pragma once

#include <stdint.h>

/**
 * An entry in the 64 bit Interrupt Descriptor Table
 */
typedef struct {
    uint16_t id_offset_low;
    uint16_t id_selector;
    uint8_t  id_zero1;
    uint8_t  id_attr;
    uint16_t id_offset_middle;
    uint32_t id_offset_high;
    uint32_t id_zero2;
} idt64_t;
