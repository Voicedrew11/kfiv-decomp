#include "kf_gs.h"
#include "kf_map.h"
#include "kf_packet.h"

extern MapSlot D_0038CD20[];
extern u32 D_00446ED4;
extern MapSlotPair D_00471FE0[2][16];

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_00109F80(s64, s64, s64);
void func_0010A008(void);
void func_0010A100(void *);

/* Draws each of a map's moving textures of this mode into its target texture, shifted by the pair that
   goes with it, and moves the pair along. */
void func_001175F8(int map, int mode) {
    ScreenQuad quad;
    MapSlot *slot = &D_0038CD20[map];
    MapSlotPair *pair = D_00471FE0[map];
    MapScrollTexture *texture;
    int i;
    u32 width;
    u32 height;
    u32 size;
    u32 width_log;
    u32 height_log;
    int half;

    texture = (MapScrollTexture *)slot->unk3C;
    if (slot->unk00 != 0) {
        for (i = 0; i < 16; i++, texture++, pair++) {
            if (texture->target_base != 0 && texture->mode == mode) {
                width = texture->width << 4;
                height = texture->height << 4;
                if (texture->period == 0 || D_00446ED4 % texture->period == 0) {
                    if (texture->step_x != 0) {
                        pair->unk00 += texture->step_x;
                        if (pair->unk00 < 0) {
                            pair->unk00 += width;
                        }
                        if (pair->unk00 >= width) {
                            pair->unk00 -= width;
                        }
                    }
                    if (texture->step_y != 0) {
                        pair->unk02 += texture->step_y;
                        if (pair->unk02 < 0) {
                            pair->unk02 += height;
                        }
                        if (pair->unk02 >= height) {
                            pair->unk02 -= height;
                        }
                    }
                }
                for (width_log = 0, size = texture->width; size >= 2; size >>= 1) {
                    width_log++;
                }
                for (height_log = 0, size = texture->height; size >= 2; size >>= 1) {
                    height_log++;
                }
                func_00109F80(((s64)texture->target_base >> 5) | (((s64)texture->target_width / 64) << 16) |
                                  ((s64)texture->target_format << 24),
                              ((s64)(texture->target_width - 1) << 16) | ((s64)(texture->target_height - 1) << 48),
                              ((s64)(0x800 - (texture->target_width >> 1)) << 4) |
                                  ((s64)(0x800 - (texture->target_height >> 1)) << 36));
                func_00109A50();
                // match: A is read from the byte, the others from the word
                func_00109A80(GS_REG_ALPHA_1,
                              (s64)(texture->blend & 3) | ((s64)MAP_SCROLL_BLEND_BITS(texture)->b << 2) |
                                  ((s64)MAP_SCROLL_BLEND_BITS(texture)->c << 4) |
                                  ((s64)MAP_SCROLL_BLEND_BITS(texture)->d << 6) | ((s64)texture->blend_fix << 32));
                func_00109A80(GS_REG_TEST_1, 0x51001);
                func_00109AD0();
                func_00109E18();
                quad.tag = (s64)texture->base | ((s64)texture->width >> 6 << 14) | ((s64)texture->format << 20) |
                           ((s64)width_log << 26) | ((s64)height_log << 30) | (1L << 34) |
                           ((s64)texture->clut_base << 37) | ((s64)texture->clut_format << 51) | (4L << 61);
                quad.unk08 = 0x60;
                quad.unk10 = 0x156;
                half = texture->target_width >> 1;
                quad.red = texture->red;
                quad.green = texture->green;
                quad.blue = texture->blue;
                quad.alpha = texture->alpha;
                quad.y0 = quad.x0 = (0x800 - half) << 4;
                quad.z0 = 0xFFFFFFFF;
                quad.u0 = 0;
                quad.v0 = 0;
                quad.y1 = quad.x1 = (half << 4) + 0x7FF8;
                quad.z1 = 0xFFFFFFFF;
                quad.u1 = width * texture->scale_x;
                quad.v1 = height * texture->scale_y;
                quad.u0 += pair->unk00;
                quad.v0 += pair->unk02;
                quad.u1 += pair->unk00;
                quad.v1 += pair->unk02;
                func_0010A100(&quad);
                func_00109E18();
                func_00109E88();
            }
        }
        func_00109E88();
        func_0010A008();
    }
}
