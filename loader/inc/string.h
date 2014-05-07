/*
 * string.h
 *
 *  Created on: 07.05.2014
 *      Author: fabian
 */

#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>


void* memset(void* ptr, int value, size_t num);

int strcmp(const char* str1, const char* str2);
int strncmp(const char* s1, const char* s2, size_t n);

#endif /* STRING_H_ */
