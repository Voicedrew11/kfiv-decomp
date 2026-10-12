#include "common.h"
#include "kf_model.h"
#include "kf_packet.h"
#include "kf_screen.h"

double sin(double);
void *memset(void *, int, u32);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00216B48(void *, void *, void *);
void func_002168B8(int *, int *, int *, f32 *, f32 *, f32 *);
void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);
float sinf(float);
int rand(void);
void srand(u32);

extern const float D_00444BDC;
extern const float D_00444BE0;
extern const float D_00444BE4;
extern f32 D_004323E0[16];

/* the screen wobble: 16 oscillators and a table of sines they are drawn from */
static float wobble_base_phase;                 /* 0x447058 advanced each frame by func_001112B0 */
static WaveOscillator wobble_oscillators[16];   /* 0x471DE0 */
static float wobble_sines[32];                  /* 0x471EE0 filled by func_001112B0 */

void func_001112B0(void) {
    float x;
    int i;

    wobble_base_phase += D_00444BDC;
    if (wobble_base_phase > D_00444BE0) {
        wobble_base_phase -= D_00444BE0;
    }
    x = wobble_base_phase;
    for (i = 0; i < 32; i++) {
        wobble_sines[i] = sin(x);
        x += D_00444BE4;
        if (x > D_00444BE0) {
            x -= D_00444BE0;
        }
    }
}

/* moves a point up or down by a table entry (wobble_sines, the sines func_001112B0 fills) picked by the
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
        (point)[1] += wobble_sines[index] * 5.0f;                                                    \
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

/* Queues a 12-quadword packet: a three-vertex primitive whose vertices all have 0x8000 at
   offset 0x0E, with the rest left for the caller to fill in. */
void func_00111BE0(void) {
    u32 *packet_start;
    u32 *packet;
    u32 *after_vertices;
    u16 *vertices;

    packet_start = packet = func_00109990(0xC);
    *packet = 0x1000000A;
    packet += 7;
    *packet++ = 0x6C01C000;
    *(u64 *)packet = 0x3000000000008003;
    packet += 2;
    *packet++ = 0x412;
    *packet = 0;
    packet += 4;
    *packet++ = 0x6D0C8001;
    vertices = (u16 *)packet;
    after_vertices = packet + 0x18;
    vertices[0x27] = vertices[0x17] = vertices[7] = 0x8000;
    packet = after_vertices;
    packet[3] = 0x15000000;
    packet += 4;
    func_00109650(packet_start, packet);
}

/* Whether any data block of the model draws with a texture numbered from first to last. */
int func_00111C78(ModelInstance *model, int first, int last) {
    ModelPart *part;
    ModelPartData *data;
    int i;
    int j;
    int count;
    int part_count;
    int texture;
    u16 length;

    part_count = model->part_count;
    part = model->parts;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            texture = data->unk40 & 0x3FFF;
            length = data->length;
            if (texture >= first && texture <= last) {
                return TRUE;
            }
            data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}

/* Resets the 16 screen wobble oscillators, spreading their phases over one turn. */
void func_00111D00(void) {
    WaveOscillator *oscillator = wobble_oscillators;
    int i;

    for (i = 0; i < 16; i++) {
        oscillator->amp_x = 0.01f;
        oscillator->amp_y = 0.0f;
        oscillator->phase = (float)i * 6.2831855f * 0.0625f;
        oscillator++;
    }
}

/* Advances each of the 16 entries' phase and recomputes its sine offsets. */
void func_00111D58(void) {
    WaveOscillator *entry = (WaveOscillator *)wobble_oscillators;
    float step = 0.06981317f;
    float half_turn = 3.1415927f;
    float two_pi = 6.2831855f;
    int i;

    for (i = 0; i < 16; i++) {
        entry->phase += step;
        if (half_turn <= entry->phase) {
            entry->phase -= two_pi;
        }
        entry->x = entry->amp_x * sinf(entry->phase) * 4096.0f;
        entry->y = entry->amp_y * sinf(entry->phase) * 4096.0f;
        entry++;
    }
}

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;
    u32 aligned = address;

    if (remainder != 0) {
        aligned = remainder - 0x10;
        aligned = address - aligned;
    }
    return aligned;
}

/* Builds the model's table of wave texture coordinates at *alloc (16-byte aligned) and moves *alloc
   past it. Each vertex keeps its own coordinates and gets a wave picked from where it is. */
void func_00111E28(ModelInstance *model, u32 *alloc) {
    ModelPart *part;
    ModelPartData *next_data;
    ModelPartData *data;
    ModelPartVertex *vertex;
    ModelWaveUv *wave_uv;
    int i;
    int j;
    int k;
    int count;
    int part_count;
    int vertex_count;
    u16 length;
    u32 address;
    u32 remainder;

    address = align_16(*alloc);
    model->wave_uvs = (ModelWaveUv *)address;
    part = model->parts;
    part_count = model->part_count;
    wave_uv = (ModelWaveUv *)address;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            data = next_data;
            next_data = (ModelPartData *)next_data->vertices; // match: the cursor steps on to the vertices
            vertex = (ModelPartVertex *)next_data;
            vertex_count = data->unk70 & 0x7FFF;
            length = data->length;
            for (k = 0; k < vertex_count; k++) {
                wave_uv->tex_u = vertex->tex_u;
                wave_uv->tex_v = vertex->tex_v;
                srand((vertex->x + (int)model->position.x) + ((int)model->position.y + vertex->y) +
                      ((int)model->position.z + vertex->z));
                wave_uv->wave = rand() % 16;
                wave_uv++;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
    address = (u32)wave_uv;
    remainder = address & 0xF;
    if (remainder != 0) {
        remainder -= 0x10;
        address -= remainder;
    }
    *alloc = address;
}

/* Moves every vertex's texture coordinates by its wave. */
void func_00112018(ModelInstance *model) {
    ModelPart *part;
    ModelPartData *next_data;
    ModelPartData *data;
    ModelPartVertex *vertex;
    ModelWaveUv *wave_uv;
    int i;
    int j;
    int k;
    int count;
    int part_count;
    int vertex_count;
    u16 length;

    part = model->parts;
    part_count = model->part_count;
    wave_uv = model->wave_uvs;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            data = next_data;
            next_data = (ModelPartData *)next_data->vertices; // match: the cursor steps on to the vertices
            vertex = (ModelPartVertex *)next_data;
            vertex_count = data->unk70 & 0x7FFF;
            length = data->length;
            for (k = 0; k < vertex_count; k++) {
                vertex->tex_u = wave_uv->tex_u + (u16)wobble_oscillators[wave_uv->wave].x;
                vertex->tex_v = wave_uv->tex_v + (u16)wobble_oscillators[wave_uv->wave].y;
                wave_uv++;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
