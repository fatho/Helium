/*
 * @file
 * @date May 15, 2014
 * @author fabian
 *
 * @brief This module handles the loading of the real kernel.
 */

#include "common/helium.h"
#include "loader/info.h"
#include "loader/kassert.h"
#include "loader/string.h"
#include "loader/util.h"

/// index of kernel in #info_modules
static size_t kernel_index = NO_MODULE;

/// barrier for AP entry
static kernel_ap_barrier = 1;

/**
 * @brief Searches for the kernel binary in the #info_modules table.
 *
 * When a kernel was found, it index is set to #kernel_index.
 */
void kernel_find() {
    kassert(kernel_index == NO_MODULE, "illegal call to kernel_find(): kernel already found");

    for (size_t i = 0; i < info_table.module_count; i++) {
        const char* module_cmdline = (const char*) (info_table.string_table_paddr
                + info_modules[i].cmdline);
        if (strncmp("kernel64", module_cmdline, 8) == 0) {
            // save kernel index
            info_table.kernel_module = i;
            kernel_index = i;
        }
    }
    if(kernel_index == NO_MODULE) {
        kpanic("found no module marked as kernel (cmdline must begin with kernel64)");
    }
}

/**
 * @brief Parses kernel header.
 */
void kernel_get_info() {

}


/**
 * @brief Maps the kernel to its desired virtual address.
 */
void kernel_map() {

}

/**
 * @brief Jumps to the BSP entry point of the kernel and releases the barrier
 * for application processors to jump.
 */
void kernel_bsp_jump() {
    kassert(kernel_ap_barrier, "kernel_bsp_jump called twice");

    kernel_ap_barrier = 0;
}

/**
 * @brief Waits until the #kernel_bsp_jump barrier is released, then jumps to
 * the AP entry point of the kernel. If the kernel has no AP entry point,
 * this function halts.
 */
void kernel_ap_jump() {
    while(kernel_ap_barrier);
}

/**
 * @brief Prepares the kernel.
 *
 * When this method completes successfully, the kernel's entry point can be called.
 */
void kernel_init() {
    kernel_find();
}
