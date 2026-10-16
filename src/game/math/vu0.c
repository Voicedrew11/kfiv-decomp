/* VU0 macro-mode helpers: vector and matrix arithmetic on 16-byte vectors (four floats), and the transform,
   projection and lighting steps of the model drawing, which keep their matrices in VU0 registers between calls
   (vf1-vf4 the projection matrix, vf5-vf7 the light directions, vf8-vf11 the light colours, vf12/vf13 the fog
   terms). */
#include "math/vu0.h"

/* Copies one 16-byte vector (the same code as libvu0's sceVu0CopyVector). */
void Vector_Copy(void *dst, void *src) {
    __asm__ volatile(
        "lq $6, 0x0(%1)\n"
        "sq $6, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "$6", "memory");
}

/* Copies one 16-byte vector: the same code as Vector_Copy, a second copy of it. */
void Vector_CopyQuadword(void *dst, void *src) {
    __asm__ volatile(
        "lq $6, 0x0(%1)\n"
        "sq $6, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "$6", "memory");
}

/* Loads the matrix the projections apply into vf1 to vf4. */
void Vu0_LoadMatrix(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%0)\n"
        "lqc2 $vf2, 0x10(%0)\n"
        "lqc2 $vf3, 0x20(%0)\n"
        "lqc2 $vf4, 0x30(%0)\n"
        : : "r"(matrix));
}

/* Loads the three light directions Vu0_LightColor takes the normal against into vf5 to vf7. */
void Vu0_LoadLightDirections(void *rows) {
    __asm__ volatile(
        "lqc2 $vf5, 0x0(%0)\n"
        "lqc2 $vf6, 0x10(%0)\n"
        "lqc2 $vf7, 0x20(%0)\n"
        : : "r"(rows));
}

/* Loads the three light colours and the ambient colour (the last row) Vu0_LightColor uses into vf8 to vf11. */
void Vu0_LoadLightColors(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf8, 0x0(%0)\n"
        "lqc2 $vf9, 0x10(%0)\n"
        "lqc2 $vf10, 0x20(%0)\n"
        "lqc2 $vf11, 0x30(%0)\n"
        : : "r"(matrix));
}

/* Loads the two terms the fog projections build w from (w = first.w + second.w / w) into vf12 and vf13. */
void Vu0_LoadFogTerms(void *first, void *second) {
    __asm__ volatile(
        "lqc2 $vf12, 0x0(%0)\n"
        "lqc2 $vf13, 0x0(%1)\n"
        : : "r"(first), "r"(second));
}

/* Applies the vf1-vf4 matrix to a point, divides by w and stores the result as fixed point with 4 fraction
   bits: the screen position. */
void Vu0_ProjectToScreen(void *screen, void *point) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vdiv Q, $vf0w, $vf16w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmulq.xyzw $vf26, $vf16, Q\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vftoi4.xyzw $vf16, $vf26\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(screen), "r"(point) : "memory");
}

/* Vu0_ProjectToScreen with the fog value in w: vf12.w + vf13.w / w. */
void Vu0_ProjectToScreenFog(void *screen, void *point) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vdiv Q, $vf0w, $vf16w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmula.w ACC, $vf12, $vf0\n"
        "vmaddq.w $vf26, $vf13, Q\n"
        "vmulq.xyz $vf26, $vf16, Q\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vftoi4.xyzw $vf16, $vf26\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf16, 0(%0)"
        : : "r"(screen), "r"(point));
}

