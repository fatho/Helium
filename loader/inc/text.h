#pragma once

#include <stdint.h>

// writes a message to the VGA memory
void kernel_msg(char*);
// scrolls the buffer one line up
void scroll_up();

