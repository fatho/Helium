/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains the C entry points for bootstrap and application processors.
 * This is the first time C code is executed while booting.
 */

#include "kernel/panic.h"
#include "kernel/debug.h"
#include "kernel/info.h"

#include "kernel/interrupts/idt.h"

#include "kernel/klibc/string.h"
#include "kernel/klibc/kstdio.h"

void print_welcome() {
    kputs("\x1b[33m");
    kprintf("                      +-----------------------------------+\n");
    kprintf("                      |        Helium Kernel v0.01        |\n");
    kprintf("                      | Copyright 2014 (c) Fabian Thorand |\n");
    kprintf("                      +-----------------------------------+\n");
    kprintf("\n\n");
    kputs("\x1b[0m");
}

/**
 * @brief 64 bit C entry point for bootstrap processor.
 */
void main_bsp() {
    print_welcome();

    kprintf(" * setting up IDT\n");
    // setup IDT
    idt_init();

    kprintf(" * parsing system information\n");
    // parse system information
    info_init();

    debug_print_info();

}

/**
 * @brief 64 bit C entry point for application processors.
 */
void main_ap() {

}
