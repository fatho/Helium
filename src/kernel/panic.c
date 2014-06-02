/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains some useful helper functions.
 */

#include "kernel/panic.h"
#include "kernel/klibc/kstdio.h"

#include <stdint.h>
#include <stdarg.h>

/// used by #PANIC_SAVE_STATE to save important register values before they are destroyed by the function call to #_kpanic
uint64_t panic_gprs[16];
/// used by #PANIC_SAVE_STATE to save the instruction pointer
uint64_t panic_rip;
/// used by #PANIC_SAVE_STATE to save the CPU flags
uint64_t panic_rflags;

#define PANIC_DUMP_STATE() { \
    kputs("\n"); \
    kprintf("        rax: %llx rbx: %llx rcx: %llx\n", panic_gprs[0], panic_gprs[1], panic_gprs[2]); \
    kprintf("        rdx: %llx rsi: %llx rdi: %llx\n", panic_gprs[3], panic_gprs[5], panic_gprs[6]); \
    kprintf("        rsp: %llx rbp: %llx r8 : %llx\n", panic_gprs[7], panic_gprs[4], panic_gprs[8]); \
    kprintf("        r9 : %llx r10: %llx r11: %llx\n", panic_gprs[9], panic_gprs[10], panic_gprs[11]); \
    kprintf("        r12: %llx r13: %llx r14: %llx\n", panic_gprs[12], panic_gprs[13], panic_gprs[14]); \
    kprintf("        r15: %llx rip: %llx rfl: %llx\n", panic_gprs[15], panic_rip, panic_rflags); \
    kputs("\n"); }

/**
 * @brief prints the \c message and the state previously saved with #PANIC_SAVE_STATE to the screen and halts.
 * @param message a message describing the kernel panic.
 * @remark This function does not return.
 */
void _kpanic(const char* message) {
    clear_screen(VGACOLOR(BLACK, RED));
    kputs(message);
    PANIC_DUMP_STATE();
    __halt__;
}

/**
 * @brief prints a formatted \c message and the state previously saved with #PANIC_SAVE_STATE to the screen and halts.
 * @param message a message describing the kernel panic.
 * @see kprintf(const char*, ...)
 * @remark This function does not return.
 */
void _kpanicf(const char* message, ...) {
    clear_screen(VGACOLOR(BLACK, RED));
    va_list vl;
    va_start(vl, message);
    kvprintf(message,vl);
    va_end(vl);
    PANIC_DUMP_STATE();
    __halt__;
}
