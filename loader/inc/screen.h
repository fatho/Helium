#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>
#include <stddef.h>

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
extern uint32_t curline;
extern uint32_t curcol;

// current text color
extern uint8_t screen_color;

// writes a message to the VGA memory
void kputs(const char*);
void kputsn(const char*, size_t);
/**
 * A really stripped down version of printf which only supports the format specifiers
 *   "%%"   print single percent sign
 *   "%s"   print string
 *   "%x"   print 32 bit number as 64 bit hex string
 *   "%llx" print 64 bit number as 64 bit hex string
 */
void kprintf(const char*, ...);
// writes a number to the VGA memory
void kputi(uint64_t number);
// clears the screen with the specified color. sets the default color for subsequent kput* calls.
void clear_screen(uint8_t color);
// scrolls the buffer one line up
void scroll_up();

#endif
