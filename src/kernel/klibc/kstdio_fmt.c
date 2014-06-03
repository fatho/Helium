/**
 * @file fmtstr.c
 * @date 02.06.2014
 * @author fabian
 *
 * @brief String formatting functions for kernel mode.
 */

#include "kernel/klibc/kstdio.h"
#include "kernel/klibc/string.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { DEFAULT, CHAR, SHORT, LONG, LONGLONG, INTMAX, SIZE, PTRDIFF, POINTER, LONGDOUBLE } fmtsize_t;

static const char digits_lower[] = "0123456789abcdef";
static const char digits_upper[] = "0123456789ABCDEF";

/**
 * @brief Converts an unsigned integer value to a decimal string and writes it to the buffer.
 */
static void uitoa(uint64_t value, char* buf, size_t bufsz, int radix, const char digits[]) {
    char tmpbuf[68];
    memset(tmpbuf, 0, sizeof(tmpbuf));
    int offset = sizeof(tmpbuf) - 1;
    do {
        offset -= 1;
        tmpbuf[offset] = digits[value % radix];
        value = value / radix;
    } while(value != 0 && offset > 0);
    size_t tmplen = sizeof(tmpbuf) - offset;
    if(tmplen > bufsz) {
        tmplen = bufsz;
    }
    memcpy(buf, tmpbuf + offset, tmplen);
}

/**
 * @brief Converts a signed integer value to a decimal string and writes it to the buffer.
 */
static void itoa(int64_t value, char* buf, size_t bufsz, int radix, const char digits[], int force_sign, char positive_sign) {
    int negative = value < 0;
    if(negative) {
        value = -value;
    }
    if(bufsz > 0 && (negative || force_sign)) {
        buf[0] = negative ? '-' : positive_sign;
        buf++;
        bufsz--;
    }
    uitoa(value, buf, bufsz, radix, digits);
}

int isdigit(int ch) {
    return (ch >= '0') && (ch <= '9');
}

int snprintf(char* str, size_t strn, const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
    int result = vsnprintf(str, strn, format, arglist);
    va_end(arglist);
    return result;
}

#define SPUTC(ch) { if(outremaining--) { str[outidx++] = ch; } written++; }
#define SPUTS(s) { const char* tmp_put = s; while(*tmp_put) { SPUTC(*tmp_put); tmp_put++; } }
#define SPAD(cnt,ch) { for(int i = 0; i < cnt; i++) { SPUTC(ch); } }
#define SPUTSPAD(s,cnt,ch) { \
    const char* tmp_pad = s; \
    int arglen = strlen(tmp_pad); \
    if(arglen >= width) { \
        SPUTS(tmp_pad); \
    } else { \
        int diff = cnt - arglen; \
        if(left_justify) { \
            SPUTS(tmp_pad); \
            SPAD(diff, ' '); \
        } else { \
            SPAD(diff, ch); \
            SPUTS(tmp_pad); \
        } \
    } \
    }

