#include "common.h"

void sceVu0UnitMatrix(f32 (*)[4]);

/* Builds a perspective matrix from a screen scale and the near and far plane distances. */
void func_0020B5F8(f32 (*matrix)[4], f32 screen_scale, f32 near_plane, f32 far_plane) {
    sceVu0UnitMatrix(matrix);
    matrix[1][1] = matrix[0][0] = screen_scale * 0.0009765625f;
    matrix[2][2] = (far_plane + near_plane) / (far_plane - near_plane);
    matrix[3][2] = far_plane * near_plane * -2.0f / (far_plane - near_plane);
    matrix[2][3] = 1.0f;
    matrix[3][3] = 0.0f;
}
