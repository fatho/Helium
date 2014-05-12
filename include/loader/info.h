#ifndef INFO_H_
#define INFO_H_

#include "idt.h"
#include <helium.h>

/// pre-allocated IDT table
extern idt64_t idt_data[HE_IDT_MAX_ENTRIES];
/// pre-allocated GDT for long mode
extern uint64_t gdt_data[HE_GDT_MAX_ENTRIES];

extern he_info_t info_table;
extern he_module_t info_modules[256];
extern he_mmap_t info_mmap[256];
extern char info_strings[HE_STRING_TABLE_SIZE];

#endif INFO_H_
