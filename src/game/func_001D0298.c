#include "kf_game.h"
#include "kf_gs.h"

extern s32 D_00420B40[16][4];
extern GlyphSheet D_00420C10[3];
extern GlyphBox D_00420C68[][3];
extern u32 *D_004471D8;
extern GsRegisterPair D_005C58C8[2];

/* Adds one glyph to the text packet at D_004471D8: PRIM, the TEX0 write of its texture page, the colour and the
   two corners (ST and XYZ each). */
void func_001D0298(int pos_x, int pos_y, int glyph, int colour, int alpha, int kind, int font) {
    s32 corners[2][4];
    f32 tex[2][2];
    s32 first_page = glyph < 0x271;
    s32 index;
    s32 column;
    s32 row;
    s32 i;
    s32 *corner;
    f32 *coord;
    s32 *rgb;

    index = glyph;
    if (!first_page) {
        index -= 0x271;
    }
    column = index % D_00420C10[kind].columns;
    tex[0][0] = column * D_00420C10[kind].step_s + D_00420C10[kind].base_s;
    row = index / D_00420C10[kind].columns;
    tex[0][1] = row * D_00420C10[kind].step_t + D_00420C10[kind].base_t;
    tex[1][0] = tex[0][0] + D_00420C10[kind].size_s;
    tex[1][1] = tex[0][1] + D_00420C10[kind].size_t;
    corners[0][0] = (pos_x + 0x800 + D_00420C68[font][kind].left) << 4;
    corners[0][1] = (pos_y + 0x800 + D_00420C68[font][kind].top) << 4;
    corners[1][0] = (pos_x + 0x800 + D_00420C68[font][kind].right) << 4;
    corners[1][1] = (pos_y + 0x800 + D_00420C68[font][kind].bottom) << 4;
    corners[0][2] = -1;
    corners[1][2] = -1;
    corners[0][3] = 0;
    corners[1][3] = 0;
    *D_004471D8++ = 0x56;
    *D_004471D8++ = 0;
    *D_004471D8++ = 0;
    *D_004471D8++ = 0;
    if (first_page && kind == 0) {
        *((GsRegisterPair *)D_004471D8)++ = D_005C58C8[0];
    } else {
        *((GsRegisterPair *)D_004471D8)++ = D_005C58C8[1];
    }
    rgb = D_00420B40[colour];
    *D_004471D8++ = rgb[0];
    *D_004471D8++ = rgb[1];
    *D_004471D8++ = rgb[2];
    *D_004471D8++ = alpha;
    corner = corners[0];
    coord = tex[0];
    for (i = 0; i < 2; i++) {
        *(f32 *)D_004471D8++ = coord[0];
        *(f32 *)D_004471D8++ = coord[1];
        *(f32 *)D_004471D8++ = 1.0f;
        *(f32 *)D_004471D8++ = 0.0f;
        *D_004471D8++ = corner[0];
        *D_004471D8++ = corner[1];
        *D_004471D8++ = corner[2];
        *D_004471D8++ = corner[3];
        corner += 4;
        coord += 2;
    }
}
