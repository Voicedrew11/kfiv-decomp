#include "common.h"

s32 func_001D0588(s16 *, s32);

/* Width of a text string (terminated by -1) in the given font. */
int func_001D05E0(s16 *text, int font) {
    int width = 0;

    while (*text != -1) {
        width += func_001D0588(text, font);
        text++;
    }
    return width;
}
