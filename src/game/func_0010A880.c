#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_texture.h"

extern u32 D_00446ED4;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109F80(s64, s64, s64);
void func_0010A100(void *);

/* Scroll a texture and redraw it into its target texture. */
void func_0010A880(void *texture_arg, void *offset_arg) {
    ScrollTexture *texture = texture_arg;
    ScrollOffset *offset = offset_arg;
    ScreenSprite sprite;
    u32 width;
    u32 height;
    u32 size;
    u32 source_width;
    u32 source_height;
    u32 width_log2;
    u32 height_log2;
    u32 half;

    source_width = texture->width;
    source_height = texture->height;
    width = source_width << 4;
    height = source_height << 4;
    if (offset != NULL) {
        if (texture->period == 0 || D_00446ED4 % texture->period == 0) {
            if (texture->step_x != 0) {
                offset->x += texture->step_x;
                if (offset->x < 0) {
                    offset->x += width;
                }
                if (offset->x >= width) {
                    offset->x -= width;
                }
            }
            if (texture->step_y != 0) {
                offset->y += texture->step_y;
                if (offset->y < 0) {
                    offset->y += height;
                }
                if (offset->y >= height) {
                    offset->y -= height;
                }
            }
            source_width = texture->width;
            source_height = texture->height;
        }
    }
    for (width_log2 = 0, size = source_width; size >= 2; size >>= 1) {
        width_log2++;
    }
    for (height_log2 = 0, size = source_height; size >= 2; size >>= 1) {
        height_log2++;
    }
    func_00109F80((texture->target_fbp / 32) | ((u64)(texture->target_width / 64) << 16) | ((u64)texture->target_psm << 24),
                  ((u64)(texture->target_width - 1) << 16) | ((u64)(texture->target_height - 1) << 48),
                  ((u64)(0x800 - (texture->target_width >> 1)) << 4) | ((u64)(0x800 - (texture->target_height >> 1)) << 36));
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, (u64)texture->alpha_a | ((u64)texture->alpha_b << 2) | ((u64)texture->alpha_c << 4) |
                                      ((u64)texture->alpha_d << 6) | ((u64)texture->alpha_fix << 32));
    func_00109A80(GS_REG_TEST_1, 0x53001);
    func_00109AD0();
    func_00109E18();
    // match: the half width is read before TEX0 is built (it decides the registers of the TEX0 terms)
    half = texture->target_width >> 1;
    sprite.tex0 = (u64)texture->tbp | ((u64)(texture->width >> 6) << 14) | ((u64)texture->psm << 20) | ((u64)width_log2 << 26) |
                  ((u64)height_log2 << 30) | (1L << 34) | ((u64)texture->cbp << 37) | ((u64)texture->cpsm << 51) | (4L << 61);
    sprite.tex1 = 0x60;
    sprite.prim = 0x156;
    sprite.rgba[0] = texture->rgba[0];
    sprite.rgba[1] = texture->rgba[1];
    sprite.rgba[2] = texture->rgba[2];
    sprite.rgba[3] = texture->rgba[3];
    sprite.vertex[0].x = (0x800 - half) << 4;
    sprite.vertex[0].y = (0x800 - half) << 4;
    sprite.vertex[0].z = 0xFFFFFFFF;
    sprite.vertex[0].tex_s = 0;
    sprite.vertex[0].tex_t = 0;
    sprite.vertex[1].x = (half << 4) + 0x7FF8;
    sprite.vertex[1].y = (half << 4) + 0x7FF8;
    sprite.vertex[1].z = 0xFFFFFFFF;
    sprite.vertex[1].tex_s = (f32)width * texture->repeat_x;
    sprite.vertex[1].tex_t = (f32)height * texture->repeat_y;
    if (offset != NULL) {
        sprite.vertex[0].tex_s += offset->x;
        sprite.vertex[0].tex_t += offset->y;
        sprite.vertex[1].tex_s += offset->x;
        sprite.vertex[1].tex_t += offset->y;
    }
    func_0010A100(&sprite);
    func_00109E18();
}
