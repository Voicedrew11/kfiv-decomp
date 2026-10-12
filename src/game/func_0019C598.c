#include "common.h"

extern f32 D_003EF6E0[4];

/* Writes the sine and cosine of an angle within +-pi. All VU0 macro code: the cosine is a polynomial in
   (pi/2 - |angle|), with the coefficients in D_003EF6E0, and the sine its square root complement, negated
   for a negative angle. Despite the order of the parameters the first output is the cosine. */
void func_0019C598(f32 angle, f32 *cos_out, f32 *sin_out) {
    __asm__ volatile(
        "mtc1 $0, $f0\n"
        "c.lt.s %0, $f0\n"
        "li.s $f0, 1.5707964\n"
        "bc1f 0f\n"
        "add.s %0, $f0, %0\n"
        "li $7, 1\n"
        "j 1f\n"
        "0:\n"
        "sub.s %0, $f0, %0\n"
        "move $7, $0\n"
        "1:\n"
        "mfc1 $8, %0\n"
        "qmtc2 $8, $vf6\n"
        "la $8, D_003EF6E0\n"
        "lqc2 $vf5, 0($8)\n"
        "vmr32.w $vf6, $vf6\n"
        "vaddx.x $vf4, $vf0, $vf6x\n"
        "vmul.x $vf6, $vf6, $vf6\n"
        "vmulx.yzw $vf4, $vf4, $vf0x\n"
        "vmulw.xyzw $vf8, $vf5, $vf6w\n"
        "vsub.xyzw $vf5, $vf0, $vf0\n"
        "vmulx.xyzw $vf8, $vf8, $vf6x\n"
        "vmulx.xyz $vf8, $vf8, $vf6x\n"
        "vaddw.x $vf4, $vf4, $vf8w\n"
        "vmulx.xy $vf8, $vf8, $vf6x\n"
        "vaddz.x $vf4, $vf4, $vf8z\n"
        "vmulx.x $vf8, $vf8, $vf6x\n"
        "vaddy.x $vf4, $vf4, $vf8y\n"
        "vaddx.x $vf4, $vf4, $vf8x\n"
        "vaddx.xy $vf4, $vf5, $vf4x\n"
        "sqc2 $vf4, 0(%2)\n"
        "vmul.x $vf7, $vf4, $vf4\n"
        "vsubx.w $vf7, $vf0, $vf7x\n"
        "vsqrt Q, $vf7w\n"
        "vwaitq\n"
        "vaddq.x $vf7, $vf0, Q\n"
        "bnez $7, 2f\n"
        "vaddx.x $vf4, $vf5, $vf7x\n"
        "b 3f\n"
        "2:\n"
        "vsubx.x $vf4, $vf5, $vf7x\n"
        "3:\n"
        "sqc2 $vf4, 0(%3)\n"
        : "=f"(angle)
        : "0"(angle), "r"(cos_out), "r"(sin_out)
        : "$f0", "$7", "$8", "memory");
}
