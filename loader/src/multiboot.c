/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains functions for parsing the multiboot info structure received from the
 * multiboot compliant bootloader.
 */

#include "loader/multiboot.h"

/**
 * @brief pointer to multiboot info structure. set by #boot32_bsp in boot32.s
 */
multiboot_info_t* multiboot_info;
