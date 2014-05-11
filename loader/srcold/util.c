#include "util.h"
#include "screen.h"

void kpanic(char* message) {
    clear_screen(TEXTCOLOR(BLUE, WHITE));
    kputs(message);
    while (1)
        ;
}
