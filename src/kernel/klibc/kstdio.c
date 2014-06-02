/**
 * @file
 *
 * @author Fabian Thorand
 * @date   May 13, 2014
 *
 * @brief This module contains a basic screen output system.
 */

#include "kernel/klibc/kstdio.h"
#include "kernel/klibc/fmtstr.h"
#include "kernel/klibc/string.h"
#include <stdarg.h>
#include <stdint.h>

/// current output cursor line
uint32_t curline = 0;
/// current output cursor column
uint32_t curcol = 0;

/// current text color
uint8_t screen_color = VGACOLOR(BLACK, WHITE);

#define COLOR_STACK_SIZE 16
static uint8_t color_stack[COLOR_STACK_SIZE];
static int color_stack_index = COLOR_STACK_SIZE;

/**
 * @brief Saves the current screen color on a stack and sets the given colors.
 * @return remaining space on the stack or -1 if the stack is already full.
 */
int screen_push_color(uint8_t background, uint8_t foreground) {
    if(color_stack_index > 0) {
        color_stack_index -= 1;
        color_stack[color_stack_index] = screen_color;
        screen_color = VGACOLOR(background, foreground);
        return color_stack_index;
    } else {
        return -1;
    }
}

/**
 * @brief Restores the screen color from the stack.
 * @return The remaining space on the stack or -1 if the stack is already empty.
 */
int screen_pop_color() {
    if(color_stack_index < COLOR_STACK_SIZE) {
        screen_color = color_stack[color_stack_index];
        color_stack_index += 1;
        return color_stack_index;
    } else {
        return -1;
    }
}

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
    char linebuf[256];
    vsnprintf(linebuf, sizeof(linebuf), format, vl);
    kputs(linebuf);
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
