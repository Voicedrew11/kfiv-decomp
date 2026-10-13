#include "kf_entity.h"

extern f32 D_00431AF0[16];

void sceVu0CopyMatrix(void *, void *);
void sceVu0TransposeMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void func_00216D20(void *, void *);
void func_00216CD8(void *, void *, void *);
void func_00216B48(int, int *, int);
s32 func_0018BD18(void *, void *, void *);
s32 func_0018C298(f32 *, f32 *, f32);
void func_0018C4B8(void *, void *);

/* the angles that turn a part of the model to look at `target` (kept within `limits`, if
   given, of the model's heading); FALSE: the model has no such part */
s32 func_0018C0D0(ModelInstance *model, void *part, f32 *target, f32 *limits, f32 *angles) {
    f32 vector[4];
    f32 look[16];
    f32 part_matrix[16];

    sceVu0CopyMatrix(part_matrix, D_00431AF0);
    if (func_0018BD18(model->parts, part, part_matrix) == 0) {
        return FALSE;
    }
    func_00216B48((int)part_matrix, (int *)model->unk40, (int)part_matrix);
    sceVu0CopyMatrix(look, D_00431AF0);
    Vector_Subtract(vector, &part_matrix[12], target);
    if (limits != NULL) {
        func_0018C298(vector, limits, model->rotation.y);
    }
    func_00216D20(&look[8], vector);
    vector[0] = 0.0f;
    vector[1] = 1.0f;
    vector[2] = 0.0f;
    vector[3] = 0.0f;
    func_00216CD8(look, vector, &look[8]);
    func_00216D20(look, look);
    func_00216CD8(&look[4], &look[8], look);
    func_00216D20(&look[4], &look[4]);
    if (model->unkE0 < 0.0f) {
        Vector_Scale(look, look, -1.0f);
    }
    look[3] = look[7] = look[11] = 0.0f;
    part_matrix[14] = part_matrix[13] = part_matrix[12] = 0.0f;
    part_matrix[15] = 1.0f;
    sceVu0TransposeMatrix(part_matrix, part_matrix);
    func_00216B48((int)look, (int *)part_matrix, (int)look);
    func_0018C4B8(angles, look);
    return TRUE;
}
