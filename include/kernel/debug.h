/*
 * assert.h
 *
 *  Created on: 12.05.2014
 *      Author: fabian
 */

#ifndef ASSERT_H_
#define DEBUG_H_

#include "kernel/panic.h"

#ifdef ASSERTIONS

/// panics when the condition is not true.
#define kassert(cond) if(!(cond)) { kpanic("Assertion Failed: " #cond); }
/// panics when the condition is not true.
#define kassertf(cond,...) if(!(cond)) { kpanicf(__VA_ARGS__); }

#else

#define kassert(cond)
#define kassertf(cond,...)

#endif

#ifdef DEBUG

#define DEBUGF(...) {kputs("\x1b[33m"); kprintf( __VA_ARGS__); kputs("\x1b[0m"); }
#define MAGIC_BREAK asm volatile ("xchg %%bx, %%bx" : : : )
#define DEBUG_HALT { int foo = 1; while(foo); }

#else

#define DEBUGF(msg,...)
#define MAGIC_BREAK
#define DEBUG_HALT

#endif

#define UNUSED(x) ((void)(x))

/**
 * @brief Writes the contents of the info structures to the screen.
 */
void debug_print_info();

#endif /* ASSERT_H_ */
