/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains the C entry points for bootstrap and application processors.
 * This is the first time C code is executed while booting.
 */

#include "loader/info.h"
#include "loader/page.h"
#include "loader/pheap.h"
#include "loader/screen.h"
#include "loader/string.h"
#include "loader/util.h"

/**
 * @brief 64 bit C entry point for bootstrap processor.
 */
void main_bsp() {
    info_init();
    pheap_init(info_table.free_paddr);
    info_debug_output();
}

/**
 * @brief 64 bit C entry point for application processors.
 */
void main_ap() {

}
