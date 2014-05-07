#include "screen.h"
#include <stdint.h>

// facts about the VGA text mode memory
#define VGAMEM (0xB8000) // base address
#define COLS 80          // number of text columns
#define ROWS 25          // number of text rows
// calculates character position inside VGA memory
#define VGAPTR(x,y) ((volatile uint16_t*)VGAMEM + y * COLS + x)

// text colors
#define TEXTCOLOR(BG,FG) ((BG << 4) | (FG & 0x0F))
#define BLACK 0x0
#define WHITE 0xF

// current output position
static int curline = 0;
static int curcol = 0;

void kernel_msg(char* message) {
    volatile uint16_t* dest = VGAPTR(curcol, curline);
    uint8_t color = TEXTCOLOR(BLACK, WHITE);
    // output until '\0' terminator
    while (*message) {
        if (*message == '\n') {
            curcol = COLS;
        } else {
            *dest = *message | (color << 8);
            dest++;
            curcol++;
        }
        if (curcol == COLS) {
            curcol = 0;
            curline += 1;
            if (curline == ROWS) {
                scroll_up();
            }
            dest = VGAPTR(curcol, curline);
        }
        message++;
    }
}

void scroll_up() {
    volatile uint16_t* src = VGAPTR(0, 0);
    volatile uint16_t* dest = VGAPTR(0, 1);
    volatile uint16_t* end = VGAPTR(0, ROWS);
    // move lines one row upwards
    while (dest != end) {
        *dest = *src;
        dest++;
        src++;
    }
    // clear last row
    while (src != dest) {
        *src = 0;
        src++;
    }
    // adjust cursor position
    curline -= 1;
}
