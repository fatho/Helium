/*
 * util.h
 *
 *  Created on: 08.05.2014
 *      Author: fabian
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MAGIC_BREAK asm volatile ("xchg %%bx, %%bx" : : : )

void kpanic(char* message);

#endif /* UTIL_H_ */
