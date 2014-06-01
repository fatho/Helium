/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains the C entry points for bootstrap and application processors.
 * This is the first time C code is executed while booting.
 */

#include "loader/acpi.h"
#include "loader/debug.h"
#include "loader/info.h"
#include "loader/paging.h"
#include "loader/pheap.h"
#include "loader/screen.h"
#include "loader/string.h"
#include "loader/util.h"

/**
 * @brief 64 bit C entry point for bootstrap processor.
 */
void main_bsp() {
    int success;

    info_init();
    info_debug_output();
    pheap_init(info_table.free_paddr);

    kprintf("TEST: %llx\n", paging_get_physical_addr(0x10000));
    MAGIC_BREAK;

    acpi_init();
    acpi_debug_output();

    extern uint8_t stack_top, stack_bottom;

    kprintf("stack: %llx - %llx\n", &stack_bottom, &stack_top);
    kprintf(".info: %llx - %llx\n", &info_start, &info_end);
}

/**
 * @brief 64 bit C entry point for application processors.
 */
void main_ap() {

}
