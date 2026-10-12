#include "kf_movie.h"
#include <libgraph.h>

/* Shows one of the two fields' frame buffers. */
void func_0021ED10(MovieDisplay *display, s32 second) {
    s32 frame_base;

    if (second) {
        frame_base = display->frame_base[1];
    } else {
        frame_base = display->frame_base[0];
    }
    display->dispfb = (display->dispfb & ~0x1FF) | (frame_base & 0x1FF);
    sceGsPutDispEnv(display);
}
