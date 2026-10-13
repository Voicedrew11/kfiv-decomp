#include "kf_entity.h"

extern f32 D_00431AF0[16];

void sceVu0CopyMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void func_00216D20(void *, void *);
void func_00216CD8(void *, void *, void *);

/* builds the rotation matrix that looks from `origin` towards `target`; `up_sign` below 0
   takes "up" as down, `side_sign` below 0 mirrors the sideways axis */
void func_0018BFA8(f32 *target, f32 *origin, s32 up_sign, f32 *matrix, f32 side_sign) {
    f32 vector[4];
    f32 *forward = &matrix[8];
    f32 *side;

    sceVu0CopyMatrix(matrix, D_00431AF0);
    Vector_Subtract(vector, target, origin);
    func_00216D20(forward, vector);
    // match: the whole vector is set in both arms
    if (up_sign >= 0) {
        vector[0] = 0.0f;
        vector[1] = 1.0f;
        vector[2] = 0.0f;
        vector[3] = 0.0f;
    } else {
        vector[0] = 0.0f;
        vector[1] = -1.0f;
        vector[2] = 0.0f;
        vector[3] = 0.0f;
    }
    func_00216CD8(matrix, vector, forward);
    side = &matrix[4];
    func_00216D20(matrix, matrix);
    func_00216CD8(side, forward, matrix);
    func_00216D20(side, side);
    if (side_sign < 0.0f) {
        Vector_Scale(matrix, matrix, -1.0f);
    }
    matrix[3] = matrix[7] = matrix[11] = 0.0f;
}
