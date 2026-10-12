#include "kf_map.h"

extern f32 D_00431AF0[16];

void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void func_00216B48(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void KF_Vector_Scale(float *, float *, float);

/* An angle brought back into -half_turn..half_turn. */
static inline f32 func_001229D0_wrap_angle(f32 angle, f32 half_turn) {
    angle += half_turn;
    angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    return angle - half_turn;
}

/* Rebuilds the trigger's model matrices, then turns the whole model about its second part's pivot. */
void func_001229D0(MapTrigger *trigger) {
    f32 matrix[16];
    f32 pivot_back[4];
    ModelPart *part;
    f32 *pivot;
    int part_count;
    int i;
    ModelPart *joint;
    f32 half_turn;

    func_0020C300(trigger->model, &trigger->model->position, &trigger->model->rotation, &trigger->model->unkE0);
    part_count = trigger->model->part_count;
    part = trigger->model->parts;
    for (i = 0; i < part_count; i++) {
        func_0020B8B0(trigger->model, part, part);
        part++;
    }
    part = trigger->model->parts;
    if (trigger->model->unk100 & 1) {
        for (i = 0; i < part_count; i++) {
            func_0010B5D0(part);
            part++;
        }
        part = trigger->model->parts;
    }
    joint = part;
    half_turn = 3.1415927f;
    sceVu0CopyMatrix(matrix, D_00431AF0);
    // match: an empty loop here (a debug macro compiled out?) keeps the scheduler from moving anything across
    // this point, which is what leaves the half_turn load in front of the call above
    do { } while (0);
    pivot = joint[1].unk40;
    KF_Vector_Scale(pivot_back, pivot, -1.0f);
    joint++;
    sceVu0TransMatrix(matrix, matrix, pivot_back);
    sceVu0RotMatrixX(matrix, matrix, func_001229D0_wrap_angle(joint->unk50, half_turn));
    sceVu0RotMatrixZ(matrix, matrix, func_001229D0_wrap_angle(joint->unk58, half_turn));
    sceVu0RotMatrixY(matrix, matrix, func_001229D0_wrap_angle(joint->unk54, half_turn));
    sceVu0TransMatrix(matrix, matrix, pivot);
    func_00216B48(trigger->model->unk80, trigger->model->unk40, matrix);
    trigger->model->unk1B4(trigger->model);
}
