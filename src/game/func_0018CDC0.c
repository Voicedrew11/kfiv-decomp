#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

float atan2f(float, float);
void func_001B2C48(void *);

/* calls func_001B2C48 on target if the player is near `point` and within max_angle of `heading` */
s32 func_0018CDC0(void *target, f32 *point, f32 max_angle, f32 max_height, f32 max_distance, f32 heading) {
    f32 distance_out[4];
    f32 angle;

    if (point[1] < D_00413F80.position.y - 230.0f) {
        return FALSE;
    }
    if (D_00413F80.position.y < point[1] - max_height) {
        return FALSE;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(point), "r"(&D_00413F80), "r"(distance_out) : "memory");
    if (max_distance < distance_out[0]) {
        return FALSE;
    }
    angle = heading - atan2f(D_00413F80.position.x - point[0], D_00413F80.position.z - point[2]);
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    func_001B2C48(target);
    return TRUE;
}
