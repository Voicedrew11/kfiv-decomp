#include "kf_map.h"

typedef u32 u_long128 __attribute__((mode(TI)));

extern ModelLight D_003E7500[];

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
int ModelMatrix_BuildDrawMatrices(ModelInstance *, s32, s32, void *, void *); // match: an int return keeps $2 busy after the call
void Vector_Copy(void *, void *);

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

/* Draws a model like func_0010D308, with the two matrices of ModelMatrix_BuildDrawMatrices on the stack and each
   part's own matrix sent as it is; parts scaled to nothing are left out. */
void func_0010D700(ModelInstance *model) {
    VuVector4 matrix_a[4];
    VuVector4 matrix_b[4];
    void *matrix_b_ptr;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    u32 word;

    if (model->parts != NULL && model->part_count != 0) {
        matrix_b_ptr = matrix_b;
        ModelMatrix_BuildDrawMatrices(model, model->unk148, model->unk148 + 0x40, matrix_a, matrix_b_ptr);
        if (model->unk100 & 0x800) {
            packet_start = packet = func_00109990(0x21); // match: two calls, the compiler merges them
        } else {
            packet_start = packet = func_00109990(0x1C);
        }
        *packet = 0x10000009;
        packet += 7;
        *packet++ = 0x6C08000C;
        COPY_MATRIX(packet, matrix_a);
        packet += 0x10;
        COPY_MATRIX(packet, matrix_b_ptr);
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
        if (model->unk100 & 0x800) {
            *packet++ = 0x10000004;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x6C030064;
            Vector_Copy(packet, &model->unk150);
            packet += 4;
            packet[3] = model->found_record_id;
            ((u_long128 *)packet)[1] = *(u_long128 *)model->found_record;
        }
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->unk60[0] == 0.0f && part->unk60[1] == 0.0f && part->unk60[2] == 0.0f) {
                continue;
            }
            packet = func_00109990(part->data_count + 6);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            COPY_MATRIX(packet, part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & 0x8FFFFFFF) | 0x30000000;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = (ModelPartData *)((char *)data + ((word & 0xFFFF) + 1) * 16);
            }
        }
        packet = func_00109990(1);
        func_00109650(packet_start, packet);
    }
}
