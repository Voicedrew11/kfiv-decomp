#include "common.h"

/* Dot product of two 3D vectors. */
float func_001B0970(void *left, void *right) {
    f32 *left_xyz = left;
    f32 *right_xyz = right;

    return left_xyz[0] * right_xyz[0] + left_xyz[1] * right_xyz[1] + left_xyz[2] * right_xyz[2];
}
