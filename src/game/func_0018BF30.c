#include "kf_entity.h"

extern f32 D_00431AF0[16];

void sceVu0CopyMatrix(void *, void *);
s32 func_0018BD18(void *, void *, void *);
void func_00216B48(int, int *, int);

s32 func_0018BF30(ModelInstance *model, void *bone, f32 *matrix) {
    sceVu0CopyMatrix(matrix, D_00431AF0);
    if (func_0018BD18(model->parts, bone, matrix) == 0) {
        return FALSE;
    }
    func_00216B48((int)matrix, (int *)model->unk40, (int)matrix);
    return TRUE;
}
