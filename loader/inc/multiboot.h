#pragma once

#include <stdint.h>

typedef struct {
  uint32_t addr_start;
  uint32_t addr_end;
  uint8_t* name;
  uint32_t reserved;
} mb_mod_t;


typedef struct {
  uint32_t tab_size;
  uint32_t str_size;
  void*    table;
  uint32_t reserved;
} mb_sym_out_t;


typedef struct {
  uint32_t num;
  uint32_t size;
  void*    table;
  void*    strtable;
} mb_sym_elf_t;

typedef struct {
  uint32_t size;
  uint64_t base;
  uint64_t length;
  uint32_t type;
} mb_mmap_t;

typedef struct {
  uint32_t size;
  uint8_t  number;
  uint8_t  mode;
  uint16_t cylinders;
  uint8_t  heads;
  uint8_t  sectors;
  void*    rest; // TODO:
} mb_drives_t;

typedef struct {
  uint16_t version;
  uint16_t cseg32;
  uint32_t offset;
  uint16_t cseg16;
  uint16_t dseg16;
  uint16_t flags;
  uint16_t cseg32_len;
  uint16_t cseg16_len;
  uint16_t dseg16_len;
} mb_apm_t;

typedef struct {
  uint32_t     flags;
  uint32_t     mem_lower;
  uint32_t     mem_upper;
  uint32_t     boot_device;
  uint8_t*     cmdline;
  uint32_t     mods_count;
  mb_mod_t*    mods;
  union {
    mb_sym_out_t aout;
    mb_sym_elf_t elf;
  } syms;
  uint32_t     mmap_len;
  mb_mmap_t*   mmap;
  uint32_t     drives_len;
  mb_drives_t* drives;
  void*        config_table;
  uint8_t      boot_loader_name;
  mb_apm_t     apm_table;
} multiboot_t;
