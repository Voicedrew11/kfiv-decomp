/* VU0 macro-mode helpers that keep their matrices in VU0 registers between calls (vf1..vf4 a transform to the
   screen, vf5..vf7 and vf8..vf11 for lighting, vf12..vf14 for texture coordinates, vf21.w a scale), and a GIF DMA
   send. Nothing in the game calls any of them. */
#include "common.h"
#include <eeregs.h>

/* Transforms the point at src by the matrix in vf1..vf4, divides by w and stores it at dest as 12.4 fixed-point
   integers. */
static void Vu0_TransformToScreen(void *src, void *dest) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "vmulax.xyzw ACC, $vf1, $vf16x\n"
        "vmadday.xyzw ACC, $vf2, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
        "vmaddw.xyzw $vf17, $vf4, $vf16w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vdiv Q, $vf0w, $vf17w\n"
        "vwaitq\n"
        "vmulq.xyzw $vf26, $vf17, Q\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vftoi4.xyzw $vf17, $vf26\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf17, 0x0(%1)"
        : : "r"(src), "r"(dest));
}

/* Multiplies a normal by the matrix in vf5..vf7 and clamps each component of the result at zero. */
static void Vu0_TransformNormalClamped(void *normal, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "vmulax.xyzw ACC, $vf5, $vf16x\n"
        "vmadday.xyzw ACC, $vf6, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf7, $vf16z\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vmaxx.xyzw $vf17, $vf17, $vf0x\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf17, 0x0(%1)"
        : : "r"(normal), "r"(result));
}

/* Applies the colour matrix in vf8..vf11 to the light factors (its fourth row added whole), multiplies by the
   colour and stores the result as integers. */
static void Vu0_ApplyLightColours(void *factors, void *colour, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf19, 0x0(%1)\n"
        "vmulax.xyzw ACC, $vf8, $vf16x\n"
        "vmadday.xyzw ACC, $vf9, $vf16y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf16z\n"
        "vmaddw.xyzw $vf17, $vf11, $vf0w\n"
        "vmul.xyzw $vf17, $vf17, $vf19\n"
        "vftoi0.xyzw $vf17, $vf17\n"
        "sqc2 $vf17, 0x0(%2)"
        : : "r"(factors), "r"(colour), "r"(result));
}

/* Stores the first vector as integers, with w taken from the second vector's w. */
static void Vu0_ColourToIntWithAlpha(void *colour, void *alpha_source, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "lqc2 $vf19, 0x0(%1)\n"
        "vadd.xyzw $vf17, $vf0, $vf16\n"
        "vmulw.w $vf17, $vf0, $vf19w\n"
        "vftoi0.xyzw $vf17, $vf17\n"
        "sqc2 $vf17, 0x0(%2)"
        : : "r"(colour), "r"(alpha_source), "r"(result));
}

/* Converts a float vector to integers; the first argument is not used. */
static void Vu0_ColourToInt(void *unused, void *source, void *result) {
    __asm__ volatile(
        "lqc2 $vf16, 0x0(%0)\n"
        "vftoi0.xyzw $vf17, $vf16\n"
        "sqc2 $vf17, 0x0(%1)"
        : : "r"(source), "r"(result));
}

/* Loads a 4x4 matrix into vf8..vf11. */
static void Vu0_LoadVf8ToVf11(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf8, 0x0(%0)\n"
        "lqc2 $vf9, 0x10(%0)\n"
        "lqc2 $vf10, 0x20(%0)\n"
        "lqc2 $vf11, 0x30(%0)\n"
        : : "r"(matrix));
}

/* Loads the first three rows of a matrix into vf5..vf7. */
static void Vu0_LoadVf5ToVf7(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf5, 0x0(%0)\n"
        "lqc2 $vf6, 0x10(%0)\n"
        "lqc2 $vf7, 0x20(%0)\n"
        : : "r"(matrix));
}

/* Loads a 4x4 matrix into vf1..vf4. */
static void Vu0_LoadVf1ToVf4(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%0)\n"
        "lqc2 $vf2, 0x10(%0)\n"
        "lqc2 $vf3, 0x20(%0)\n"
        "lqc2 $vf4, 0x30(%0)\n"
        : : "r"(matrix));
}