int vsnprintf(char* str, size_t strn, const char* format, va_list arglist) {
    int written = 0;
    // write index in str
    size_t outidx = 0;
    // remaining bytes in str
    size_t outremaining = strn - 1;
    if (!format) {
        format = "";
    }

    for(const char* fmt = format; *fmt; fmt++) {
        if(*fmt != '%') {
            SPUTC(*fmt);
            continue;
        }

        int left_justify = 0;
        int force_sign = 0;
        int pad_char = ' ';
        int positive_sign = '+';
        int width = 0;
        int precision = 6;
        fmtsize_t size = DEFAULT;

        // parse flags
        parse_flag:
        fmt++;
        switch(*fmt) {
        case '-':
            left_justify = 1; goto parse_flag;
        case '+':
            force_sign = 1; positive_sign = '+'; goto parse_flag;
        case ' ':
            force_sign = 1; positive_sign = ' '; goto parse_flag;
        //case '#':
        case '0':
            pad_char = '0'; goto parse_flag;
        default:
            break;
        }

        // parse width
        if(*fmt == '*') {
            width = va_arg(arglist, int);
            if(width < 0) {
                left_justify = 1;
                width = -width;
            }
            fmt++;
        } else if(isdigit(*fmt)) {
            do {
                int val = *fmt - '0';
                width *= 10;
                width += val;
                fmt++;
            } while(isdigit(*fmt));
        }

        // parse precision
        if(*fmt == '.') {
            fmt++;
            if(*fmt == '*') {
                precision = va_arg(arglist, int);
                if(precision < 0) {
                    precision = -precision;
                }
                fmt++;
            } else if(isdigit(*fmt)) {
                precision = 0;
                do {
                    int val = *fmt - '0';
                    precision *= 10;
                    precision += val;
                    fmt++;
                } while(isdigit(*fmt));
            }
        }

        // parse length
        switch(*fmt) {
        case 'j':
            size = INTMAX; fmt++; break;
        case 'z':
            size = SIZE; fmt++; break;
        case 't':
            size = PTRDIFF; fmt++; break;
        case 'L':
            size = LONGDOUBLE; fmt++; break;
        case 'h':
            fmt++;
            if(*fmt == 'h') {
                size = CHAR;
                fmt++;
            } else {
                size = SHORT;
            }
            break;
        case 'l':
            fmt++;
            if(*fmt == 'l') {
                size = LONGLONG;
                fmt++;
            } else {
                size = LONG;
            }
            break;
        default:
            break;
        }

        // parse fmt
        switch(*fmt) {
        case 'd':
        case 'i': { // signed decimal integer
            int64_t arg;
            switch(size) {
            case CHAR:
                arg = va_arg(arglist, int); break;
            case SHORT:
                arg = va_arg(arglist, int); break;
            case LONG:
                arg = va_arg(arglist, long int); break;
            case LONGLONG:
                arg = va_arg(arglist, long long int); break;
            case INTMAX:
                arg = va_arg(arglist, intmax_t); break;
            case SIZE:
                arg = va_arg(arglist, size_t); break;
            case PTRDIFF:
                arg = va_arg(arglist, ptrdiff_t); break;
            default:
                arg = va_arg(arglist, int); break;
            }
            char tmpbuf[24];
            memset(tmpbuf, 0, sizeof(tmpbuf));
            itoa(arg, tmpbuf, sizeof(tmpbuf), 10, digits_lower, force_sign, positive_sign);
            if(isdigit(tmpbuf[0]) || pad_char == ' ') {
                SPUTSPAD(tmpbuf, width, pad_char);
            } else {
                SPUTC(tmpbuf[0]);
                SPUTSPAD(tmpbuf+1, width-1, pad_char);
            }
            break;
        }
        case 'x':
        case 'X':
        case 'o':
        case 'p':
        case 'u': { // unsigned decimal integer
            int radix;
            switch(*fmt) {
            case 'x':
            case 'X':
                radix = 16; break;
            case 'p':
                radix = 16; size = POINTER; break;
            case 'o':
                radix = 8; break;
            default:
                radix = 10; break;
            }

            uint64_t arg;
            switch(size) {
            case CHAR:
                arg = va_arg(arglist, unsigned int); break;
            case SHORT:
                arg = va_arg(arglist, unsigned int); break;
            case LONG:
                arg = va_arg(arglist, unsigned long int); break;
            case LONGLONG:
                arg = va_arg(arglist, unsigned long long int); break;
            case INTMAX:
                arg = va_arg(arglist, uintmax_t); break;
            case SIZE:
                arg = va_arg(arglist, size_t); break;
            case PTRDIFF:
                arg = va_arg(arglist, ptrdiff_t); break;
            case POINTER:
                arg = va_arg(arglist, uintptr_t); break;
            default:
                arg = va_arg(arglist, unsigned int); break;
            }
            char tmpbuf[24];
            memset(tmpbuf, 0, sizeof(tmpbuf));
            uitoa(arg, tmpbuf, sizeof(tmpbuf), radix, *fmt == 'X' ? digits_upper : digits_lower);
            SPUTSPAD(tmpbuf, width, pad_char);
            break;
        }
        case 'c': { // character (passed as int)
            int arg = va_arg(arglist, int);
            char argtmp[2] = { arg, 0 };
            SPUTSPAD(argtmp, width, ' ');
            break;
        }
        case 's': { // string
            const char* arg = va_arg(arglist, const char*);
            SPUTSPAD(arg, width, ' ');
            break;
        }
        case 'n': { // write number of characters written
            int* arg = va_arg(arglist, int*);
            *arg = written;
            break;
        }
        case '%': {
            SPUTC('%');
            break;
        }
        /// IGNORE FLOATING POINT FOR NOW
        case 'f':
        case 'F':
            // TODO: decimal floating point
            break;
        case 'e':
        case 'E':
            // TODO: decimal floating point in scientific notation
            break;
        case 'g':
        case 'G':
            // TODO: decimal floating point in shortest representation (%e or %f)
            break;
        case 'a':
        case 'A':
            // TODO: hexadecimal floating point
            break;
        }

    }
    if(outremaining) {
        SPUTC('\0');
    } else {
        str[strn-1] = '\0';
    }
    return written;
}
