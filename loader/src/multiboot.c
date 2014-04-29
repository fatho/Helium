#include <text.h>
#include <multiboot.h>

multiboot_t* multiboot_info;

void multiboot_init() {
    kernel_msg("Parsing multiboot info...\n");
    kernel_msg(multiboot_info->cmdline);
    kernel_msg("\n");
    return;
}