/* Vu0_ProjectToScreen for the three points of a triangle, interleaved. */
void Vu0_ProjectToScreen3(void *screen0, void *screen1, void *screen2, void *point0, void *point1, void *point2) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "lqc2 $vf17, 0(%4)\n"
        "lqc2 $vf18, 0(%5)\n"
        "vnop\n"
        "vdiv Q, $vf0w, $vf16w\n"
        "vnop\n"
        "vmulax.xyzw ACC, $vf1, $vf17x\n"
        "vmadday.xyzw ACC, $vf2, $vf17y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
        "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
        "vmulax.xyzw ACC, $vf1, $vf18x\n"
        "vmulq.xyzw $vf26, $vf16, Q\n"
        "vmadday.xyzw ACC, $vf2, $vf18y\n"
        "vdiv Q, $vf0w, $vf17w\n"
        "vnop\n"
        "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
        "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
        "vftoi4.xyzw $vf16, $vf26\n"
        "vnop\n"
        "vnop\n"
        "vmulq.xyzw $vf27, $vf17, Q\n"
        "vdiv Q, $vf0w, $vf18w\n"
        "sqc2 $vf16, 0(%0)\n"
        "vnop\n"
        "vftoi4.xyzw $vf17, $vf27\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmulq.xyzw $vf28, $vf18, Q\n"
        "sqc2 $vf17, 0(%1)\n"
        "vnop\n"
        "vnop\n"
        "vftoi4.xyzw $vf18, $vf28\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf18, 0(%2)\n"
        : : "r"(screen0), "r"(screen1), "r"(screen2), "r"(point0), "r"(point1), "r"(point2) : "memory");
}

/* Vu0_ProjectToScreenFog for the three points of a triangle, interleaved. */
void Vu0_ProjectToScreenFog3(void *screen0, void *screen1, void *screen2, void *point0, void *point1, void *point2) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "lqc2 $vf17, 0(%4)\n"
        "lqc2 $vf18, 0(%5)\n"
        "vnop\n"
        "vdiv Q, $vf0w, $vf16w\n"
        "vmulax.xyzw ACC, $vf1, $vf17x\n"
        "vmadday.xyzw ACC, $vf2, $vf17y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
        "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
        "vnop\n"
        "vmula.w ACC, $vf12, $vf0\n"
        "vmaddq.w $vf26, $vf13, Q\n"
        "vmulq.xyz $vf26, $vf16, Q\n"
        "vdiv Q, $vf0w, $vf17w\n"
        "vmulax.xyzw ACC, $vf1, $vf18x\n"
        "vmadday.xyzw ACC, $vf2, $vf18y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
        "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
        "vftoi4.xyzw $vf16, $vf26\n"
        "vmula.w ACC, $vf12, $vf0\n"
        "vmaddq.w $vf27, $vf13, Q\n"
        "vmulq.xyz $vf27, $vf17, Q\n"
        "vdiv Q, $vf0w, $vf18w\n"
        "sqc2 $vf16, 0(%0)\n"
        "vnop\n"
        "vftoi4.xyzw $vf17, $vf27\n"
        "vnop\n"
        "vnop\n"
        "vmula.w ACC, $vf12, $vf0\n"
        "vmaddq.w $vf28, $vf13, Q\n"
        "vmulq.xyz $vf28, $vf18, Q\n"
        "sqc2 $vf17, 0(%1)\n"
        "vnop\n"
        "vnop\n"
        "vftoi4.xyzw $vf18, $vf28\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf18, 0(%2)\n"
        : : "r"(screen0), "r"(screen1), "r"(screen2), "r"(point0), "r"(point1), "r"(point2) : "memory");
}

/* Lights a vertex: the normal against the vf5-vf7 directions (negative amounts count as 0), those amounts
   through the vf8-vf11 colours plus the ambient row, times the vertex colour; stored as integers. */
void Vu0_LightColor(void *color_out, void *normal, void *color) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf19, 0(%2)\n"
        "vmulax.xyzw ACC, $vf5, $vf16x\n"
        "vmadday.xyzw ACC, $vf6, $vf16y\n"
        "vmaddz.xyzw $vf16, $vf7, $vf16z\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmaxx.xyzw $vf16, $vf16, $vf0x\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmulax.xyzw ACC, $vf8, $vf16x\n"
        "vmadday.xyzw ACC, $vf9, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf11, $vf0w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmul.xyzw $vf16, $vf16, $vf19\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vftoi0.xyzw $vf16, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf16, 0(%0)\n"
        : : "r"(color_out), "r"(normal), "r"(color) : "memory");
}

