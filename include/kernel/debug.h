/*
 * assert.h
 *
 *  Created on: 12.05.2014
 *      Author: fabian
 */

#ifndef ASSERT_H_
#define DEBUG_H_

#include "util.h"

#ifdef ASSERTIONS

/// panics when the condition is not true.
#define kassert(cond,msg) if(!(cond)) { kpanic(msg); }
/// panics when the condition is not true.
#define kassertf(cond,msg,...) if(!(cond)) { kpanicf(msg,__VA_ARGS__); }

#else

#define kassert(cond,msg)
#define kassertf(cond,msg,...)

#endif

#ifdef DEBUG

#define DEBUGF(...) kprintf( __VA_ARGS__);
#define MAGIC_BREAK asm volatile ("xchg %%bx, %%bx" : : : )
#define DEBUG_HALT { int foo = 1; while(foo); }

#else

#define DEBUGF(msg,...)
#define MAGIC_BREAK
#define DEBUG_HALT

#endif

#endif /* ASSERT_H_ */
