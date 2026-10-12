#include "kf_menu.h"
#include "kf_packet.h"
#include <eeregs.h>

/* Sends a four-quadword packet that draws a flat rectangle as a four-corner strip. */
void func_001FC5E8(MenuCursorBar *bar) {
    FlatRectDef *rect = (FlatRectDef *)bar;
    GsCornerBits first;
    GsCornerBits second;
    u32 *packet;

    if (rect != NULL) {
        packet = (u32 *)func_001FBE70();
        first.pos_x = rect->first.pos_x;
        first.pos_y = rect->first.pos_y;
        first.depth = rect->first.depth;
        second.pos_x = rect->second.pos_x;
        second.pos_y = rect->second.pos_y;
        second.depth = rect->first.depth; // match: the first corner's depth, and neither copy is read again
        packet[0] = 0x8001;
        packet[1] = 0x64000000;
        packet[2] = 0xFF555510;
        packet[3] = 0;
        packet[4] = 0x44;
        packet[5] = 0;
        packet[6] = rect->red | (rect->green << 8) | (rect->blue << 16) | (rect->alpha << 24);
        *(f32 *)&packet[7] = rect->scale;
        packet[8] = (s32)first.pos_x | ((s32)second.pos_y << 16);
        packet[9] = rect->first.depth;
        packet[10] = (s32)first.pos_x | ((s32)first.pos_y << 16);
        packet[11] = rect->first.depth;
        packet[12] = (s32)second.pos_x | ((s32)second.pos_y << 16);
        packet[13] = rect->first.depth;
        packet[14] = (s32)second.pos_x | ((s32)first.pos_y << 16);
        packet[15] = rect->first.depth;
        while (*D2_CHCR & 0x100);
        *D2_QWC = 4;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
    }
}