/* Vu0_LightColor for the three vertices of a triangle, interleaved. */
void Vu0_LightColor3(void *color_out0, void *color_out1, void *color_out2, void *normal0, void *normal1, void *normal2,
                     void *color0, void *color1, void *color2) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "vmulax.xyzw ACC, $vf5, $vf16x\n"
        "vmadday.xyzw ACC, $vf6, $vf16y\n"
        "vmaddz.xyzw $vf16, $vf7, $vf16z\n"
        "lqc2 $vf17, 0(%4)\n"
        "vmulax.xyzw ACC, $vf5, $vf17x\n"
        "vmadday.xyzw ACC, $vf6, $vf17y\n"
        "vmaddz.xyzw $vf17, $vf7, $vf17z\n"
        "vmaxx.xyzw $vf16, $vf16, $vf0x\n"
        "lqc2 $vf19, 0(%6)\n"
        "lqc2 $vf18, 0(%5)\n"
        "vmulax.xyzw ACC, $vf8, $vf16x\n"
        "vmadday.xyzw ACC, $vf9, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf11, $vf0w\n"
        "vmulax.xyzw ACC, $vf5, $vf18x\n"
        "vmadday.xyzw ACC, $vf6, $vf18y\n"
        "vmaddz.xyzw $vf18, $vf7, $vf18z\n"
        "vmaxx.xyzw $vf17, $vf17, $vf0x\n"
        "vmul.xyzw $vf16, $vf16, $vf19\n"
        "vmulax.xyzw ACC, $vf8, $vf17x\n"
        "vmadday.xyzw ACC, $vf9, $vf17y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf17z\n"
        "vmaddw.xyzw $vf17, $vf11, $vf0w\n"
        "vftoi0.xyzw $vf16, $vf16\n"
        "vmaxx.xyzw $vf18, $vf18, $vf0x\n"
        "lqc2 $vf20, 0(%7)\n"
        "vmul.xyzw $vf17, $vf17, $vf20\n"
        "sqc2 $vf16, 0(%0)\n"
        "vmulax.xyzw ACC, $vf8, $vf18x\n"
        "vmadday.xyzw ACC, $vf9, $vf18y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf18z\n"
        "vmaddw.xyzw $vf18, $vf11, $vf0w\n"
        "vftoi0.xyzw $vf17, $vf17\n"
        "vnop\n"
        "lqc2 $vf21, 0(%8)\n"
        "vmul.xyzw $vf18, $vf18, $vf21\n"
        "sqc2 $vf17, 0(%1)\n"
        "vnop\n"
        "vnop\n"
        "vftoi0.xyzw $vf18, $vf18\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf18, 0(%2)\n"
        : : "r"(color_out0), "r"(color_out1), "r"(color_out2), "r"(normal0), "r"(normal1), "r"(normal2),
            "r"(color0), "r"(color1), "r"(color2) : "memory");
}

/* dst = matrix * src, 4x4 matrices. */
void Matrix_Multiply(void *dst, void *matrix, void *src) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x10(%1)\n"
        "lqc2 $vf3, 0x20(%1)\n"
        "lqc2 $vf4, 0x30(%1)\n"
        "lqc2 $vf22, 0x0(%2)\n"
        "lqc2 $vf23, 0x10(%2)\n"
        "lqc2 $vf24, 0x20(%2)\n"
        "lqc2 $vf25, 0x30(%2)\n"
        "vmulax.xyzw ACC, $vf1, $vf22x\n"
        "vmadday.xyzw ACC, $vf2, $vf22y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf22z\n"
        "vmaddw.xyzw $vf22, $vf4, $vf22w\n"
        "vmulax.xyzw ACC, $vf1, $vf23x\n"
        "vmadday.xyzw ACC, $vf2, $vf23y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf23z\n"
        "vmaddw.xyzw $vf23, $vf4, $vf23w\n"
        "vmulax.xyzw ACC, $vf1, $vf24x\n"
        "vmadday.xyzw ACC, $vf2, $vf24y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf24z\n"
        "vmaddw.xyzw $vf24, $vf4, $vf24w\n"
        "vmulax.xyzw ACC, $vf1, $vf25x\n"
        "vmadday.xyzw ACC, $vf2, $vf25y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf25z\n"
        "vmaddw.xyzw $vf25, $vf4, $vf25w\n"
        "sqc2 $vf22, 0x0(%0)\n"
        "sqc2 $vf23, 0x10(%0)\n"
        "sqc2 $vf24, 0x20(%0)\n"
        "sqc2 $vf25, 0x30(%0)\n"
        : : "r"(dst), "r"(matrix), "r"(src) : "memory");
}

