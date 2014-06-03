/**
 * @file kstdio_screen.c
 *
 * @author Fabian Thorand
 * @date   03.06.2014
 *
 * @brief Backend for kstdio.h that writes directly to VGA memory.
 */

#include "kernel/klibc/kstdio_screen.h"
#include "kernel/klibc/kstdio.h"

/// current output cursor line
uint32_t curline = 0;
/// current output cursor column
uint32_t curcol = 0;

/// current text color
uint8_t screen_color = VGACOLOR(BLACK, WHITE);

#define COLOR_STACK_SIZE 16
static uint8_t color_stack[COLOR_STACK_SIZE];
static int color_stack_index = COLOR_STACK_SIZE;

kstdio_backend_t kstdio_screen_direct = {screen_kputchar, NULL, NULL};

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
 * @brief Writes a character to the screen.
 *
 * @param chr the character
 */
void screen_kputchar(int chr) {
    volatile uint16_t* dest = VGAPTR(curcol, curline);
    static int in_escape = 0;
    static int format_code = 0;

    if(chr == '\x1b') {
        in_escape = 1;
        format_code = 0;
    }
    switch(in_escape) {
    case 1:
        if(chr == '[') {
            in_escape = 2;
        }
        break;
    case 2:
        if(chr == 'm') {
            // escape sequence finished
            in_escape = 0;
            switch(format_code) {
            case 0:
                screen_color = VGACOLOR(BLACK, WHITE); break;
            case 30: // set foreground color to black
                screen_color = VGASETFG(screen_color, BLACK); break;
            case 31: // set foreground color to red
                screen_color = VGASETFG(screen_color, RED); break;
            case 32: // set foreground color to green
                screen_color = VGASETFG(screen_color, GREEN); break;
            case 33: // set foreground color to yellow
                screen_color = VGASETFG(screen_color, YELLOW); break;
            case 34: // set foreground color to blue
                screen_color = VGASETFG(screen_color, BLUE); break;
            case 35: // set foreground color to magenta (purple)
                screen_color = VGASETFG(screen_color, MAGENTA); break;
            case 36: // set foreground color to cyan
                screen_color = VGASETFG(screen_color, CYAN); break;
            case 37: // set foreground color to white
            case 39: // set foreground color to default (white)
                screen_color = VGASETFG(screen_color, WHITE); break;
            case 40: // set foreground color to black
                screen_color = VGASETBG(screen_color, BLACK); break;
            case 41: // set foreground color to red
                screen_color = VGASETBG(screen_color, RED); break;
            case 42: // set foreground color to green
                screen_color = VGASETBG(screen_color, GREEN); break;
            case 43: // set foreground color to yellow
                screen_color = VGASETBG(screen_color, YELLOW); break;
            case 44: // set foreground color to blue
                screen_color = VGASETBG(screen_color, BLUE); break;
            case 45: // set foreground color to magenta (purple)
                screen_color = VGASETBG(screen_color, MAGENTA); break;
            case 46: // set foreground color to cyan
                screen_color = VGASETBG(screen_color, CYAN); break;
            case 47: // set foreground color to white
            case 49: // set foreground color to default (white)
                screen_color = VGASETBG(screen_color, WHITE); break;
            }
            return;
        } else if(chr >= '0' && chr <= '9') {
            format_code *= 10;
            format_code += chr - '0';
        } else {
            in_escape = 0;
        }
    }

    if(!in_escape) {
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
                screen_scroll();
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
void screen_clear(uint8_t color) {
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
void screen_scroll() {
    for (int i = COLS; i < COLS * ROWS; i++) {
        ((uint16_t*) VGAMEM)[i - COLS] = ((uint16_t*) VGAMEM)[i];
    }
    for (int i = COLS * (ROWS - 1); i < ROWS * COLS; i++) {
        ((uint16_t*) VGAMEM)[i] = (screen_color << 8) | ' ';
    }
    // adjust cursor position
    curline -= 1;
}
