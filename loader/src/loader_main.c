#include <stdint.h>
#include <stddef.h>
#include "screen.h"
#include "string.h"
#include "multiboot.h"

multiboot_info_t* multiboot_info;

void parse_modules(multiboot_mod_t* modules, uint32_t modcount) {
    for(uint32_t i = 0; i < modcount; i++) {
        if(strncmp((char*)modules[i].name, "kernel64", 8) == 0) {
            kputs("=== KERNEL FOUND ===\n");
        } else {
            kputs("=== MODULE FOUND ===\n");
        }
        kputs("cmdline   : "); kputs((char*)modules[i].name); kputs("\n");
        kputs("addr_start: "); kputi(modules[i].addr_start); kputs("\n");
        kputs("addr_end  : "); kputi(modules[i].addr_end); kputs("\n");
    }
}

void multiboot_init() {
    kputs("Parsing multiboot info...\n");
    parse_modules((multiboot_mod_t*)multiboot_info->mods, multiboot_info->mods_count);
    return;
}
