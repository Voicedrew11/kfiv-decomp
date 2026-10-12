#include "kf_model.h"

typedef u32 u_long128 __attribute__((mode(TI)));

extern int D_00446764;          /* triangles cut up this way */

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00110D60(BlendVertex *, BlendVertex *, BlendVertex *, int);

/* the scratchpad as this function uses it: the edge opposite the apex, then two rows of the fan */
#define SCRATCH_EDGE ((BlendVertex *)0x70000200)
#define SCRATCH_ROW_A ((BlendVertex *)0x70000340)
#define SCRATCH_ROW_B ((BlendVertex *)0x70000480)

/* copies a quadword and steps both pointers past it */
#define COPY_QWORD(to, from)                                                                       \
    *(u_long128 *)(to) = *(u_long128 *)(from);                                                     \
    (from) += 4;                                                                                   \
    (to) += 4

/* turns a packed vertex into one of floats */
#define TO_BLEND_VERTEX(to, from)                                                                  \
    (to).unk00[0] = (from)->x;                                                                     \
    (to).unk00[1] = (from)->y;                                                                     \
    (to).unk00[2] = (from)->z;                                                                     \
    (to).unk00[3] = 1.0f;                                                                          \
    (to).unk10[0] = (from)->unk08[0] * 4096.0f;                                                    \
    (to).unk10[1] = (from)->unk08[1] * 4096.0f;                                                    \
    (to).unk10[2] = (from)->unk08[2] * 4096.0f;                                                    \
    (to).unk10[3] = 0.0f;                                                                          \
    (to).unk20[0] = (from)->u * 4096.0f;                                                           \
    (to).unk20[1] = (from)->v * 4096.0f;                                                           \
    (to).unk20[2] = 1.0f;                                                                          \
    (to).unk20[3] = 0.0f;                                                                          \
    (to).unk30[0] = (from)->colour[0];                                                             \
    (to).unk30[1] = (from)->colour[1];                                                             \
    (to).unk30[2] = (from)->colour[2];                                                             \
    (to).unk30[3] = (from)->colour[3]

/* Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones and
   with its vertices as floats (func_0010F478 does the same with packed vertices): the edge between
   its last two vertices is cut in four, and so is each line from the first vertex (the apex) to a
   point of that edge; each pair of neighbouring lines goes out as a strip. `data` is the block (a
   ModelPartData) as words. `first` sends the matrix along, `setup` the block's GS setup. */
void func_00110050(u32 *data, ModelPartVertexWords *vertex, int first, int setup, void *matrix) {
    BlendVertex apex_point;
    BlendVertex before_point;
    BlendVertex vertex_point;
    u16 steps = 4;
    u16 vertex_count = steps * (steps * 2 + 1);
    int size;
    u32 *packet_start;
    ModelPartVertexWords *before = vertex - 1;
    ModelPartVertexWords *apex = vertex - 2;
    BlendVertex *edge_point;
    u32 *packet;
    BlendVertex *previous_point;
    BlendVertex *out;
    BlendVertex *row = SCRATCH_ROW_A;
    BlendVertex *previous = SCRATCH_ROW_B;
    BlendVertex *row_point;
    int i;
    int k;

    size = 1;
    if (setup) {
        size = 8;
    }
    size += vertex_count * 4 + 5;
    TO_BLEND_VERTEX(apex_point, apex);
    TO_BLEND_VERTEX(before_point, before);
    TO_BLEND_VERTEX(vertex_point, vertex);
    packet_start = packet = func_00109990(first ? size + 10 : size);
    if (first) {
        *packet = 0x10000005;
        packet += 7;
        *packet++ = 0x6C040004;
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
        *packet = 0x10000003;
        packet += 4;
        *packet++ = 0x6C020064;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 0.0f;
        *(f32 *)packet++ = 128.0f;
        *(f32 *)packet++ = 0.0f;
        *(f32 *)packet++ = 128.0f;
    }
    *packet = (size - 2) | 0x10000000;
    packet += 4;
    data += 4;
    if (setup) {
        *(u_long128 *)packet = 0x6C06C0A0;
        packet += 4;
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        *(u_long128 *)packet = 0;
        packet += 4;
        *(u_long128 *)packet = 0;
        packet += 4;
    } else {
        data += 20;
    }
    packet += 3;
    *packet++ = 0x6C01C000;
    data += 5;
    *packet++ = 0x8024;
    *packet++ = *data++;
    *packet++ = *data++;
    *packet = *data;
    packet += 4;
    *packet++ = (vertex_count << 18) | 0x6C008001;
    func_00110D60(SCRATCH_EDGE, &before_point, &vertex_point, steps);
    edge_point = SCRATCH_EDGE + 1;
    func_00110D60(SCRATCH_ROW_B, &apex_point, SCRATCH_EDGE, steps);
    out = (BlendVertex *)packet;
    for (i = 1; i <= steps; i++) {
        row_point = row; // match: one variable swaps the rows and then walks the older one
        row = previous;
        previous = row_point;
        func_00110D60(row_point, &apex_point, edge_point++, steps);
        *out = apex_point;
        *(s32 *)&out->unk10[3] = 0x8000;
        out++;
        *out = row[1];
        *(s32 *)&out->unk10[3] = 0x8000;
        out++;
        row_point = row + 2;
        *out++ = previous[1];
        previous_point = previous + 2;
        for (k = 1; k < steps; k++) {
            *out++ = *row_point++;
            *out++ = *previous_point++;
        }
    }
    out = (BlendVertex *)((u32 *)out + 3); // match: stepped in place, then handed back to packet
    packet = (u32 *)out;
    if (first) {
        *packet++ = 0x15000280;
    } else {
        *packet++ = 0x17000000;
    }
    func_00109650(packet_start, packet);
    D_00446764++;
}
