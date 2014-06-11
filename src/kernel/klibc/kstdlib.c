/**
 * @file kstdlib.c
 * @date 11.06.2014
 * @author fabian
 *
 * @brief
 */

#include "kernel/klibc/kstdlib.h"

#include <limits.h>

#define max(a,b) ((a)>(b) ? (a) : (b))
#define min(a,b) ((a)<(b) ? (a) : (b))

/**
 * @brief Converts string to double
 *
 * @remark Who needs floating point numbers in kernel mode anyway?
 * @todo Implement #strtod
 */
double strtod (const char* str, char** endptr) {
    ((void)str);
    ((void)endptr);
    return 0;
}

/**
 * @brief Converts string to long integer.
 */
long int strtol (const char* str, char** endptr, int base) {
    int neg = 0;
    // validate base parameter
    if(base > 36 || (base != 0 && base < 2)) {
        return 0;
    }
    // skip leading whitespaces
    while(*str == ' ' || (*str >= 0x09 && *str <= 0xD)) str++;
    // parse sign
    switch(*str) {
    case '-':
        neg = 1;
        /* no break */
    case '+':
        str++;
        break;
    }
    // parse prefix
    if(*str == '0') {
        // leading zeros are always safe to ignore
        str++;
        //
        if((*str == 'x' || *str == 'X') && (base == 0 || base == 16)) {
            base = 16;
            str++;
        } else if (base == 0) {
            base = 8;
        }
    } else if(base == 0) {
        base = 10;
    }

    unsigned long acc = 0;
    unsigned long overflow_limit = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    int overflow_digit = (int)(overflow_limit % base);
    overflow_limit /= base;

    int overflow = 0;

    // parse number
    while(*str) {
        int digit;
        if(*str >= '0' && *str <= '9') {
            digit = *str - '0';
        } else if(*str >= 'A' && *str <= 'Z') {
            digit = 10 + *str - 'A';
        } else if(*str >= 'a' && *str <= 'z') {
            digit = 10 + *str - 'a';
        } else {
            break;
        }
        if(digit >= base)
            break;
        if(overflow || acc > overflow_limit || (acc == overflow_limit && digit > overflow_digit)) {
            overflow = 1;
        } else {
            acc = acc * 10 + digit;
        }
        str++;
    }

    long result;

    if(overflow) {
        result = neg ? LONG_MIN : LONG_MAX;
    } else {
        result = neg ? -acc : acc;
    }

    // set end marker
    if(endptr) {
        *endptr = (char*)str; // (cast away const)
    }

    return result;
}
