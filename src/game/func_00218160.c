#include "kf_game.h"

extern TextLog D_01E0E9C0;

/* Sets the text log's colour from three ints: red, green, blue. */
void func_00218160(int *colour) {
    TextLog *log = &D_01E0E9C0;

    log->red = colour[0];
    log->green = colour[1];
    log->blue = colour[2];
}
