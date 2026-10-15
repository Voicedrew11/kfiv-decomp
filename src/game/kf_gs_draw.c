/* GS drawing helpers built on the packet buffer: image transfers, register presets, screen sprites and
   fills, matrix packets, scrolling textures and copying the frame into a texture. */
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_texture.h"

u32 *Packet_Alloc(int);
void Packet_AppendToChain(u32 *, u32 *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_SetDrawTarget(s64, s64, s64);
void GsDraw_QueueImageTransfer(int, int, int, int, int, int, int, int, int, int, int, int, int);
void GsDraw_QueueFlatSprite(void *);
void GsDraw_QueueFlush(void);
void GsDraw_QueueTexturedSprite(void *);
extern int D_00446F78;
extern u32 D_00446ED4;

/* Queues a GS local-to-local image transfer (BITBLTBUF, TRXPOS, TRXREG, TRXDIR). */
void GsDraw_QueueImageTransfer(int source_base, int source_format, int source_width, int source_x, int source_y, int width,
                   int height, int dest_base, int dest_format, int dest_width, int dest_x, int dest_y,
                   int direction) {
    u32 *cursor = Packet_Alloc(8);
    u32 *packet = cursor;

    *cursor++ = 0x10000006;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0;
    *cursor++ = 0x51000005;
    *(u64 *)cursor = 0x1000000000008004;
    cursor += 2;
    *(u64 *)cursor = 0xE;
    cursor += 2;
    *(u64 *)cursor = (u64)source_base | ((u64)source_width << 16) | ((u64)source_format << 24) |
                     ((u64)dest_base << 32) | ((u64)dest_width << 48) | ((u64)dest_format << 56);
    cursor += 2;
    *cursor++ = 0x50;
    *cursor++ = 0;
    *(u64 *)cursor = (u64)source_x | ((u64)source_y << 16) | ((u64)dest_x << 32) | ((u64)dest_y << 48) |
                     ((u64)direction << 59);
    cursor += 2;
    *cursor++ = 0x51;
    *cursor++ = 0;
    *(u64 *)cursor = (u64)width | ((u64)height << 32);
    cursor += 2;
    *cursor++ = 0x52;
    *cursor++ = 0;
    *(u64 *)cursor = 2;
    cursor += 2;
    *cursor++ = 0x53;
    *cursor++ = 0;
    Packet_AppendToChain(packet, cursor);
}

/* Queues a packet that only holds a VIF FLUSHA: what follows waits until the GS paths are idle. */
void GsDraw_QueueFlush(void) {
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = Packet_Alloc(3);
    *packet++ = 0x10000001;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x13000000;
    Packet_AppendToChain(packet_start, packet);
}

/* Queues a GS TEXFLUSH write. */
void GsDraw_QueueTexFlush(void) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEXFLUSH, 0xFFFFFFFFL);
    Packet_EndGsRegisters();
}

/* Queues the depth state for normal drawing in both contexts: Z test GREATER with Z writes on. */
void GsDraw_EnableDepthTest(void) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x70000);
    Packet_AddGsRegister(0x48, 0x70000);
    Packet_AddGsRegister(0x4E, 0xA000118);
    Packet_AddGsRegister(0x4F, 0xA000118);
    Packet_EndGsRegisters();
}

/* Queues the depth state that ignores depth in both contexts: Z test ALWAYS with Z writes off. */
void GsDraw_DisableDepthTest(void) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
    Packet_EndGsRegisters();
}

/* Queues FRAME, SCISSOR and XYOFFSET (both contexts) for drawing into another buffer. */
void GsDraw_SetDrawTarget(s64 frame, s64 scissor, s64 xy_offset) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_FRAME_1, frame);
    Packet_AddGsRegister(GS_REG_FRAME_2, frame);
    Packet_AddGsRegister(GS_REG_SCISSOR_1, scissor);
    Packet_AddGsRegister(GS_REG_SCISSOR_2, scissor);
    Packet_AddGsRegister(GS_REG_XYOFFSET_1, xy_offset);
    Packet_AddGsRegister(GS_REG_XYOFFSET_2, xy_offset);
    Packet_EndGsRegisters();
}

/* Queues the draw target back to the 640x448 frame being drawn. */
void GsDraw_ResetDrawTarget(void) {
    u32 frame_base = 0x1180;

    if (D_00446F78 != 0) {
        frame_base = 0;
    }
    GsDraw_SetDrawTarget((frame_base >> 5) | 0xA0000, 0x1BF0000027F0000L, 0x720000006C00L);
}

/* Queues a copy of the 640x448 frame being drawn to dest_base in GS memory. */
void GsDraw_CopyFrame(int dest_base) {
    GsDraw_QueueImageTransfer((D_00446F78 != 0) ? 0 : 0x1180, 0, 0xA, 0, 0, 0x280, 0x1C0, dest_base, 0, 0xA, 0, 0, 2);
}