/* The same as Vu0_LoadVf5ToVf7. */
static void func_00207690(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf5, 0x0(%0)\n"
        "lqc2 $vf6, 0x10(%0)\n"
        "lqc2 $vf7, 0x20(%0)\n"
        : : "r"(matrix));
}

/* The same as Vu0_LoadVf8ToVf11. */
static void func_002076A0(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf8, 0x0(%0)\n"
        "lqc2 $vf9, 0x10(%0)\n"
        "lqc2 $vf10, 0x20(%0)\n"
        "lqc2 $vf11, 0x30(%0)\n"
        : : "r"(matrix));
}

/* Loads a 4x4 matrix into vf12..vf15. */
static void Vu0_LoadVf12ToVf15(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf12, 0x0(%0)\n"
        "lqc2 $vf13, 0x10(%0)\n"
        "lqc2 $vf14, 0x20(%0)\n"
        "lqc2 $vf15, 0x30(%0)\n"
        : : "r"(matrix));
}

/* The same as Vu0_LoadVf1ToVf4. */
static void func_002076D0(void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%0)\n"
        "lqc2 $vf2, 0x10(%0)\n"
        "lqc2 $vf3, 0x20(%0)\n"
        "lqc2 $vf4, 0x30(%0)\n"
        : : "r"(matrix));
}

/* Sets vf21 to (0, 0, 0, 1/2), the scale the texture-coordinate helpers use (vf20 is left as (1, 1, 1, 2)). */
static void Vu0_SetHalfScale(void) {
    __asm__ volatile(
        "vaddw.xyzw $vf20, $vf0, $vf0w\n"
        "vdiv Q, $vf0w, $vf20w\n"
        "vmul.xyzw $vf21, $vf0, $vf0\n"
        "vwaitq\n"
        "vmulq.w $vf21, $vf0, Q");
}

/* Vu0_TexCoordFromNormal for the vector already in vf16. */
static void Vu0_TexCoordFromLoaded(void *result) {
    __asm__ volatile(
        "vmulax.xyzw ACC, $vf12, $vf16x\n"
        "vmadday.xyzw ACC, $vf13, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
        "vaddw.xy $vf17, $vf17, $vf0w\n"
        "vmulw.xy $vf17, $vf17, $vf21w\n"
        "vaddw.z $vf17, $vf0, $vf0w\n"
        "sqc2 $vf17, 0x0(%0)"
        : : "r"(result));
}

/* Transforms the vector at src by vf12..vf14 and maps x and y from -1..1 to 0..1 (times vf21.w); z becomes 1. */
static void Vu0_TexCoordFromNormal(void *dest, void *src) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "vmulax.xyzw ACC, $vf12, $vf16x\n"
        "vmadday.xyzw ACC, $vf13, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
        "vaddw.xy $vf17, $vf17, $vf0w\n"
        "vmulw.xy $vf17, $vf17, $vf21w\n"
        "vaddw.z $vf17, $vf0, $vf0w\n"
        "sqc2 $vf17, 0(%0)\n"
        : : "r"(dest), "r"(src));
}

/* Vu0_TexCoordFromNormal with the arguments the other way round (src, dest). */
static void func_00207748(void *src, void *dest) {
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "vmulax.xyzw ACC, $vf12, $vf16x\n"
        "vmadday.xyzw ACC, $vf13, $vf16y\n"
        "vmaddz.xyzw $vf17, $vf14, $vf16z\n"
        "vaddw.xy $vf17, $vf17, $vf0w\n"
        "vmulw.xy $vf17, $vf17, $vf21w\n"
        "vaddw.z $vf17, $vf0, $vf0w\n"
        "sqc2 $vf17, 0(%1)\n"
        : : "r"(src), "r"(dest));
}

/* Waits until DMA channel 2 and the GIF are idle, then sends qword_count quadwords from the scratchpad
   address src_addr to the GIF. */
static void Gif_SendFromScratchpad(s32 src_addr, s32 qword_count) {
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    *D2_QWC = qword_count;
    *D2_MADR = (src_addr & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x105;
}
