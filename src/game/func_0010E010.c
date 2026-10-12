#include "kf_gs.h"
#include "kf_map.h"

typedef u32 u_long128 __attribute__((mode(TI)));

extern ModelLight D_003E7500[];

u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
int func_0020CBC0(ModelInstance *, s32, s32, void *, void *); // match: an int return keeps $2 busy after the call
void func_00216B48(void *, void *, void *);

/* the scratchpad as this function uses it: two matrices func_0020CBC0 makes for the model */
#define SCRATCH_MATRIX_A ((void *)0x70000000)
#define SCRATCH_MATRIX_B ((void *)0x70000040)

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

/* Draws a model: queues a packet with its two matrices, its MapSlotUnk0C block and its lights,
   then for every part that is switched on and not scaled to nothing the GS state and a packet
   with the part's matrix and a reference to each of its data blocks. */
void func_0010E010(ModelInstance *model) {
    void *matrix_a = SCRATCH_MATRIX_A;
    void *matrix_b = SCRATCH_MATRIX_B;
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
        func_0020CBC0(model, model->unk148, model->unk148 + 0x40, matrix_a, matrix_b);
        /* the packet below is 0x1C quadwords either way: with the flag set the last 5 are never written */
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
        func_00109650(packet_start, func_00109990(1)); /* the packet ends where the next one would start */
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->unk60[0] == 0.0f && part->unk60[1] == 0.0f && part->unk60[2] == 0.0f) {
                continue;
            }
            func_00109A50();
            if (model->unk145 == 0) {
                func_00109A80(0x1A, 1);
                func_00109A80(GS_REG_TEST_1, 0x5000D);
                func_00109A80(GS_REG_ZBUF_1, 0xA000118);
                if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT41) {
                    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
                } else {
                    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
                }
            } else {
                func_00109A80(0x1A, 0);
                func_00109A80(0x1B, 0x278);
                data = (ModelPartData *)(u32)part->data_addr;
                func_00109A80(0x15, *(s64 *)&data->unk30);
                func_00109A80(0x07, data->unk40);
                func_00109A80(GS_REG_ALPHA_2,
                              (s32)((GsAlphaReg *)&data->unk50)->f.a_source | ((s32)((GsAlphaReg *)&data->unk50)->f.b_source << 2) | 0x20 |
                                  ((s32)((GsAlphaReg *)&data->unk50)->f.d_source << 6) | ((s64)(s16)model->unk146 << 32));
                func_00109A80(GS_REG_TEST_2, 0x5000D);
                if ((s16)model->unk146 >= 0x80) {
                    func_00109A80(GS_REG_ZBUF_2, 0xA000118);
                } else {
                    func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
                }
            }
            func_00109AD0();
            packet_start = packet = func_00109990(part->data_count + 7);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            func_00216B48((int)packet, (int *)model->unk40, (int)part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & 0x8FFFFFFF) | 0x30000000;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = (ModelPartData *)((char *)data + ((word & 0xFFFF) + 1) * 16);
            }
            func_00109650(packet_start, packet);
        }
        func_00109A50();
        func_00109A80(0x1A, 1);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L); // match: ZBUF_2 twice; ZBUF_1 is left as the last part set it
        func_00109AD0();
    }
}