/* The same as GsDraw_CopyFrame; nothing calls it. */
void func_0010A0A0(int value) {
    GsDraw_QueueImageTransfer((D_00446F78 != 0) ? 0 : 0x1180, 0, 0xA, 0, 0, 0x280, 0x1C0, value, 0, 0xA, 0, 0, 2);
}

/* Queue a textured screen sprite. */
void GsDraw_QueueTexturedSprite(void *sprite_arg) {
    ScreenSprite *sprite = sprite_arg;
    ScreenSpriteVertex *vertex;
    u32 *packet_start;
    u32 *packet;
    s32 prim;

    prim = sprite->prim;
    packet_start = packet = Packet_Alloc(0xD);
    *packet++ = 0x1000000B;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C09C000;
    *(u64 *)packet = ((s64)(prim | 6) << 47) | 0x8000400000008001L;
    packet += 2;
    *(u64 *)packet = 0x513513EE;
    packet += 2;
    *(u64 *)packet = sprite->tex0;
    packet += 2;
    *packet++ = 6;
    *packet++ = 0;
    *(u64 *)packet = sprite->tex1;
    packet += 2;
    *packet++ = 0x14;
    *packet++ = 0;
    vertex = &sprite->vertex[0];
    *packet++ = vertex->tex_s;
    *packet++ = vertex->tex_t;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 1;
    vertex = &sprite->vertex[1];
    *packet++ = vertex->tex_s;
    *packet++ = vertex->tex_t;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x15000276;
    Packet_AppendToChain(packet_start, packet);
}

/* Queue a flat-coloured (untextured) screen sprite. */
void GsDraw_QueueFlatSprite(void *sprite_arg) {
    ScreenSprite *sprite = sprite_arg;
    ScreenSpriteVertex *vertex;
    u32 *packet_start;
    u32 *packet;
    s32 prim;

    prim = sprite->prim & 0xFFFFFFE9;
    packet_start = packet = Packet_Alloc(9);
    prim |= 6;
    *packet++ = 0x10000007;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C05C000;
    *(u64 *)packet = ((s64)prim << 47) | 0x4000400000008001L;
    packet += 2;
    *(u64 *)packet = 0x5151;
    packet += 2;
    vertex = &sprite->vertex[0];
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 1;
    vertex = &sprite->vertex[1];
    *packet++ = sprite->rgba[0];
    *packet++ = sprite->rgba[1];
    *packet++ = sprite->rgba[2];
    *packet++ = sprite->rgba[3];
    *packet++ = vertex->x;
    *packet++ = vertex->y;
    *packet++ = vertex->z;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x15000276;
    Packet_AppendToChain(packet_start, packet);
}

/* Cover the screen with a blended flat colour, in ten vertical strips. */
void GsDraw_FillScreen(int red, int green, int blue, int alpha, int blend) {
    ScreenSprite sprite;
    int strips_left;

    strips_left = 9;
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, blend);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_EndGsRegisters();
    sprite.prim = 0x46;
    sprite.rgba[0] = red;
    sprite.rgba[1] = green;
    sprite.rgba[2] = blue;
    sprite.rgba[3] = alpha;
    sprite.vertex[0].x = 0x6C00;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[1].x = 0x7000;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[0].z = 0;
    sprite.vertex[1].z = 0;
    do {
        strips_left--;
        GsDraw_QueueFlatSprite(&sprite);
        sprite.vertex[0].x += 0x400;
        sprite.vertex[1].x += 0x400;
    } while (strips_left >= 0);
}

/* Clears the 640x448 frame to black with one sprite (Z test ALWAYS, Z writes off, so the depth is kept), then
   puts the normal depth test back. */
void GsDraw_ClearFrame(void) {
    ScreenSprite sprite;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x30000);
    Packet_AddGsRegister(0x4E, 0x10A000118UL);
    Packet_EndGsRegisters();
    sprite.prim = 6;
    sprite.vertex[0].x = 0x6C00;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[1].x = 0x9400;
    sprite.vertex[1].y = 0x8E00;
    sprite.rgba[0] = 0;
    sprite.rgba[1] = 0;
    sprite.rgba[2] = 0;
    sprite.rgba[3] = 0;
    sprite.vertex[0].z = 0;
    sprite.vertex[1].z = 0;
    GsDraw_QueueFlatSprite(&sprite);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x47, 0x70000);
    Packet_AddGsRegister(0x4E, 0x0A000118);
    Packet_EndGsRegisters();
}

/* Clears the depth of the 640x448 frame to 0: a sprite whose every pixel fails the alpha test and so writes
   only Z. */
