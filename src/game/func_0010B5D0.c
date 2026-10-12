#include "kf_model.h"

/* loads the matrix the children are multiplied by into vf1..vf4 */
#define LOAD_PARENT_MATRIX(matrix)                                                                 \
    __asm__ volatile(                                                                              \
        "lqc2 $vf1, 0x0(%0)\n"                                                                     \
        "lqc2 $vf2, 0x10(%0)\n"                                                                    \
        "lqc2 $vf3, 0x20(%0)\n"                                                                    \
        "lqc2 $vf4, 0x30(%0)\n"                                                                    \
        :                                                                                          \
        : "r"(matrix))

/* multiplies the matrix at `matrix` by the one loaded in vf1..vf4, in place */
#define MULTIPLY_BY_PARENT(matrix)                                                                 \
    __asm__ volatile(                                                                              \
        "lqc2 $vf22, 0x0(%0)\n"                                                                    \
        "lqc2 $vf23, 0x10(%0)\n"                                                                   \
        "lqc2 $vf24, 0x20(%0)\n"                                                                   \
        "lqc2 $vf25, 0x30(%0)\n"                                                                   \
        :                                                                                          \
        : "r"(matrix));                                                                            \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf22x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf22y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf22z\n"                                                         \
        "vmaddw.xyzw $vf22, $vf4, $vf22w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf23x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf23y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf23z\n"                                                         \
        "vmaddw.xyzw $vf23, $vf4, $vf23w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf24x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf24y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf24z\n"                                                         \
        "vmaddw.xyzw $vf24, $vf4, $vf24w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf25x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf25y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf25z\n"                                                         \
        "vmaddw.xyzw $vf25, $vf4, $vf25w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "sqc2 $vf22, 0x0(%0)\n"                                                                    \
        "sqc2 $vf23, 0x10(%0)\n"                                                                   \
        "sqc2 $vf24, 0x20(%0)\n"                                                                   \
        "sqc2 $vf25, 0x30(%0)\n"                                                                   \
        : "=r"(matrix)                                                                             \
        : "0"(matrix))

/* Multiply the matrices of a part's children by the part's own matrix. */
void func_0010B5D0(ModelPart *part) {
    register ModelPart *matrix asm("$2"); // match: the game keeps the matrix address in $2
    ModelPart *child;

    matrix = part;
    child = matrix->child;
    if (child != NULL) {
        LOAD_PARENT_MATRIX(matrix);
        matrix = child;
        MULTIPLY_BY_PARENT(matrix);
        part = matrix->sibling;
        if (part != NULL) {
            do {
                matrix = part;
                MULTIPLY_BY_PARENT(matrix);
                matrix = matrix->sibling;
                part = matrix;
            } while (matrix != NULL);
        }
    }
}
