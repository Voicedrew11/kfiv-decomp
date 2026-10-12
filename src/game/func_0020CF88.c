#include "common.h"

extern f32 D_00446FA4[];

/* Steps *angle by 0.01 towards target (the threshold is a global holding -0.0f). */
void func_0020CF88(f32 *angle, f32 target) {
    f32 current = *angle;

    if (current - target < D_00446FA4[0]) {
        *angle = current + 0.01f;
    } else {
        *angle = current - 0.01f;
    }
}
