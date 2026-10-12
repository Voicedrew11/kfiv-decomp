#include "kf_state.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include <eeregs.h>

/* Sends a seven-quadword packet that draws a textured rectangle as a four-corner strip. */
void func_001FBEB0(KindObject *object) {
    TexturedRectDef *rect = (TexturedRectDef *)object;
    u32 *packet;

    if (rect != NULL) {
        packet = (u32 *)func_001FBE70();
        packet[0] = 0x8001;
        packet[1] = 0xC4000000;
        packet[2] = 0x25252106;
        packet[3] = 0xFFFFF525;
        *(s64 *)&packet[4] = rect->tex0;
        packet[6] = 0x54;
        packet[7] = 0;
        packet[8] = rect->red | (rect->green << 8) | (rect->blue << 16) | (rect->alpha << 24);
        *(f32 *)&packet[9] = rect->scale;
        *(f32 *)&packet[10] = rect->first_s;
        *(f32 *)&packet[11] = rect->second_t;
        packet[12] = rect->first.pos_x | ((s32)rect->second.pos_y << 16);
        packet[13] = rect->first.depth;
        *(f32 *)&packet[14] = rect->first_s;
        *(f32 *)&packet[15] = rect->first_t;
        packet[16] = rect->first.pos_x | ((s32)rect->first.pos_y << 16);
        packet[17] = rect->first.depth;
        *(f32 *)&packet[18] = rect->second_s;
        *(f32 *)&packet[19] = rect->second_t;
        packet[20] = rect->second.pos_x | ((s32)rect->second.pos_y << 16);
        packet[21] = rect->first.depth;
        *(f32 *)&packet[22] = rect->second_s;
        *(f32 *)&packet[23] = rect->first_t;
        packet[24] = rect->second.pos_x | ((s32)rect->first.pos_y << 16);
        packet[25] = rect->first.depth;
        while (*D2_CHCR & 0x100);
        *D2_QWC = 7;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
    }
}
