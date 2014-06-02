#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

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
 * @brief writes a null terminated string to the VGA buffer.
 */
void kputs(const char* msg);

/**
 * @brief writes a null terminated string to the VGA buffer, but at most \c size characters.
 */
void kputsn(const char* msg, size_t size);

/**
 * @brief Writes a character to the screen.
 *
 * @param chr the character
 */
void kputchar(int chr);

/**
 * @brief A really simple implementation of printf.
 *
 * The following format specifiers are supported:
 *   - "%%"   print single percent sign
 *   - "%s"   print string
 *   - "%x"   print 32 bit number as 64 bit hex string
 *   - "%llx" print 64 bit number as 64 bit hex string
 */
void kprintf(const char*, ...);

/**
 * @brief same as \c kprintf(const char*, ...), but with explicit variable argument list.
 */
void kvprintf(const char* format, va_list vl);

/**
 * @brief writes a number to the VGA memory in 64 bit hexadecimal format.
 */
void kputhex(uint64_t number);

/**
 * @brief clears the screen with the specified color. sets the default color for subsequent screen calls.
 */
void clear_screen(uint8_t color);

/**
 * @brief scrolls the VGA buffer one line up
 */
void scroll_up();

#endif
