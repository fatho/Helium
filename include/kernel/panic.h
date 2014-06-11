/*
 * util.h
 *
 *  Created on: 08.05.2014
 *      Author: fabian
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define __halt__ { while(1) { asm volatile ("cli"); asm volatile ("hlt"); } }

extern uint64_t panic_gprs[16];
extern uint64_t panic_rip, panic_rflags;
extern char* panic_file;
extern int panic_line;

/**
 * @brief Emits code that saves all general purpose registers to #panic_gprs,
 * the instruction pointer to #panic_rip and the flags to #panic_rflags.
 */
#define PANIC_SAVE_STATE() { \
    asm volatile ("movq %%rax, %0" : "=r" (panic_gprs[0]) ); \
    asm volatile ("movq %%rbx, %0" : "=r" (panic_gprs[1]) ); \
    asm volatile ("movq %%rcx, %0" : "=r" (panic_gprs[2]) ); \
    asm volatile ("movq %%rdx, %0" : "=r" (panic_gprs[3]) ); \
    asm volatile ("movq %%rbp, %0" : "=r" (panic_gprs[4]) ); \
    asm volatile ("movq %%rsi, %0" : "=r" (panic_gprs[5]) ); \
    asm volatile ("movq %%rdi, %0" : "=r" (panic_gprs[6]) ); \
    asm volatile ("movq %%rsp, %0" : "=r" (panic_gprs[7]) ); \
    asm volatile ("movq %%r8, %0" : "=r" (panic_gprs[8]) ); \
    asm volatile ("movq %%r9, %0" : "=r" (panic_gprs[9]) ); \
    asm volatile ("movq %%r10, %0" : "=r" (panic_gprs[10]) ); \
    asm volatile ("movq %%r11, %0" : "=r" (panic_gprs[11]) ); \
    asm volatile ("movq %%r12, %0" : "=r" (panic_gprs[12]) ); \
    asm volatile ("movq %%r13, %0" : "=r" (panic_gprs[13]) ); \
    asm volatile ("movq %%r14, %0" : "=r" (panic_gprs[14]) ); \
    asm volatile ("movq %%r15, %0" : "=r" (panic_gprs[15]) ); \
    asm volatile ("leaq (%%rip), %%rax; movq %%rax, %0" : "=r" (panic_rip) ); \
    asm volatile ("pushfq; popq %%rax; movq %%rax, %0" : "=r" (panic_rflags)); \
    panic_file = __FILE__; \
    panic_line = __LINE__; }


/**
 * @brief Emits #PANIC_SAVE_STATE and a call to #_kpanic
 */
#define kpanic(msg) { PANIC_SAVE_STATE(); _kpanic(msg); }

/**
 * @brief Emits #PANIC_SAVE_STATE and a call to #_kpanicf
 */
#define kpanicf(...) { PANIC_SAVE_STATE(); _kpanicf(__VA_ARGS__); }


/**
 * @brief prints the \c message and the state previously saved with #PANIC_SAVE_STATE to the screen and halts.
 * @param message a message describing the kernel panic.
 * @remark This function does not return.
 */
void _kpanic(const char* message);

/**
 * @brief prints a formatted \c message and the state previously saved with #PANIC_SAVE_STATE to the screen and halts.
 * @param message a message describing the kernel panic.
 * @see kprintf(const char*, ...)
 * @remark This function does not return.
 */
void _kpanicf(const char* message, ...);

#endif /* UTIL_H_ */
