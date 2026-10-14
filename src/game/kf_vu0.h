/* VU0 vector and matrix helpers (kf_vu0.c). Vectors are 16 bytes, four floats (or four ints after a
   conversion); matrices are four such rows. */
#ifndef KF_VU0_H
#define KF_VU0_H

#include "common.h"

/* Copies one 16-byte vector. */
void Vector_Copy(void *dst, void *src);

/* Copies one 16-byte vector: the same code as Vector_Copy. */
void Vector_CopyQuadword(void *dst, void *src);

/* Loads the matrix the projections apply into VU0 (vf1-vf4). */
void Vu0_LoadMatrix(void *matrix);

/* Loads the three light directions Vu0_LightColor uses (vf5-vf7). */
void Vu0_LoadLightDirections(void *rows);

/* Loads the three light colours and the ambient colour Vu0_LightColor uses (vf8-vf11). */
void Vu0_LoadLightColors(void *matrix);

/* Loads the two terms the fog projections build w from (vf12, vf13). */
void Vu0_LoadFogTerms(void *first, void *second);

/* Projects a point with the loaded matrix and stores the screen position as fixed point with 4 fraction bits. */
void Vu0_ProjectToScreen(void *screen, void *point);

/* Vu0_ProjectToScreen with the fog value in w. */
void Vu0_ProjectToScreenFog(void *screen, void *point);

/* Vu0_ProjectToScreen for three points. */
void Vu0_ProjectToScreen3(void *screen0, void *screen1, void *screen2, void *point0, void *point1, void *point2);

/* Vu0_ProjectToScreenFog for three points. */
void Vu0_ProjectToScreenFog3(void *screen0, void *screen1, void *screen2, void *point0, void *point1, void *point2);

/* Lights a vertex colour with the loaded lights and stores it as integers. */
void Vu0_LightColor(void *color_out, void *normal, void *color);

/* Vu0_LightColor for three vertices. */
void Vu0_LightColor3(void *color_out0, void *color_out1, void *color_out2, void *normal0, void *normal1, void *normal2,
                     void *color0, void *color1, void *color2);

/* dst = matrix * src, 4x4 matrices. */
void Matrix_Multiply(void *dst, void *matrix, void *src);

/* dst = matrix * src, all four components. */
void Vector_ApplyMatrix(void *dst, void *matrix, void *src);

/* dst = lhs * rhs component by component. */
void Vector_MultiplyComponents(void *dst, void *lhs, void *rhs);

/* Float vector to fixed point with 4 fraction bits. */
void Vector_ToFixed4(void *dst, void *src);

/* Float vector to integers. */
void Vector_ToInt(void *dst, void *src);

/* Fixed point vector with 4 fraction bits to floats. */
void Vector_FromFixed4(void *dst, void *src);

/* Integer vector to floats. */
void Vector_FromInt(void *dst, void *src);

/* dst = lhs + rhs (x, y, z). */
void Vector_Add(void *dst, void *lhs, void *rhs);

/* dst = lhs + rhs (all four). */
void Vector_Add4(void *dst, void *lhs, void *rhs);

/* dst = lhs - rhs (x, y, z). */
void Vector_Subtract(void *dst, void *lhs, void *rhs);

/* dst = lhs - rhs (all four). */
void Vector_Subtract4(void *dst, void *lhs, void *rhs);

/* dst = src * scale (x, y, z). */
void Vector_Scale(void *dst, void *src, float scale);

/* dst = src * scale (all four). */
void Vector_Scale4(void *dst, void *src, float scale);

/* dst = lhs x rhs. */
void Vector_Cross(void *dst, void *lhs, void *rhs);

/* dst = the four vectors weighted by the four components of weights (x, y, z). */
void Vector_WeightedSum4(void *dst, void *vector0, void *vector1, void *vector2, void *vector3, float *weights);

/* dst = src scaled to length 1 (x, y, z). */
void Vector_Normalize(void *dst, void *src);

#endif
