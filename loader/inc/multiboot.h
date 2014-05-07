#ifndef MULTIBOOT_H_
#define MULTIBOOT_H_

#include <stdint.h>

typedef struct {
    uint32_t addr_start;
    uint32_t addr_end;
    uint32_t name;
    uint32_t reserved;
}__attribute__((packed)) multiboot_mod_t;

typedef struct {
    uint32_t tab_size;
    uint32_t str_size;
    uint32_t table;
    uint32_t reserved;
}__attribute__((packed)) aout_symbol_table_t;

typedef struct {
    uint32_t num;
    uint32_t size;
    uint32_t table;
    uint32_t strtable;
}__attribute__((packed)) elf_symbol_table_t;

typedef struct {
    uint32_t size;
    uint64_t base;
    uint64_t length;
    uint32_t type;
}__attribute__((packed)) multiboot_mmap_t;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods;
    union {
        aout_symbol_table_t aout;
        elf_symbol_table_t elf;
    } syms;
    uint32_t mmap_len;
    uint32_t mmap;
}__attribute__((packed)) multiboot_info_t;

#endif
