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
#include "kernel/cpu.h"
#include "kernel/debug.h"
#include "kernel/info.h"

#include "kernel/interrupts/idt.h"

#include "kernel/klibc/string.h"
#include "kernel/klibc/kstdio.h"

#include "kernel/mem/pfa.h"

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

    char cpuvendor[13];
    cpu_get_vendor(cpuvendor);
    cpuvendor[12] = 0;
    kprintf("Detected CPU: %s\n", cpuvendor);

    kprintf(" * setting up IDT\n");
    idt_init();

    kprintf(" * parsing system information\n");
    info_init();

    kprintf(" * initializing page frame allocator\n");
    pfa_init();

    debug_print_info();

}

/**
 * @brief 64 bit C entry point for application processors.
 */
void main_ap() {

}
