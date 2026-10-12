#include "kf_entity.h"

extern f32 D_00431AF0[16];

void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216B48(int, int *, int);

/* func_00194FB8, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* looks for `target` in the tree of parts under `part` and multiplies `matrix` by the
   transforms on the way to it; FALSE: not found */
s32 func_0018BD18(void *part_arg, void *target_arg, void *matrix) {
    ModelPart *part = part_arg;
    ModelPart *target = target_arg;
    f32 local[16];
    s32 found;

    if (part == target) {
        sceVu0TransMatrix(local, D_00431AF0, target->unk40);
        func_00216B48((int)matrix, (int *)matrix, (int)local);
        return TRUE;
    }
    if (part->child != NULL && func_0018BD18(part->child, target, matrix) != 0) {
        if ((s32)(MODEL_PART_WORD(part) >> 32) & 0x7F) {
            sceVu0RotMatrixX(local, D_00431AF0, wrap_angle(part->unk50));
            sceVu0RotMatrixZ(local, local, wrap_angle(part->unk58));
            sceVu0RotMatrixY(local, local, wrap_angle(part->unk54));
            sceVu0TransMatrix(local, local, part->unk40);
            func_00216B48((int)matrix, (int *)local, (int)matrix);
        }
        return TRUE;
    }
    if (part->sibling != NULL) {
        found = func_0018BD18(part->sibling, target, matrix);
        if (found != 0) {
            return TRUE;
        }
    }
    return FALSE;
}
