#include "kf_game.h"

extern TextLog D_01E0E9C0;

/* Queues one character of the text log's font (16 characters a row, starting at '!') at a GS position.
   Anything up to the space character is drawn as character 0x80. */
void func_00217CB8(char character, int pos_x, int pos_y, int pos_z) {
    TextLog *log = &D_01E0E9C0;
    TextLogGlyph *glyph = log->packet;
    int code;
    int tex_u;
    int tex_v;

    code = character;
    if (code <= 0x20) {
        code = 0x80;
    }
    glyph->corners[0].red = log->red;
    glyph->corners[0].green = log->green;
    glyph->corners[0].blue = log->blue;
    glyph->corners[0].alpha = 0x80;
    tex_u = (((code - 0x21) & 0xF) << 8) | 0x10;
    tex_v = (((code - 0x21) / 16) << 8) | 0x10;
    glyph->corners[0].tex_u = tex_u;
    glyph->corners[0].tex_v = tex_v;
    glyph->corners[0].pos_x = pos_x << 4;
    glyph->corners[0].pos_y = pos_y << 4;
    glyph->corners[0].pos_z = pos_z << 4;
    glyph->corners[1].red = log->red;
    glyph->corners[1].green = log->green;
    glyph->corners[1].blue = log->blue;
    glyph->corners[1].alpha = 0x80;
    glyph->corners[1].tex_u = tex_u + 0x100;
    glyph->corners[1].tex_v = tex_v + 0x100;
    glyph->corners[1].pos_x = (pos_x + 16) << 4;
    glyph->corners[1].pos_y = (pos_y + 8) << 4;
    glyph->corners[1].pos_z = pos_z << 4;
    log->packet = glyph + 1;
}
