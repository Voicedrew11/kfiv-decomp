#include "kf_game.h"
#include <eeregs.h>

extern TextLog D_01E0E9C0;
extern TextLogPacket D_01E0F210;

int FlushCache(int);
void func_00217CB8(char, int, int, int);
void func_00217DA0(void);

/* Draws the text log in its window, wrapping at the right edge and stopping at the bottom, then empties it. */
void func_00217F98(void) {
    TextLogPacket *packet = &D_01E0F210;
    int vertex_count = 0;
    int remaining = D_01E0E9C0.used;
    char *text = D_01E0E9C0.text;
    int pos_x;
    int pos_y;

    D_01E0E9C0.packet = packet->glyphs;
    pos_x = D_01E0E9C0.left;
    pos_y = D_01E0E9C0.top;
    while (remaining-- != 0) {
        if (*text == '\n') {
            pos_y += 8;
            pos_x = D_01E0E9C0.left;
        } else {
            func_00217CB8(*text, pos_x, pos_y, D_01E0E9C0.depth);
            pos_x += 16;
            vertex_count += 2;
            if (D_01E0E9C0.left + D_01E0E9C0.width - 16 < pos_x) {
                pos_x = D_01E0E9C0.left;
                pos_y += 8;
            }
        }
        if (D_01E0E9C0.top + D_01E0E9C0.height - 8 < pos_y) {
            break;
        }
        text++;
    }
    if (vertex_count != 0) {
        func_00217DA0();
        packet->tag[0] = vertex_count | 0x8000;
        packet->tag[1] = 0x30AB4000;
        packet->tag[2] = 0x413;
        packet->tag[3] = 0;
        FlushCache(0);
        while (*D2_CHCR & 0x100) {
        }
        *D2_QWC = ((int)D_01E0E9C0.packet - (int)&D_01E0F210) >> 4;
        *D2_MADR = (u32)&D_01E0F210 & 0x7FFFFFFF;
        *D2_CHCR = 0x100;
    }
    D_01E0E9C0.used = 0;
}
