#include <stdint.h>

void halt() {
    while (1)
        ;
}

void kernel_main() {
    uint16_t* vga = (uint16_t*)0xB8000;
    uint16_t color_mask = 0x0F00;
    char* output = "Hello World from 64 Bit!";
    for(int i = 0; i < 24; i++) {
        vga[i] = output[i] | color_mask;
    }
    // hang
    halt();
}
