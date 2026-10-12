#include "kf_model.h"

typedef u32 u_long128 __attribute__((mode(TI)));

extern int D_00446764;          /* triangles cut up this way */

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_0010F898(ModelPartVertexWords *, ModelPartVertexWords *, ModelPartVertexWords *, int);

/* the scratchpad as this function uses it: the edge opposite the apex, then two rows of the fan */
#define SCRATCH_EDGE ((ModelPartVertexWords *)0x70000200)
#define SCRATCH_ROW_A ((ModelPartVertexWords *)0x700002A0)
#define SCRATCH_ROW_B ((ModelPartVertexWords *)0x70000340)

/* copies a quadword and steps both pointers past it */
#define COPY_QWORD(to, from)                                                                       \
    *(u_long128 *)(to) = *(u_long128 *)(from);                                                               \
    (from) += 4;                                                                                   \
    (to) += 4

/* Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones:
   the edge between its last two vertices is cut in four, and so is each line from the first
   vertex (the apex) to a point of that edge; each pair of neighbouring lines goes out as a strip.
   `data` is the block (a ModelPartData) as words. `first` sends the matrix along, `setup` the
   block's GS setup. */
void func_0010F478(u32 *data, ModelPartVertexWords *vertex, int first, int setup, void *matrix) {
    u16 steps = 4;
    u16 vertex_count = steps * (steps * 2 + 1);
    int size;
    u32 *packet_start;
    ModelPartVertexWords *before = vertex - 1;
    ModelPartVertexWords *apex = vertex - 2;
    ModelPartVertexWords *edge_point;
    u32 *packet;
    ModelPartVertexWords *previous_point;
    ModelPartVertexWords *out;
    ModelPartVertexWords *row = SCRATCH_ROW_A;
    ModelPartVertexWords *previous = SCRATCH_ROW_B;
    ModelPartVertexWords *row_point;
    int i;
    int k;

    size = 1;
    if (setup) {
        size = 6;
    }
    size += vertex_count * 2 + 5;
    packet_start = packet = func_00109990(first ? size + 6 : size);
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
    }
    *packet = (size - 2) | 0x10000000;
    packet += 4;
    data += 4;
    if (setup) {
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
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
    *packet++ = (vertex_count << 18) | 0x6D008001;
    func_0010F898(SCRATCH_EDGE, before, vertex, steps);
    edge_point = SCRATCH_EDGE + 1;
    func_0010F898(previous, apex, SCRATCH_EDGE, steps);
    out = (ModelPartVertexWords *)packet;
    for (i = 1; i <= steps; i++) {
        row_point = row; // match: one variable swaps the rows and then walks the older one
        row = previous;
        previous = row_point;
        func_0010F898(row_point, apex, edge_point++, steps);
        *out = *apex;
        out->unk0E = 0x8000;
        out++;
        *out = row[1];
        out->unk0E = 0x8000;
        out++;
        row_point = row + 2;
        *out++ = previous[1];
        previous_point = previous + 2;
        for (k = 1; k < steps; k++) {
            *out++ = *row_point++;
            *out++ = *previous_point++;
        }
    }
    out = (ModelPartVertexWords *)((u32 *)out + 3); // match: stepped in place, then handed back to packet
    packet = (u32 *)out;
    if (first) {
        *packet++ = 0x15000000;
    } else {
        *packet++ = 0x17000000;
    }
    func_00109650(packet_start, packet);
    D_00446764++;
}
