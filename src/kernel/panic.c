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
/// used by #PANIC_SAVE_STATE to save control registers
uint64_t panic_crs[5];
/// used by #PANIC_SAVE_STATE to save the current file name.
const char* panic_file;
/// used by #PANIC_SAVE_STATE to save the line in the current file.
int panic_line;
/// used by #PANIC_SAVE_STATE to save the current function name.
const char* panic_func;

#define PANIC_DUMP_STATE() { \
    kprintf("  in %s (%s:%d)\n", panic_func, panic_file, panic_line); \
    kputs("\n"); \
    \
    kprintf("        rax: %16llx r8 : %16llx r14: %16llx\n", panic_gprs[0], panic_gprs[8], panic_gprs[14]); \
    kprintf("        rbx: %16llx r9 : %16llx r15: %16llx\n", panic_gprs[1], panic_gprs[9], panic_gprs[15]); \
    kprintf("        rcx: %16llx r10: %16llx rbp: %16llx\n", panic_gprs[2], panic_gprs[10], panic_gprs[4]); \
    kprintf("        rdx: %16llx r11: %16llx rsp: %16llx\n", panic_gprs[3], panic_gprs[11], panic_gprs[7]); \
    kprintf("        rsi: %16llx r12: %16llx rip: %16llx\n", panic_gprs[5], panic_gprs[12], panic_rip); \
    kprintf("        rdi: %16llx r13: %16llx rfl: %16llx\n", panic_gprs[6], panic_gprs[13], panic_rflags); \
    kprintf("        cr0: %16llx cr1: %16llx cr2: %16llx\n", panic_crs[0], panic_crs[1], panic_crs[2]); \
    kprintf("        cr3: %16llx cr4: %16llx            \n", panic_crs[3], panic_crs[4]); \
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
