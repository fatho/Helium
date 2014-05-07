#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdint.h>

// writes a message to the VGA memory
void kputs(char*);
// writes a number to the VGA memory
void kputi(uint64_t number);
// scrolls the buffer one line up
void scroll_up();

#endif
