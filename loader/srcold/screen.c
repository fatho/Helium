#include "screen.h"
#include "string.h"
#include "util.h"
#include <stdarg.h>
#include <stdint.h>

// base address of VGA memory
#define VGAMEM  0xB8000
// number of text columns
#define COLS    80
// number of text rows
#define ROWS    25
// size of VGA buffer
#define VGASIZE (COLS*ROWS*sizeof(uint16_t))

// calculates character position inside VGA memory
#define VGAPTR(x,y) ((volatile uint16_t*)VGAMEM + y * COLS + x)

// text colors
#define TEXTCOLOR(BG,FG) ((BG << 4) | (FG & 0x0F))
#define BLACK   0x0
#define BLUE    0x1
#define GREEN   0x2
#define CYAN    0x3
#define RED     0x4
#define MAGENTA 0x5
#define BROWN   0x6
#define GRAY    0x7
#define WHITE   0xF

// current output position
uint32_t curline = 0;
uint32_t curcol = 0;

// current text color
uint8_t screen_color = TEXTCOLOR(BLACK, WHITE);

void kputs(const char* message) {
    kputsn(message, strlen(message));
}

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

void kprintf(const char* format, ...) {
    va_list vl;
    va_start(vl, format);

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

    va_end(vl);
}

void clear_screen(uint8_t color) {
    volatile uint16_t* dest = VGAPTR(0, 0);
    screen_color = color;
    curcol = 0;
    curline = 0;
    for (int i = 0; i < COLS * ROWS; i++) {
        dest[i] = (color << 8) | ' ';
    }
}

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