void GsDraw_ClearDepth(void) {
    ScreenSprite sprite;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x32001);
    Packet_EndGsRegisters();
    sprite.prim = 6;
    sprite.rgba[0] = 0;
    sprite.rgba[1] = 0;
    sprite.rgba[2] = 0;
    sprite.rgba[3] = 0;
    sprite.vertex[0].x = 0x6C00;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[0].z = 0;
    sprite.vertex[1].x = 0x9400;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[1].z = 0;
    GsDraw_QueueFlatSprite(&sprite);
}

/* Sets the alpha channel of the whole frame to `alpha`, leaving its colours: a black sprite in ten strips,
   blended as Cd and written to the frame only. */
void GsDraw_SetScreenAlpha(int alpha) {
    ScreenSprite sprite;
    int strips_left;

    strips_left = 9;
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, ((s64)alpha << 0x20) | 0x68);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_EndGsRegisters();
    sprite.prim = 0x46;
    sprite.rgba[3] = alpha;
    sprite.rgba[0] = 0;
    sprite.rgba[1] = 0;
    sprite.rgba[2] = 0;
    sprite.vertex[0].x = 0x6C00;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[0].z = 0;
    sprite.vertex[1].x = 0x7000;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[1].z = 0;
    do {
        strips_left--;
        GsDraw_QueueFlatSprite(&sprite);
        sprite.vertex[0].x += 0x400;
        sprite.vertex[1].x += 0x400;
    } while (strips_left >= 0);
}

/* Queues a packet that unpacks a 4x4 matrix to VU1 memory address 0. */
void GsDraw_QueueVu1Matrix(void *matrix) {
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = Packet_Alloc(7);
    *packet++ = 0x10000005;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C040000;
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf17, 0x10(%0)\n"
        "lqc2 $vf18, 0x20(%0)\n"
        "lqc2 $vf19, 0x30(%0)\n"
        "sqc2 $vf16, 0x0(%1)\n"
        "sqc2 $vf17, 0x10(%1)\n"
        "sqc2 $vf18, 0x20(%1)\n"
        "sqc2 $vf19, 0x30(%1)\n"
        : : "r"(matrix), "r"(packet));
    packet += 0x10;
    Packet_AppendToChain(packet_start, packet);
}

/* Scroll a texture and redraw it into its target texture. */
void GsDraw_DrawScrollingTexture(void *texture_arg, void *offset_arg) {
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
    GsDraw_SetDrawTarget((texture->target_fbp / 32) | ((u64)(texture->target_width / 64) << 16) | ((u64)texture->target_psm << 24),
                  ((u64)(texture->target_width - 1) << 16) | ((u64)(texture->target_height - 1) << 48),
                  ((u64)(0x800 - (texture->target_width >> 1)) << 4) | ((u64)(0x800 - (texture->target_height >> 1)) << 36));
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, (u64)texture->alpha_a | ((u64)texture->alpha_b << 2) | ((u64)texture->alpha_c << 4) |
                                      ((u64)texture->alpha_d << 6) | ((u64)texture->alpha_fix << 32));
    Packet_AddGsRegister(GS_REG_TEST_1, 0x53001);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
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
    GsDraw_QueueTexturedSprite(&sprite);
    GsDraw_QueueFlush();
}

/* Copy the frame being drawn (the buffer GsDraw_ResetDrawTarget draws to) into a width x height texture at `fbp`. */
void GsDraw_CopyFrameToTexture(int fbp, int psm, int width, int height) {
    ScreenSprite sprite;
    int frame_base;

    frame_base = 0x1180;
    if (D_00446F78 != 0) {
        frame_base = 0;
    }
    GsDraw_SetDrawTarget((fbp >> 5) | ((u64)(width / 64) << 16) | ((u64)psm << 24),
                  ((u64)(width - 1) << 16) | ((u64)(height - 1) << 48),
                  ((0x800 - (width >> 1)) << 4) | ((u64)((0x800 - (height >> 1)) << 4) << 32));
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x80000000A8L);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_AddGsRegister(8, 5);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    sprite.tex0 = frame_base | 0x28028000L | (0xC800L << 19);
    sprite.tex1 = 0x60;
    sprite.prim = 0x156;
    sprite.rgba[3] = sprite.rgba[2] = sprite.rgba[1] = sprite.rgba[0] = 0x80;
    sprite.vertex[0].x = (0x800 - width / 2) << 4;
    sprite.vertex[0].y = (0x800 - height / 2) << 4;
    sprite.vertex[1].x = ((width / 2) << 4) + 0x7FF8;
    sprite.vertex[1].y = ((height / 2) << 4) + 0x7FF8;
    sprite.vertex[1].z = sprite.vertex[0].z = 0xFFFFFFFF;
    sprite.vertex[1].tex_s = 0x2800;
    sprite.vertex[1].tex_t = 0x1C00;
    sprite.vertex[0].tex_s = 0;
    sprite.vertex[0].tex_t = 0;
    GsDraw_QueueTexturedSprite(&sprite);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(8, 0);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
}
