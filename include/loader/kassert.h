/*
 * assert.h
 *
 *  Created on: 12.05.2014
 *      Author: fabian
 */

#ifndef ASSERT_H_
#define ASSERT_H_

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

#endif /* ASSERT_H_ */
