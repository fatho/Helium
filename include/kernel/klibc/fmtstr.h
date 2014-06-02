/**
 * @file fmtstr.h
 * @date 02.06.2014
 * @author fabian
 *
 * @brief
 */

#ifndef FMTSTR_H_
#define FMTSTR_H_


#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

int vsnprintf(char* str, size_t strn, const char* format, va_list arglist);

int snprintf(char* str, size_t strn, const char* format, ...);


#endif /* FMTSTR_H_ */
