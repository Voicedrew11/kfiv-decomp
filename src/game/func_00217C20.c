#include "kf_game.h"

extern TextLog D_01E0E9C0;
extern TextLogPacket D_01E0F210;

/* Places the text log's window (x, y, width and height in frame pixels, on a screen of the given size whose
   centre is at the GS offset), makes the text white and empties the log. */
void func_00217C20(int window_x, int window_y, int width, int height, int offset_x, int offset_y, int screen_width,
                   int screen_height, int depth) {
    TextLog *log = &D_01E0E9C0;

    log->left = window_x + offset_x - screen_width / 2;
    log->top = window_y / 2 + offset_y - screen_height / 2;
    log->width = width;
    log->height = height / 2;
    log->depth = depth;
    log->red = log->green = log->blue = 0xFF;
    if (width < 0x10) {
        log->width = 0x10;
    }
    log->used = 0;
    log->packet = (TextLogGlyph *)&D_01E0F210;
}
