/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains the C entry points for bootstrap and application processors.
 * This is the first time C code is executed while booting.
 */

#include "kernel/acpi.h"
#include "kernel/debug.h"
#include "kernel/info.h"
#include "kernel/page.h"
#include "kernel/pheap.h"
#include "kernel/screen.h"
#include "kernel/string.h"
#include "kernel/util.h"

/**
 * @brief 64 bit C entry point for bootstrap processor.
 */
void main_bsp() {
    info_init();
    info_debug_output();
    pheap_init(info_table.free_paddr);

    kprintf("TEST: %llx\n", paging_get_physical_addr(0x10000));
    MAGIC_BREAK;

    acpi_init();
    acpi_debug_output();
}

/**
 * @brief 64 bit C entry point for application processors.
 */
void main_ap() {

}
