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
// use direct screen backend
#include "kernel/klibc/kstdio_screen.h"

#include <stdint.h>
#include <stdarg.h>

/// used by #PANIC_SAVE_STATE to save important register values before they are destroyed by the function call to #_kpanic
uint64_t panic_gprs[16];
/// used by #PANIC_SAVE_STATE to save the instruction pointer
uint64_t panic_rip;
/// used by #PANIC_SAVE_STATE to save the CPU flags
uint64_t panic_rflags;
/// used by #PANIC_SAVE_STATE to save the current file name.
char* panic_file;
/// used by #PANIC_SAVE_STATE to save the line in the current file.
int panic_line;

#define PANIC_DUMP_STATE() { \
    kprintf("  >>> in %s:%d\n", panic_file, panic_line); \
    kputs("\n"); \
    \
    kprintf("        rax: %16llx r8 : %16llx r14: %16llx\n", panic_gprs[0], panic_gprs[7], panic_gprs[2]); \
    kprintf("        rbx: %16llx r9 : %16llx r15: %16llx\n", panic_gprs[1], panic_gprs[5], panic_gprs[6]); \
    kprintf("        rcx: %16llx r10: %16llx rsp: %16llx\n", panic_gprs[2], panic_gprs[4], panic_gprs[8]); \
    kprintf("        rdx: %16llx r11: %16llx rbp: %16llx\n", panic_gprs[3], panic_gprs[10], panic_gprs[11]); \
    kprintf("        rsi: %16llx r12: %16llx rip: %16llx\n", panic_gprs[5], panic_gprs[13], panic_gprs[14]); \
    kprintf("        rdi: %16llx r13: %16llx rfl: %16llx\n", panic_gprs[6], panic_rip, panic_rflags); \
    \
    kprintf("        rax: %16llx rbx: %16llx rcx: %16llx\n", panic_gprs[0], panic_gprs[1], panic_gprs[2]); \
    kprintf("        rdx: %16llx rsi: %16llx rdi: %16llx\n", panic_gprs[3], panic_gprs[5], panic_gprs[6]); \
    kprintf("        rsp: %16llx rbp: %16llx r8 : %16llx\n", panic_gprs[7], panic_gprs[4], panic_gprs[8]); \
    kprintf("        r9 : %16llx r10: %16llx r11: %16llx\n", panic_gprs[9], panic_gprs[10], panic_gprs[11]); \
    kprintf("        r12: %16llx r13: %16llx r14: %16llx\n", panic_gprs[12], panic_gprs[13], panic_gprs[14]); \
    kprintf("        r15: %16llx rip: %16llx rfl: %16llx\n", panic_gprs[15], panic_rip, panic_rflags); \
    kputs("\n"); }

/**
 * @brief prints the \c message and the state previously saved with #PANIC_SAVE_STATE to the screen and halts.
 * @param message a message describing the kernel panic.
 * @remark This function does not return.
 */
void _kpanic(const char* message) {
    // in case of panic, switch back to direct screen backend
    kstdio_set_backend(&kstdio_screen_direct);
    screen_clear(VGACOLOR(BLACK, RED));
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
    // in case of panic, switch back to direct screen backend
    kstdio_set_backend(&kstdio_screen_direct);
    screen_clear(VGACOLOR(BLACK, RED));
    va_list vl;
    va_start(vl, message);
    kvprintf(message,vl);
    va_end(vl);
    PANIC_DUMP_STATE();
    __halt__;
}
