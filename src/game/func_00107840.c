#include "kf_entity.h"
#include "kf_gs.h"

typedef float Mat[4][4] __attribute__((aligned(16)));
typedef float Vec[4] __attribute__((aligned(16)));
typedef int IVec[4] __attribute__((aligned(16)));

typedef struct {
    s64 tag;              /* 0x00 GIF tag */
    s64 zero;             /* 0x08 */
    int reg;               /* 0x10 = 0x116 */
    u8 rgba[4]; /* 0x14 */
    s64 pad18;            /* 0x18 unused */
    u16 x0, y0; /* 0x20 */
    u32 z0;       /* 0x24 = -1 */
    int f0;                /* 0x28 */
    int g0;                /* 0x2C */
    u16 x1, y1; /* 0x30 */
    u32 z1;       /* 0x34 = -1 */
    int f1;                /* 0x38 */
    int g1;                /* 0x3C */
} KfPacket;

extern int D_00446F78;
extern float D_00420D00[];
extern Mat D_004323E0;
extern Mat D_00431AF0;
extern float D_00444BAC;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);
void func_00216778(float *, void *);
void KF_Vu0_LoadMatrix(Mat);
void KF_Vector_MultiplyComponents(float *, float *, float *);
void func_00216C48(float *, float *, float *);
void KF_Vector_Subtract(float *, float *, float *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vu0_ProjectToScreen(IVec, float *);
void sceVu0MulMatrix(Mat, Mat, Mat);
void sceVu0Normalize(float *, float *);
float sceVu0InnerProduct(float *, float *);
float sqrtf(float);

void func_00107840(void) {
    Mat view_matrix;
    Vec offset_to_entity;
    Vec scratch_vector;
    IVec screen_position;
    KfPacket sprite_packet;
    int tag_flags;
    Entity *entity;
    float focus_distance, nearest_distance_squared, distance_squared;
    int offset_x, offset_y, strip;
    int tag_high_bits;
    u64 tag_low_bits;

    tag_flags = 0x1180;
    if (D_00446F78 != 0) tag_flags = 0;
    focus_distance = D_00420D00[18] > 1000.0f ? 1000.0f : D_00420D00[18];
    nearest_distance_squared = focus_distance * focus_distance;
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (((int)(entity->flags >> 1) & 1) == 0) continue;
        if (focus_distance < entity->distance_to_player) continue;
        func_00216778(offset_to_entity, entity->model->position);
        offset_to_entity[1] -= entity->height * 0.5f;
        KF_Vector_Subtract(offset_to_entity, offset_to_entity, D_00420D00);
        KF_Vector_MultiplyComponents(scratch_vector, offset_to_entity, offset_to_entity);
        distance_squared = scratch_vector[0] + scratch_vector[1] + scratch_vector[2];
        if (nearest_distance_squared < distance_squared) continue;
        sceVu0Normalize(scratch_vector, offset_to_entity);
        if (sceVu0InnerProduct(scratch_vector, D_00420D00 + 4) < D_00444BAC) continue;
        nearest_distance_squared = distance_squared;
        focus_distance = sqrtf(distance_squared);
    }

    sceVu0MulMatrix(view_matrix, D_004323E0, D_00431AF0);
    KF_Vu0_LoadMatrix(view_matrix);
    KF_Vector_Scale(scratch_vector, D_00420D00 + 12, focus_distance);
    func_00216C48(scratch_vector, scratch_vector, D_00420D00 + 8);
    scratch_vector[3] = 1.0f;
    KF_Vu0_ProjectToScreen(screen_position, scratch_vector);

    offset_x = screen_position[0] - 0x8000;
    if (offset_x < 0) offset_x = screen_position[0] - 0x7FFF;
    offset_y = screen_position[1] - 0x8000;
    if (offset_y < 0) offset_y = screen_position[1] - 0x7FFF;
    offset_x = offset_x / 2;
    offset_y = offset_y / 2;
    if (offset_x > 0x80) offset_x = 0x80;
    else if (offset_x < -0x80) offset_x = -0x80;
    if (offset_y > 0x80) offset_y = 0x80;
    else if (offset_y < -0x80) offset_y = -0x80;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_SCANMSK, 3);
    func_00109AD0();

    sprite_packet.reg = 0x116;
    tag_low_bits = tag_flags | 0x28028000L;
    sprite_packet.zero = 0;
    sprite_packet.rgba[0] = 0x80;
    sprite_packet.rgba[1] = 0x80;
    sprite_packet.rgba[2] = 0x80;
    tag_high_bits = 0x29;
    sprite_packet.z0 = 0xFFFFFFFF;
    sprite_packet.tag = tag_low_bits | ((u64)tag_high_bits << 30);
    sprite_packet.rgba[3] = 0x80;
    sprite_packet.z1 = 0xFFFFFFFF;

    if (offset_x <= 0) {
        sprite_packet.x0 = 0x6BF8;
        sprite_packet.x1 = 0x6E00;
        sprite_packet.f1 = 0x200;
        sprite_packet.f0 = 0;
    } else {
        sprite_packet.x0 = 0x9400;
        sprite_packet.x1 = 0x91F8;
        sprite_packet.f0 = 0x2800;
        sprite_packet.f1 = 0x2600;
    }
    if (offset_y <= 0) {
        sprite_packet.y0 = 0x71F8;
        sprite_packet.y1 = 0x8E00;
        sprite_packet.g1 = 0x1C00;
        sprite_packet.g0 = 0;
    } else {
        sprite_packet.y0 = 0x8E00;
        sprite_packet.y1 = 0x71F8;
        sprite_packet.g0 = 0x1C00;
        sprite_packet.g1 = 0;
    }
    sprite_packet.x0 += offset_x;
    sprite_packet.x1 += offset_x;
    sprite_packet.y0 += offset_y;
    sprite_packet.y1 += offset_y;
    if (offset_x <= 0) offset_x = 0x200;
    else offset_x = -0x200;
    for (strip = 19; strip >= 0; strip--) {
        func_0010A100(&sprite_packet);
        sprite_packet.x0 += offset_x;
        sprite_packet.x1 += offset_x;
        sprite_packet.f0 += offset_x;
        sprite_packet.f1 += offset_x;
    }

    func_00109A50();
    func_00109A80(GS_REG_SCANMSK, 0);
    func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
    func_00109AD0();
}
