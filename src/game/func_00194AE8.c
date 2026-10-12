#include "kf_model.h"

void func_00216778(void *, void *);

/* dst = a * (1 - t) + b * t across the xyz lanes, on the VU0 (func_00194F18 inlined) */
static inline void blend_vectors(Vector4 *dst, Vector4 *src_a, Vector4 *src_b, f32 ratio) {
    __asm__ volatile(
        "qmtc2.ni %3, $vf3\n"
        "vsubx.w $vf4, $vf0, $vf3x\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "vmulax.xyz ACC, $vf2, $vf3x\n"
        "vmaddw.xyz $vf1, $vf1, $vf4w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf1, 0x0(%0)\n"
        : : "r"(dst), "r"(src_a), "r"(src_b), "r"(ratio));
}

/* Poses every part between the first keys of two motions: ratio 0 gives the first motion's, 1 the second's. */
s32 func_00194AE8(ModelInstance *model, s32 table_a, s32 table_b, f32 ratio) {
    Vector4 *point0 = (Vector4 *)0x70000000;
    Vector4 *point1 = (Vector4 *)0x70000010;
    Vector4 *result = (Vector4 *)0x70000020;
    MotionData *motion_a;
    MotionData *motion_b;
    ModelPartLevels *record_a;
    ModelPartLevels *record_b;
    MotionVector *vectors_a;
    MotionVector *vectors_b;
    ModelPart *part;
    MotionKey *key_a;
    MotionKey *key_b;
    s32 i;
    f32 angle;
    f32 angle_alt;

    motion_a = *(MotionData **)((char *)model->level_tables + (table_a << 2));
    motion_a = (MotionData *)((ModelPartLevels *)motion_a - 1);
    motion_b = *(MotionData **)((char *)model->level_tables + (table_b << 2));
    motion_b = (MotionData *)((ModelPartLevels *)motion_b - 1);
    record_a = *(ModelPartLevels **)((char *)model->level_tables + (table_a << 2));
    record_b = *(ModelPartLevels **)((char *)model->level_tables + (table_b << 2));
    vectors_a = motion_a->vectors;
    vectors_b = motion_b->vectors;
    part = model->parts;
    for (i = 0; i < model->part_count; i++, record_a++, record_b++, part++) {
        key_a = (MotionKey *)record_a->thresholds;
        if (key_a == NULL) {
            continue;
        }
        key_b = (MotionKey *)record_b->thresholds;
        if (key_b == NULL) {
            continue;
        }
        if (record_a->count == 0) {
            continue;
        }
        if (record_b->count == 0) {
            continue;
        }
        *(MotionVector *)point0 = vectors_a[key_a->rotation];
        *(MotionVector *)point1 = vectors_b[key_b->rotation];
        blend_vectors(result, point0, point1, ratio);
        angle = result->x;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk50 = angle;
        angle_alt = result->y;
        angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle_alt) {
            angle_alt -= 6.2831855f;
        } else if (angle_alt < -3.1415927f) {
            angle_alt += 6.2831855f;
        }
        part->unk54 = angle_alt;
        angle = result->z;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk58 = angle;

        *(MotionVector *)point0 = vectors_a[key_a->position];
        point0->w = 1.0f;
        *(MotionVector *)point1 = vectors_b[key_b->position];
        point1->w = 1.0f;
        blend_vectors(result, point0, point1, ratio);
        func_00216778(part->unk40, (Vector4 *)0x70000020);

        part->unk60[0] = vectors_a[key_a->scale].x * (1.0f - ratio) + vectors_b[key_b->scale].x * ratio;
        part->unk60[1] = vectors_a[key_a->scale].y * (1.0f - ratio) + vectors_b[key_b->scale].y * ratio;
        part->unk60[2] = vectors_a[key_a->scale].z * (1.0f - ratio) + vectors_b[key_b->scale].z * ratio;
    }
    return 1;
}
