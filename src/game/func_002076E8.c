#include "common.h"

/* Sets vf21.w to 1 / 2 (vf20 = 2 in every field, vf21.xyz = 0). */
void func_002076E8(void) {
    __asm__ volatile(
        "vaddw.xyzw $vf20, $vf0, $vf0w\n"
        "vdiv Q, $vf0w, $vf20w\n"
        "vmul.xyzw $vf21, $vf0, $vf0\n"
        "vwaitq\n"
        "vmulq.w $vf21, $vf0, Q");
}
