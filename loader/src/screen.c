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
    volatile uint16_t* dest = VGAPTR(curcol, curline);
    // output until '\0' terminator or length
    while (*message && length--) {
        if (*message == '\n') {
            curcol = COLS;
        } else {
            *dest = *message | (screen_color << 8);
            dest++;
            curcol++;
        }
        if (curcol == COLS) {
            curcol = 0;
            curline += 1;
            if (curline == ROWS) {
                scroll_up();
                //curline = 0;
            }
            dest = VGAPTR(curcol, curline);
        }
        message++;
    }
}

/**
 * @brief writes a number to the VGA memory in 64 bit hexadecimal format.
 */
void kputi(uint64_t number) {
    char string[17];
    for (int i = 0; i < 16; i++) {
        uint8_t block = (number >> (4 * (15 - i))) & 0xF;
        if (block < 10) {
            string[i] = '0' + block;
        } else {
            string[i] = 'A' + (block - 10);
        }
    }
    string[16] = 0;
    kputs(string);
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
    const char* src = format;
    const char* src2 = format;

    // until 0 terminator
    while (*src) {
        // find percent sign
        while (*src2 && *src2 != '%') {
            src2++;
        }
        if (*src2) {
            // if *src2 is not 0, is must be the percent-sign
            src2++;
            if (*src2 == '%') {
                // double-percent, output one
                kputsn(src, src2 - src);
                src2++;
            } else if (strncmp(src2, "s", 1) == 0) {
                // double-percent, output one
                kputsn(src, src2 - src - 1);
                src2++;
                const char* arg = va_arg(vl, const char*);
                kputs(arg);
            } else if (strncmp(src2, "x", 1) == 0) {
                // double-percent, output one
                kputsn(src, src2 - src - 1);
                src2++;
                uint32_t arg = va_arg(vl, uint32_t);
                kputi(arg);
            } else if (strncmp(src2, "llx", 3) == 0) {
                // double-percent, output one
                kputsn(src, src2 - src - 1);
                src2 += 3;
                uint64_t arg = va_arg(vl, uint64_t);
                kputi(arg);
            } else {
                // undefined sequence, output all
                kputsn(src, src2 - src + 1);
                src2++;
            }
        } else {
            // *src2 is 0, print rest of string
            kputsn(src, src2 - src);
        }

        src = src2;
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
    for(int i = COLS * (ROWS - 1); i < ROWS*COLS; i++) {
        ((uint16_t*) VGAMEM)[i] = (screen_color << 8) | ' ';
    }
    // adjust cursor position
    curline -= 1;
}
