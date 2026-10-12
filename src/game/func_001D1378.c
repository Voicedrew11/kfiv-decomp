#include "kf_packet.h"

/* Fill a screen sprite: GS state, colour, and a pos_x/pos_y/width/height box (in pixels) with its texture box. */
void func_001D1378(ScreenSprite *sprite, s64 prim, u64 tex0, u64 tex1, u8 red, u8 green, u8 blue, u8 alpha,
                   s32 pos_x, s32 pos_y, u32 depth, s32 width, s32 height, s32 first_s, s32 first_t, s32 second_s,
                   s32 second_t) {
    sprite->prim = prim;
    sprite->tex0 = tex0;
    sprite->tex1 = tex1;
    sprite->rgba[0] = red;
    sprite->rgba[1] = green;
    sprite->rgba[2] = blue;
    sprite->rgba[3] = alpha;
    pos_x += 0x10;
    pos_y += 0x10;
    sprite->vertex[0].x = (pos_x + 0x6C0) << 4;
    sprite->vertex[0].y = (pos_y + 0x720) << 4;
    width += 0x6C0;
    height += 0x720;
    sprite->vertex[1].x = (pos_x + width) << 4;
    sprite->vertex[1].y = (pos_y + height) << 4;
    sprite->vertex[0].z = depth;
    sprite->vertex[0].tex_t = first_t << 4;
    sprite->vertex[1].z = depth;
    sprite->vertex[0].tex_s = first_s << 4;
    sprite->vertex[1].tex_s = second_s << 4;
    sprite->vertex[1].tex_t = second_t << 4;
}
