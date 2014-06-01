/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains some useful helper functions.
 */

#include "kernel/util.h"
#include "kernel/screen.h"

#include <stdarg.h>

/**
 * @brief prints \c message to the screen and halts.
 * @param message a message describing the kernel panic.
 * @remark This function does not return.
 */
void kpanic(const char* message) {
    clear_screen(SCREENCOLOR(BLUE, WHITE));
    kputs(message);
    __halt__;
}

/**
 * @brief prints a formatted \c message to the screen and halts.
 * @param message a message describing the kernel panic.
 * @see kprintf(const char*, ...)
 * @remark This function does not return.
 */
void kpanicf(const char* message, ...) {
    clear_screen(SCREENCOLOR(BLUE, WHITE));
    va_list vl;
    va_start(vl, message);
    kvprintf(message,vl);
    va_end(vl);
    __halt__;
}
