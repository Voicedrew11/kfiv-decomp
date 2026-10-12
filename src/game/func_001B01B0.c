#include "kf_model.h"

// Horizontal (XZ-plane) distance between origin and target.
float func_001B01B0(float *origin, float *target) {
    Vector4 out;
    Vector4 target_at_origin_height;

    target_at_origin_height.x = target[0];
    target_at_origin_height.y = origin[1];
    target_at_origin_height.z = target[2];
    target_at_origin_height.w = 0;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf17, $vf16\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vaddx.z $vf17, $vf17, $vf17x\n"
        "vaddy.z $vf17, $vf17, $vf17y\n"
        "vsqrt Q, $vf17z\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        :
        : "r"(origin), "r"(&target_at_origin_height), "r"(&out)
        : "memory");
    return out.x;
}
