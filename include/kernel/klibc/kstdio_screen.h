/**
 * @file kstdio_screen.h
 *
 * @author Fabian Thorand
 * @date   03.06.2014
 *
 * @brief 
 */
#ifndef KSTDIO_SCREEN_H_
#define KSTDIO_SCREEN_H_

#include "kernel/klibc/kstdio.h"

/// base address of VGA memory
#define VGAMEM  0xB8000
/// number of text columns
#define COLS    80
/// number of text rows
#define ROWS    25
/// size of VGA buffer in bytes
#define VGASIZE (COLS*ROWS*sizeof(uint16_t))


/**
 * Calculates a location inside the VGA buffer from \c x and \c y.
 *
 * @param x,y the VGA screen coordinate (80 columns, 25 rows)
 *
 * @remark ATTENTION! does not perform bound checks.
 */
#define VGAPTR(x,y) ((volatile uint16_t*)VGAMEM + y * COLS + x)

/**
 * Calculates a VGA color byte from fore- and background color.
 */
#define VGACOLOR(BG,FG) (((BG) << 4) | ((FG) & 0x0F))
#define VGASETFG(C,FG) ((C & 0xF0) | ((FG) & 0x0F))
#define VGASETBG(C,BG) ((C & 0x0F) | ((BG) & 0xF0))
/// the light color or blink bit
#define LIGHT_OR_BLINK 0x8
#define BLACK   0x0
#define BLUE    0x1
#define GREEN   0x2
#define CYAN    0x3
#define RED     0x4
#define MAGENTA 0x5
#define BROWN   0x6
#define GRAY    0x7
#define DARKGRAY (BLACK | LIGHT_OR_BLINK)
#define LIGHTBLUE (BLUE | LIGHT_OR_BLINK)
#define LIGHTGREEN (GREEN | LIGHT_OR_BLINK)
#define LIGHTCYAN (CYAN | LIGHT_OR_BLINK)
#define LIGHTRED (RED | LIGHT_OR_BLINK)
#define LIGHTMAGENTA (MAGENTA | LIGHT_OR_BLINK)
#define YELLOW (BROWN | LIGHT_OR_BLINK)
#define WHITE   (GRAY | LIGHT_OR_BLINK)

// current output position
extern uint32_t curline;
extern uint32_t curcol;

// current text color
extern uint8_t screen_color;

/// direct screen backend for ksdtio
extern kstdio_backend_t kstdio_screen_direct;

/**
 * @brief Saves the current screen color on a stack and sets the given colors.
 * @return remaining space on the stack or -1 if the stack is already full.
 */
int screen_push_color(uint8_t background, uint8_t foreground);

/**
 * @brief Restores the screen color from the stack.
 * @return The remaining space on the stack or -1 if the stack is already empty.
 */
int screen_pop_color();

/**
 * @brief clears the screen with the specified color. sets the default color for subsequent screen calls.
 */
void screen_clear(uint8_t color);

/**
 * @brief scrolls the VGA buffer one line up
 */
void screen_scroll();

/**
 * @brief Writes a character directly to the screen.
 */
void screen_kputchar(int chr);

#endif /* KSTDIO_SCREEN_H_ */
