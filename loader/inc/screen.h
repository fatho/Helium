#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

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
extern int curline;
extern int curcol;

// current text color
extern uint8_t screen_color;

// writes a message to the VGA memory
void kputs(char*);
// writes a number to the VGA memory
void kputi(uint64_t number);
// clears the screen with the specified color. sets the default color for subsequent kput* calls.
void clear_screen(uint8_t color);
// scrolls the buffer one line up
void scroll_up();

#endif
