#include "eeregs.h"
#include "kf_game.h"
#include "kf_model.h"
#include "kf_screen.h"

extern f32 D_00428BD0[16];
extern f32 D_00428C10[16];
extern f32 D_0042B7D0[16];
extern f32 (*D_00446D40)[4];

void *memset(void *, int, u32);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0MulMatrix(void *, void *, void *);
int FlushCache(int);
u32 *func_001FBE70(void);

/* Hands a packet in the scratchpad to the GIF once it is idle. */
static inline void send_packet(u32 *packet, s32 size) {
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    *D2_QWC = size;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x105;
}

/* Draws a part in one flat colour (the four bytes of the third argument): turns its points into screen
   points, then sends each strip to the GS as it stands, its registers first, with the colour on every
   point, and, when the strip has a second texture (unk10), the same strip again with that texture and the
   points' normals as texture coordinates. Last a packet that sets ALPHA_1. */
void func_002008E8(PoolPart *part, s32 parent, RenderSlotArg color) {
    PoolDrawScratch *scratch = POOL_DRAW_SCRATCH;
    PoolDrawScratch *again;
    u32 *out;
    f32 unit[16];
    s32 source;
    s32 target;

    memset(unit, 0, 0x40);
    unit[0] = 1.0f;
    unit[5] = 1.0f;
    unit[10] = 1.0f;
    unit[15] = 1.0f;
    scratch->unkA0[0] = 128.0f;
    scratch->unkA0[1] = 128.0f;
    scratch->unkA0[2] = 128.0f;
    scratch->unkA0[3] = 128.0f;
    scratch->work = D_00446D40;
    scratch->screen_points = D_00446D40 + 0x500;
    scratch->normals = D_00446D40 + 0xA00;
    *(Uint128 *)scratch->unk1A0 = 0x00000080000000000000000000000000;
    scratch->point_count = part->point_count;
    scratch->strip_count = part->strip_count;
    scratch->normal_count = part->normal_count;
    scratch->point_index = 0;
    scratch->normal_index = 0;
    sceVu0RotMatrixX(scratch->world, unit, part->rotation[0]);
    sceVu0RotMatrixZ(scratch->world, scratch->world, part->rotation[2]);
    sceVu0RotMatrixY(scratch->world, scratch->world, part->rotation[1]);
    sceVu0TransMatrix(scratch->world, scratch->world, part->position);
    sceVu0MulMatrix(scratch->world, (void *)parent, scratch->world);
    sceVu0MulMatrix(scratch->screen, D_0042B7D0, scratch->world);
    sceVu0MulMatrix(scratch->light, scratch->world, D_00428C10);
    __asm__ volatile("lqc2 $vf12, 0x0(%0)\n"
                     "lqc2 $vf13, 0x10(%0)\n"
                     "lqc2 $vf14, 0x20(%0)\n"
                     "lqc2 $vf15, 0x30(%0)\n"
                     : : "r"(scratch->world));
    __asm__ volatile("lqc2 $vf1, 0x0(%0)\n"
                     "lqc2 $vf2, 0x10(%0)\n"
                     "lqc2 $vf3, 0x20(%0)\n"
                     "lqc2 $vf4, 0x30(%0)\n"
                     : : "r"(scratch->screen));
    scratch->point_index = 0;
    scratch->normal_index = 0;
    __asm__ volatile("lqc2 $vf5, 0x0(%0)\n"
                     "lqc2 $vf6, 0x10(%0)\n"
                     "lqc2 $vf7, 0x20(%0)\n"
                     : : "r"(scratch->light));
    __asm__ volatile("lqc2 $vf8, 0x0(%0)\n"
                     "lqc2 $vf9, 0x10(%0)\n"
                     "lqc2 $vf10, 0x20(%0)\n"
                     "lqc2 $vf11, 0x30(%0)\n"
                     : : "r"(D_00428BD0));
    __asm__ volatile("vaddw.xyzw $vf20, $vf0, $vf0w\n"
                     "vdiv Q, $vf0w, $vf20w\n"
                     "vmul.xyzw $vf21, $vf0, $vf0\n"
                     "vwaitq\n"
                     "vmulq.w $vf21, $vf0, Q\n");
    for (scratch->point_at = 0; scratch->point_at < scratch->point_count; scratch->point_at++) {
        source = scratch->point_index * 16 + (s32)part->points;
        target = scratch->point_index++ * 16 + (s32)scratch->screen_points;
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf1, $vf16x\n"
                         "vmadday.xyzw ACC, $vf2, $vf16y\n"
                         "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
                         "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vdiv Q, $vf0w, $vf17w\n"
                         "vwaitq\n"
                         "vmulq.xyzw $vf26, $vf17, Q\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "vftoi4.xyzw $vf17, $vf26\n"
                         "vnop\n"
                         "vnop\n"
                         "vnop\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(target), "r"(source));
    }
    for (scratch->normal = 0; scratch->normal < scratch->normal_count; scratch->normal++) {
        __asm__ volatile("lqc2 $vf16, 0x0(%1)\n"
                         "vmulax.xyzw ACC, $vf12, $vf16x\n"
                         "vmadday.xyzw ACC, $vf13, $vf16y\n"
                         "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
                         "vaddw.xy $vf17, $vf17, $vf0w\n"
                         "vmulw.xy $vf17, $vf17, $vf21w\n"
                         "vaddw.z $vf17, $vf0, $vf0w\n"
                         "sqc2 $vf17, 0x0(%0)\n"
                         : : "r"(scratch->normal_index * 16 + (s32)scratch->normals), "r"(scratch->normal_index * 16 + (s32)part->normals));
        scratch->normal_index++;
    }
    FlushCache(0);
    scratch->prim_tag[2] = 0x412;
    scratch->reg_tag[0] = 0x8008;
    scratch->reg_tag[1] = 0x10004000;
    scratch->reg_tag[2] = 0xE;
    scratch->prim_tag[3] = 0;
    scratch->strip_at = part->material;
    scratch->reg_tag[3] = 0;
    scratch->tex0 = scratch->strip_at->tex0; // match: every strip is sent with the first strip's TEX0
    for (scratch->strip = 0; scratch->strip < scratch->strip_count; scratch->strip++) {
        scratch->cursor = func_001FBE70();
        scratch->packet = scratch->cursor;
        *scratch->cursor++ = scratch->reg_tag[0];
        *scratch->cursor++ = scratch->reg_tag[1];
        *scratch->cursor++ = scratch->reg_tag[2];
        *scratch->cursor++ = scratch->reg_tag[3];
        scratch->registers = (u64 *)scratch->cursor;
        scratch->strip_point = scratch->strip_at->points;
        scratch->strip_start = scratch->strip_at;
        ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: written to the strip's own TEX0, which is never sent
        scratch->registers[0] = 0;
        scratch->registers[1] = 0;
        scratch->registers[4] = scratch->tex0;
        scratch->registers[5] = 7;
        scratch->registers[2] = 0x60;
        scratch->registers[3] = 0x15;
        scratch->registers[6] = 0;
        scratch->registers[7] = 9;
        scratch->cursor += 0x10;
        *((u64 *)scratch->cursor)++ = 4;
        *((u64 *)scratch->cursor)++ = 0x1C;
        *((u64 *)scratch->cursor)++ = scratch->tex0;
        *((u64 *)scratch->cursor)++ = 0x17;
        *scratch->cursor++ = scratch->strip_at->unk04;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x43;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x50000;
        *scratch->cursor++ = 0;
        *scratch->cursor++ = 0x48;
        *scratch->cursor++ = 0;
        scratch->prim_tag[0] = scratch->strip_at->point_count | 0x8000;
        scratch->prim_tag[1] = 0x31004000;
        scratch->prim_tag[1] |= (scratch->strip_at->prim & 0x7FF) << 15;
        *scratch->cursor++ = scratch->prim_tag[0];
        *scratch->cursor++ = scratch->prim_tag[1];
        *scratch->cursor++ = scratch->prim_tag[2];
        *scratch->cursor++ = scratch->prim_tag[3];
        for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
            ((f32 *)scratch->cursor)[0] = scratch->strip_point->tex_s;
            ((f32 *)scratch->cursor)[1] = scratch->strip_point->tex_t;
            ((f32 *)scratch->cursor)[2] = 1.0f;
            POOL_DRAW_SCRATCH_CURSOR(scratch).words += 4; // match: through the union this store waits for the float stores, which decides the loop's registers
            scratch->cursor[0] = ((u8 *)&color)[0];
            again = POOL_DRAW_SCRATCH; // match: a second pointer the compiler cannot fold: its reload of the cursor is the daddu copy
            out = again->cursor;
            out[1] = ((u8 *)&color)[1];
            out[2] = ((u8 *)&color)[2];
            out[3] = ((u8 *)&color)[3];
            __asm__ volatile("lq $9, 0x0(%0)\n"
                             "sq $9, 0x0(%1)\n"
                             : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(out + 4) : "$9");
            scratch->cursor = out + 8;
            scratch->strip_point++;
        }
        if (scratch->strip_at->unk10 != 0) {
            scratch->strip_point = scratch->strip_at->points;
            *scratch->cursor++ = scratch->reg_tag[0];
            *scratch->cursor++ = scratch->reg_tag[1];
            *scratch->cursor++ = scratch->reg_tag[2];
            *scratch->cursor++ = scratch->reg_tag[3];
            scratch->registers = (u64 *)scratch->cursor;
            scratch->strip_start = scratch->strip_at;
            ((PoolPartMaterialBits *)scratch->strip_start)->cld = 4; // match: as above
            scratch->registers[0] = 0;
            scratch->registers[1] = 0;
            scratch->registers[4] = scratch->strip_at->unk10 | 0x800000000;
            scratch->registers[5] = 7;
            scratch->registers[2] = 0x60;
            scratch->registers[3] = 0x15;
            scratch->registers[6] = 0;
            scratch->registers[7] = 9;
            scratch->cursor += 0x10;
            *((u64 *)scratch->cursor)++ = 4;
            *((u64 *)scratch->cursor)++ = 0x1C;
            *((u64 *)scratch->cursor)++ = scratch->strip_at->unk10;
            *((u64 *)scratch->cursor)++ = 0x17;
            *((u64 *)scratch->cursor)++ = 9;
            *scratch->cursor++ = 0x43;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x50000;
            *scratch->cursor++ = 0;
            *scratch->cursor++ = 0x48;
            *scratch->cursor++ = 0;
            scratch->prim_tag[0] = scratch->strip_at->point_count | 0x8000;
            scratch->prim_tag[1] = 0x31004000;
            scratch->prim_tag[1] |= (scratch->strip_at->prim & 0x7FF) << 15;
            scratch->prim_tag[1] |= 0x200000;
            scratch->prim_tag[1] |= 0x80000;
            *scratch->cursor++ = scratch->prim_tag[0];
            *scratch->cursor++ = scratch->prim_tag[1];
            *scratch->cursor++ = scratch->prim_tag[2];
            *scratch->cursor++ = scratch->prim_tag[3];
            for (scratch->point = 0; scratch->point < scratch->strip_at->point_count; scratch->point++) {
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "sq $9, 0x0(%1)\n"
                                 : : "r"(&scratch->normals[scratch->strip_point->normal]), "r"(scratch->cursor) : "$9");
                __asm__ volatile("lq $9, 0x0(%0)\n"
                                 "lq $8, 0x0(%2)\n"
                                 "sq $9, 0x0(%1)\n"
                                 "sq $8, 0x0(%3)\n"
                                 : : "r"(&scratch->screen_points[scratch->strip_point->point]), "r"(scratch->cursor + 8), "r"(scratch->unk1A0), "r"(scratch->cursor + 4) : "$8", "$9");
                scratch->cursor += 12;
                scratch->strip_point++;
            }
        }
        scratch->strip_at = (PoolPartMaterial *)scratch->strip_point;
        scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
        if (scratch->packet_size < 3) { // match: never true, the packet is 9 quadwords before its points
            return;
        }
        send_packet(scratch->packet, scratch->packet_size);
    }
    scratch->cursor = func_001FBE70();
    scratch->packet = scratch->cursor;
    *scratch->cursor++ = 0x8001;
    *scratch->cursor++ = 0x10004000;
    *scratch->cursor++ = 0xE;
    *scratch->cursor++ = 0;
    *((u64 *)scratch->cursor)++ = 0x44;
    *scratch->cursor++ = 0x42;
    *scratch->cursor++ = 0;
    scratch->packet_size = ((s32)scratch->cursor - (s32)scratch->packet) >> 4;
    send_packet(scratch->packet, scratch->packet_size);
}
