#include "kf_model.h"
#include "kf_packet.h"

extern f32 D_004323E0[16];
extern float D_00471EE0[];

void *memset(void *, int, u32);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00216B48(void *, void *, void *);
void func_002168B8(int *, int *, int *, f32 *, f32 *, f32 *);
void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);

/* moves a point up or down by a table entry (D_00471EE0, the sines func_001112B0 fills) picked by the
   vertex's x and z */
#define WOBBLE(point, vertex)                                                                      \
    {                                                                                              \
        int index = (vertex).x + (vertex).z;                                                       \
                                                                                                   \
        index = (index * index * index) % 32;                                                      \
                                                                                                   \
        if (index < 0) {                                                                           \
            index = -index;                                                                        \
        }                                                                                          \
        (point)[1] += D_00471EE0[index] * 5.0f;                                                    \
    }

/* Draws a model with its own packets: every vertex where it is, with the texture coordinates of the
   screen position it would have if it were moved up or down by a sine picked by its x and z (so a
   texture that holds the screen comes out rippled). The four values go into the packet's last GS
   register write. */
void func_00111368(ModelInstance *model, int unk1, int unk2, int unk3, int unk4) {
    f32 points[3][4];
    int screens[3][4];
    f32 matrix[16];
    int part_count;
    int data_count;
    ModelPartData *data;
    ModelPartData *next;
    u32 *packet_start;
    u32 *packet;
    ModelPart *part;
    ModelPartVertex *vertex;
    PacketStVertex *out;
    int *screen;
    int vertex_count;
    u16 length;
    int i;
    int j;
    int k;
    int corner;
    int flags;
    u64 prim = 0x54; // match: a variable, so that its shift stays out of the constant
    u64 tag_low;
    u64 tag_high;

    memset(points, 0, sizeof(points));
    points[2][3] = points[1][3] = points[0][3] = 1.0f;
    if (model->parts != NULL && model->part_count != 0) {
        packet = func_00109990(9);
        packet_start = packet; // match: copied from packet, or the stores are addressed from the start
        part_count = model->part_count;
        *packet = 0x10000008;
        packet += 7;
        *packet = 0x13000000;
        packet += 4;
        *packet++ = 0x51000006;
        *(u64 *)packet = 0x5000000000008001;
        packet += 2;
        *(u64 *)packet = 0x1EEEE;
        packet += 2;
        *(u64 *)packet = 0x220A12BC0;
        packet += 2;
        *(u64 *)packet = 6;
        packet += 2;
        *(u64 *)packet = 0x60;
        packet += 2;
        *(u64 *)packet = 0x14;
        packet += 2;
        *(u64 *)packet = 0x44;
        packet += 2;
        *(u64 *)packet = 0x42;
        packet += 2;
        *(u64 *)packet = 5;
        packet += 2;
        *(u64 *)packet = 8;
        packet += 2;
        *packet++ = unk1;
        *packet++ = unk2;
        packet[0] = unk3;
        packet[1] = unk4;
        part = model->parts;
        for (i = 0; i < part_count; i++, part++) {
            func_00216B48((int)matrix, (int *)model->unk40, (int)part);
            func_00216B48((int)matrix, (int *)D_004323E0, (int)matrix);
            KF_Vu0_LoadMatrix(matrix);
            data_count = part->data_count;
            next = (ModelPartData *)(int)part->data_addr;
            for (j = 0; j < data_count; j++) {
                flags = 0;
                length = next->length;
                data = next;
                vertex_count = next->unk70 & 0x7FFF;
                next = (ModelPartData *)next->vertices; // match: the cursor steps on; data keeps the block's start
                vertex = (ModelPartVertex *)next;
                packet = func_00109990(vertex_count * 2 + 3);
                *packet = (vertex_count * 2 + 2) | 0x10000000;
                packet += 7;
                *packet++ = (vertex_count * 2) | 0x51000001;
                tag_low = (u64)vertex_count | 0x8000 | ((u64)2 << 60);
                tag_high = (prim << 47) | ((u64)1 << 46);
                *(u64 *)packet = tag_low | tag_high;
                packet += 2;
                *(u64 *)packet = 0x52;
                out = (PacketStVertex *)(packet + 2);
                for (k = vertex_count / 3 - 1; k != -1; k--) {
                    points[0][0] = vertex[0].x;
                    points[0][1] = vertex[0].y;
                    points[0][2] = vertex[0].z;
                    points[1][0] = vertex[1].x;
                    points[1][1] = vertex[1].y;
                    points[1][2] = vertex[1].z;
                    points[2][0] = vertex[2].x;
                    points[2][1] = vertex[2].y;
                    points[2][2] = vertex[2].z;
                    func_002168B8(&out[0].x, &out[1].x, &out[2].x, points[0], points[1], points[2]);
                    out[0].z >>= 4;
                    out[1].z >>= 4;
                    out[2].z >>= 4;
                    WOBBLE(points[0], vertex[0]);
                    WOBBLE(points[1], vertex[1]);
                    WOBBLE(points[2], vertex[2]);
                    func_002168B8(screens[0], screens[1], screens[2], points[0], points[1], points[2]);
                    screen = screens[0];
                    for (corner = 0; corner < 3; corner++) {
                        if ((u32)out->z > 0xFFFF0 || ((out->x | out->y) & 0xFFFF0000)) {
                            flags |= 1;
                            out->flags |= 0x8000;
                        }
                        if (flags != 0) {
                            out->flags |= 0x8000;
                        }
                        flags = (flags << 1) & 7;
                        out->flags |= vertex->unk0E;
                        vertex++;
                        out->tex_s = (f32)(screen[0] - 0x6C00) / 10240.0f;
                        out->tex_t = (f32)(screen[1] - 0x7200) / 7168.0f;
                        out->tex_q = 1.0f;
                        screen += 4;
                        out++;
                    }
                }
                for (k = vertex_count % 3 - 1; k != -1; k--) {
                    points[0][0] = vertex->x;
                    points[0][1] = vertex->y;
                    points[0][2] = vertex->z;
                    KF_Vu0_ProjectToScreen(&out->x, points[0]);
                    out->z >>= 4;
                    WOBBLE(points[0], vertex[0]);
                    KF_Vu0_ProjectToScreen(screens[0], points[0]);
                    if ((u32)out->z > 0xFFFF0 || ((out->x | out->y) & 0xFFFF0000)) {
                        flags |= 1;
                        out->flags |= 0x8000;
                    }
                    if (flags != 0) {
                        out->flags |= 0x8000;
                    }
                    flags = (flags << 1) & 7;
                    out->flags |= vertex->unk0E;
                    vertex++;
                    out->tex_q = 1.0f;
                    out->tex_s = (f32)(screens[0][0] - 0x6C00) / 10240.0f;
                    out->tex_t = (f32)(screens[0][1] - 0x7200) / 7168.0f;
                    out++;
                }
                next = (ModelPartData *)((char *)data + (length + 1) * 0x10);
            }
        }
        packet = func_00109990(4);
        *packet = 0x10000003;
        packet += 7;
        *packet++ = 0x51000002;
        *(u64 *)packet = 0x1000000000008001;
        packet += 2;
        *(u64 *)packet = 0xE;
        packet += 2;
        ((u64 *)packet)[1] = 8;
        ((u64 *)packet)[0] = 0;
        packet = func_00109990(1);
        func_00109650(packet_start, packet);
    }
}
