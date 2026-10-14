#include "common.h"

void sceVu0CopyMatrix(void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);

void func_0020CB70(void *dst, void *src_matrix, void *src_vec) {
    f32 *scratch = (f32 *)0x70000000;

    sceVu0CopyMatrix(scratch, src_matrix);
    scratch[12] = 0.0f;
    scratch[13] = 0.0f;
    scratch[14] = 0.0f;
    Vector_ApplyMatrix(dst, scratch, src_vec);
}
