/*
 * util.h
 *
 *  Created on: 08.05.2014
 *      Author: fabian
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MAGIC_BREAK asm volatile ("xchg %%bx, %%bx" : : : )
#define HALT { while(1) { asm volatile ("cli"); asm volatile ("hlt"); } }

/**
 * @brief prints \c message to the screen and halts.
 * @param message a message describing the kernel panic.
 */
void kpanic(const char* message);

/**
 * @brief prints a formatted \c message to the screen and halts.
 * @param message a message describing the kernel panic.
 * @see kprintf(const char*, ...)
 */
void kpanicf(const char* message, ...);

#endif /* UTIL_H_ */
