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
int curline = 0;
int curcol = 0;

// current text color
uint8_t screen_color = TEXTCOLOR(BLACK, WHITE);

void kputs(char* message) {
    volatile uint16_t* dest = VGAPTR(curcol, curline);
    // output until '\0' terminator
    while (*message) {
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
        uint8_t block = (number >> (4 * (15-i))) & 0xF;
        if(block < 10) {
            string[i] = '0' + block;
        } else {
            string[i] = 'A' + (block - 10);
        }
    }
    string[16] = 0;
    kputs(string);
}

void clear_screen(uint8_t color) {
    volatile uint16_t* dest = VGAPTR(0, 0);
    screen_color = color;
    curcol = 0;
    curline = 0;
    for(int i = 0; i < COLS*ROWS; i++) {
        dest[i] = (color << 8) | ' ';
    }
}

void scroll_up() {
    for(int i = 80*25-1; i>= 0; i--) {
        ((uint16_t*)VGAMEM)[i] = ((uint16_t*)VGAMEM)[i - 80];
    }
    // adjust cursor position
    curline -= 1;
}