/* dst = matrix * src, all four components. */
void Vector_ApplyMatrix(void *dst, void *matrix, void *src) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x10(%1)\n"
        "lqc2 $vf3, 0x20(%1)\n"
        "lqc2 $vf4, 0x30(%1)\n"
        "lqc2 $vf16, 0x0(%2)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(matrix), "r"(src) : "memory");
}

/* dst = lhs * rhs component by component, all four components. */
void Vector_MultiplyComponents(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vmul.xyzw $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* Converts a float vector to fixed point with 4 fraction bits. */
void Vector_ToFixed4(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vftoi4.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}

/* Converts a float vector to integers. */
void Vector_ToInt(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vftoi0.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}

/* Converts a fixed point vector with 4 fraction bits to floats. */
void Vector_FromFixed4(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vitof4.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}

/* Converts an integer vector to floats. */
void Vector_FromInt(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "vitof0.xyzw $vf16, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}

/* dst = lhs + rhs (x, y, z only). */
void Vector_Add(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vadd.xyz $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* dst = lhs + rhs, all four components. */
void Vector_Add4(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vadd.xyzw $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* dst = lhs - rhs (x, y, z only). */
void Vector_Subtract(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* dst = lhs - rhs, all four components. */
void Vector_Subtract4(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vsub.xyzw $vf16, $vf16, $vf17\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* dst = src * scale (x, y, z only). */
void Vector_Scale(void *dst, void *src, float scale) {
    __asm__ volatile(
        "mfc1 $2, %2\n"
        "lqc2 $vf16, 0x0(%1)\n"
        "qmtc2 $2, $vf31\n"
        "vmulx.xyz $vf16, $vf16, $vf31x\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src), "f"(scale) : "$2", "memory");
}

/* dst = src * scale, all four components. */
void Vector_Scale4(void *dst, void *src, float scale) {
    __asm__ volatile(
        "mfc1 $2, %2\n"
        "lqc2 $vf16, 0x0(%1)\n"
        "qmtc2 $2, $vf31\n"
        "vmulx.xyzw $vf16, $vf16, $vf31x\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(src), "f"(scale) : "$2", "memory");
}

/* dst = lhs x rhs, the cross product. */
void Vector_Cross(void *dst, void *lhs, void *rhs) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%1)\n"
        "lqc2 $vf17, 0x0(%2)\n"
        "vopmula.xyz ACC, $vf16, $vf17\n"
        "vopmsub.xyz $vf16, $vf17, $vf16\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(lhs), "r"(rhs) : "memory");
}

/* dst = vector0 * weights.x + vector1 * weights.y + vector2 * weights.z + vector3 * weights.w (x, y, z only);
   the callers blend two points and two tangents along a curve with it. */
void Vector_WeightedSum4(void *dst, void *vector0, void *vector1, void *vector2, void *vector3, float *weights) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%5)\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "lqc2 $vf3, 0x0(%3)\n"
        "lqc2 $vf4, 0x0(%4)\n"
        "vmulax.xyz ACC, $vf1, $vf16x\n"
        "vmadday.xyz ACC, $vf2, $vf16y\n"
        "vmaddaz.xyz ACC, $vf3, $vf16z\n"
        "vmaddw.xyz $vf16, $vf4, $vf16w\n"
        "sqc2 $vf16, 0x0(%0)\n"
        : : "r"(dst), "r"(vector0), "r"(vector1), "r"(vector2), "r"(vector3), "r"(weights) : "memory");
}

/* dst = src scaled to length 1 (x, y, z only). */
void Vector_Normalize(void *dst, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vaddx.z $vf17, $vf17, $vf17x\n"
        "vaddy.z $vf17, $vf17, $vf17y\n"
        "vsqrt Q, $vf17z\n"
        "vwaitq\n"
        "vmulq.xyzw $vf17, $vf0, Q\n"
        "vdiv Q, $vf0w, $vf17w\n"
        "vwaitq\n"
        "vmulq.xyz $vf16, $vf16, Q\n"
        "sqc2 $vf16, 0(%0)\n"
        : : "r"(dst), "r"(src) : "memory");
}
