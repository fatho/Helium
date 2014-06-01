/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains a basic screen output system.
 */

#include "loader/screen.h"
#include "loader/string.h"
#include "loader/util.h"
#include <stdarg.h>
#include <stdint.h>

/// current output cursor line
uint32_t curline = 0;
/// current output cursor column
uint32_t curcol = 0;

/// current text color
uint8_t screen_color = SCREENCOLOR(BLACK, WHITE);

/**
 * @brief writes a null terminated string to the VGA buffer.
 */
void kputs(const char* message) {
    kputsn(message, strlen(message));
}

/**
 * @brief writes a null terminated string to the VGA buffer, but at most \c size characters.
 */
void kputsn(const char* message, size_t length) {
    // output until '\0' terminator or length
    while (*message && length--) {
        kputchar(*message);
        message += 1;
    }
}

/**
 * @brief Writes a character to the screen.
 *
 * @param chr the character
 */
void kputchar(int chr) {
    volatile uint16_t* dest = VGAPTR(curcol, curline);

    if (chr == '\n') {
        curcol = COLS;
    } else {
        *dest = (uint8_t) chr | (screen_color << 8);
        curcol++;
    }
    if (curcol == COLS) {
        curcol = 0;
        curline += 1;
        if (curline == ROWS) {
            scroll_up();
        }
    }
}

/**
 * @brief writes a number to the VGA memory in 64 bit hexadecimal format.
 */
void kputhex(uint64_t number) {
    for (int i = 0; i <= 15; i++) {
        uint8_t block = (number >> (4 * (15 - i))) & 0xF;
        char chr = (block < 10) ? '0' + block : 'A' + (block - 10);
        kputchar(chr);
    }
}

/**
 * @brief Converts a signed integer value to a decimal string and writes it to the buffer.
 */
void itoa(int64_t value, char* buf, size_t bufsz) {
    int negative = value < 0;
    if(negative) {
        value = -value;
        if(bufsz > 0) {
            buf[0] = '-';
            buf++;
            bufsz--;
        }
    }
    uitoa(value, buf, bufsz);
}

/**
 * @brief Converts an unsigned integer value to a decimal string and writes it to the buffer.
 */
void uitoa(uint64_t value, char* buf, size_t bufsz) {
    char tmpbuf[24];
    memset(tmpbuf, 0, sizeof(tmpbuf));
    int offset = sizeof(tmpbuf) - 1;
    do {
        offset -= 1;
        tmpbuf[offset] = '0' + (value % 10);
        value = value / 10;
    } while(value != 0 && offset > 0);
    int tmplen = sizeof(tmpbuf) - offset;
    if(tmplen > bufsz) {
        tmplen = bufsz;
    }
    memcpy(buf, tmpbuf + offset, tmplen);
}

/**
 * @brief A really simple implementation of printf.
 *
 * The following format specifiers are supported:
 *   - "%%"   print single percent sign
 *   - "%s"   print string
 *   - "%x"   print 32 bit number as 64 bit hex string
 *   - "%llx" print 64 bit number as 64 bit hex string
 */
void kprintf(const char* format, ...) {
    va_list vl;
    va_start(vl, format);

    kvprintf(format, vl);

    va_end(vl);
}

/**
 * @brief same as \c kvprintf(const char*, ...), but with explicit variable argument list.
 */
void kvprintf(const char* format, va_list vl) {
    for (const char* str = format; *str; str++) {
        if (*str != '%') {
            kputchar(*str);
            continue;
        }
        int longcnt = 0;

        // parse format flags
        parse_flag:
        str++;
        switch(*str) {
        case 'l': longcnt += 1; goto parse_flag;
        }

        // parse data type
        switch(*str) {
        case 's': {
            const char* arg = va_arg(vl, const char*);
            kputs(arg);
            break;
        }
        case 'c': {
            char arg = va_arg(vl, int);
            kputchar(arg);
            break;
        }
        case 'd': {
            char buf[24];
            memset(buf, 0, sizeof(buf));
            int64_t arg;
            if(longcnt >= 2) {
                arg = va_arg(vl, int64_t);
            } else {
                arg = va_arg(vl, int64_t);
            }
            itoa(arg, buf, sizeof(buf));
            kputs(buf);
            break;
        }
        case 'x':
        case 'X': {
            char basechr = (*str == 'x') ? 'a' : 'A';
            if(longcnt >= 2) {
                uint64_t arg = va_arg(vl, uint64_t);
                for (int i = 0; i < 16; i++) {
                    uint8_t block = (arg >> (4 * (15 - i))) & 0xF;
                    char chr = (block < 10) ? '0' + block : basechr + (block - 10);
                    kputchar(chr);
                }
            } else {
                uint32_t arg = va_arg(vl, uint32_t);
                for (int i = 0; i < 8; i++) {
                    uint8_t block = (arg >> (4 * (7 - i))) & 0xF;
                    char chr = (block < 10) ? '0' + block : basechr + (block - 10);
                    kputchar(chr);
                }
            }
            break;
        }
        }
    }
}

/**
 * Clears the screen with the specified foreground and background color.
 * Sets \c color as the new default \c screen_color
 *
 * @param color a VGA color code where the higher four bits specify the background
 * and the lower four bits specify foreground color.
 */
void clear_screen(uint8_t color) {
    volatile uint16_t* dest = VGAPTR(0, 0);
    screen_color = color;
    curcol = 0;
    curline = 0;
    for (int i = 0; i < COLS * ROWS; i++) {
        dest[i] = (color << 8) | ' ';
    }
}

/**
 * Scrolls the VGA buffer up by one line and clears the last line with the current \c screen_color.
 * It also decreases the current cursor row by one.
 */
void scroll_up() {
    for (int i = COLS; i < COLS * ROWS; i++) {
        ((uint16_t*) VGAMEM)[i - COLS] = ((uint16_t*) VGAMEM)[i];
    }
    for (int i = COLS * (ROWS - 1); i < ROWS * COLS; i++) {
        ((uint16_t*) VGAMEM)[i] = (screen_color << 8) | ' ';
    }
    // adjust cursor position
    curline -= 1;
}
