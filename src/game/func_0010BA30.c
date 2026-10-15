#include "kf_map.h"

typedef u32 u_long128 __attribute__((mode(TI)));

extern ModelLight D_003E7500[];
extern f32 D_00431AF0[16];

float asinf(float);
float atan2f(float, float);
float Angle_Wrap(float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
int ModelMatrix_BuildDrawMatrices(ModelInstance *, s32, s32, void *, void *); // match: an int return keeps $2 busy after the call
void Vector_Copy(void *, void *);
void Matrix_Multiply(void *, void *, void *);

/* the scratchpad as this function uses it: the turn it works out, the two matrices ModelMatrix_BuildDrawMatrices
   makes for the model, then a matrix per part and the same without the model's own placement */
#define SCRATCH_TURN ((f32 *)0x70000000)
#define SCRATCH_MATRIX_A ((void *)0x70000040)
#define SCRATCH_MATRIX_B ((void *)0x70000080)
#define SCRATCH_PART_MATRICES ((f32 (*)[16])0x700000C0)
#define SCRATCH_PART_TURNS ((f32 (*)[16])0x70001100)

/* copies a 4x4 matrix through VU0 registers */
#define COPY_MATRIX(to, from)                                                                      \
    __asm__ volatile(                                                                              \
        "lqc2 $vf16, 0x0(%0)\n"                                                                    \
        "lqc2 $vf17, 0x10(%0)\n"                                                                   \
        "lqc2 $vf18, 0x20(%0)\n"                                                                   \
        "lqc2 $vf19, 0x30(%0)\n"                                                                   \
        "sqc2 $vf16, 0x0(%1)\n"                                                                    \
        "sqc2 $vf17, 0x10(%1)\n"                                                                   \
        "sqc2 $vf18, 0x20(%1)\n"                                                                   \
        "sqc2 $vf19, 0x30(%1)\n"                                                                   \
        : : "r"(from), "r"(to))

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Draws a model (the draw function func_0010B758 installs): works out a turn from the block at
   unk148 and the model's rotation, queues a packet with the model's two matrices, its MapSlotUnk0C
   block, its lights and the vectors at unk160 and unk150, then for every part that is switched on
   and not scaled to nothing its two matrices, its parent's two and a reference to each of its data
   blocks. */
void func_0010BA30(ModelInstance *model) {
    void *matrix_a = SCRATCH_MATRIX_A;
    void *matrix_b = SCRATCH_MATRIX_B;
    f32 (*part_matrices)[16] = SCRATCH_PART_MATRICES;
    f32 (*part_turns)[16] = SCRATCH_PART_TURNS;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    s32 part_count;
    u32 word;
    f32 pitch;
    f32 yaw;

    i = 0;
    pitch = -asinf(((f32 *)model->unk148)[4]);
    yaw = Angle_Wrap(atan2f(((f32 *)model->unk148)[8], ((f32 *)model->unk148)[0]) + 1.5707964f);
    sceVu0RotMatrixY(SCRATCH_TURN, D_00431AF0, yaw);
    sceVu0RotMatrixX(SCRATCH_TURN, SCRATCH_TURN, pitch);
    sceVu0RotMatrixX(SCRATCH_TURN, SCRATCH_TURN, wrap_angle(model->rotation.x));
    sceVu0RotMatrixY(SCRATCH_TURN, SCRATCH_TURN, wrap_angle(model->rotation.y));
    sceVu0RotMatrixZ(SCRATCH_TURN, SCRATCH_TURN, wrap_angle(model->rotation.z));
    ModelMatrix_BuildDrawMatrices(model, model->unk148, model->unk148 + 0x40, SCRATCH_MATRIX_A, SCRATCH_MATRIX_B);
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        Matrix_Multiply(part_matrices[i], model->unk40, part);
        Matrix_Multiply(part_turns[i], SCRATCH_TURN, part);
    }
    packet_start = packet = func_00109990(0x20);
    *packet = 0x10000009;
    packet += 7;
    *packet++ = 0x6C08000C;
    COPY_MATRIX(packet, matrix_a);
    packet += 0x10;
    COPY_MATRIX(packet, matrix_b);
    packet += 0x10;
    *packet = 0x10000002;
    packet += 7;
    *packet++ = 0x6C01003C;
    *(Vector4 *)packet = *(Vector4 *)model->unk14C;
    packet += 4;
    *packet = 0x10000003;
    packet += 7;
    *packet++ = 0x6C02003D;
    *(u64 *)packet = 0x1000000000008001;
    packet += 2;
    *(u64 *)packet = 0xE;
    packet += 2;
    *(u64 *)packet = ((MapSlotUnk0CValues *)model->unk14C)->unk10;
    packet += 2;
    *packet++ = 0x3D;
    *packet++ = 0;
    *packet = 0x1000000A;
    packet += 7;
    *packet = 0x6C090040;
    packet += 4;
    *packet++ = model->hit_count;
    light_out = packet;
    light_out_b = packet + 0x10;
    for (i = 0; i < model->hit_count; i++) {
        *(u_long128 *)light_out = *(u_long128 *)D_003E7500[model->hit_handles[i]].unk00;
        *(u_long128 *)light_out_b = *(u_long128 *)D_003E7500[model->hit_handles[i]].unk10;
        light_out_b += 4;
        ((f32 *)light_out)[3] = D_003E7500[model->hit_handles[i]].unk24;
        light_out += 4;
    }
    packet += 0x20;
    *packet++ = 0x10000003;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C020064;
    Vector_Copy(packet, model->unk160);
    Vector_Copy(packet + 4, &model->unk150);
    part = model->parts;
    for (i = 0; i < model->part_count; i++, part++) {
        if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
            continue;
        }
        if (part->unk60[0] == 0.0f && part->unk60[1] == 0.0f && part->unk60[2] == 0.0f) {
            continue;
        }
        packet = func_00109990(2);
        packet[0] = 0x10000001;
        packet[7] = 0x13000000;
        packet = func_00109990(0xC);
        *packet = 0x10000005;
        packet += 7;
        *packet++ = 0x6C040004;
        COPY_MATRIX(packet, part_matrices[i]);
        packet += 0x10;
        *packet++ = 0x10000005;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x6C040024;
        COPY_MATRIX(packet, part_turns[i]);
        if (MODEL_PART_PARENT_ANY(part) != -1) { // match: always true, the 7-bit field holds 0x7F for "no parent"
            packet = func_00109990(0xC);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040008;
            COPY_MATRIX(packet, part_matrices[MODEL_PART_PARENT_ANY(part)]);
            packet += 0x10;
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040028;
            COPY_MATRIX(packet, part_turns[MODEL_PART_PARENT_ANY(part)]);
        }
        data = (ModelPartData *)(u32)part->data_addr;
        packet = func_00109990(part->data_count);
        for (j = 0; j < part->data_count; j++) {
            word = *(u32 *)data;
            *packet++ = (word & 0x8FFFFFFF) | 0x30000000;
            *packet = (u32)data + 0x10;
            packet += 3;
            data = (ModelPartData *)((char *)data + ((word & 0xFFFF) + 1) * 16);
        }
    }
    packet = func_00109990(1); // match: packet named after packet_start's last use, or the stores go through packet_start
    func_00109650(packet_start, packet);
}
