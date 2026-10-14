#include "kf_entity.h"

extern EntityRangeWeights D_0043EB68;

int rand(void);
float sinf(float);
float cosf(float);
void Vector_Add(void *, void *, void *);

/* picks a random point within `range` of `centre`. The nearer half of the range is twice as
   likely while `position` is in the outer half, and the other way round. */
void func_0018B778(float range, void *point_arg, void *centre, void *position) {
    EntityRangeWeights table = D_0043EB68;
    f32 *point = point_arg;
    f32 distance_out[4];
    f32 angle;
    s32 total;
    s32 roll;
    s32 i;

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
        : : "r"(position), "r"(centre), "r"(distance_out) : "memory");
    if ((s32)(distance_out[0] * 100.0f / range) > 50) {
        for (i = 0; i < 5; i++) {
            table.weights[i + 5] *= 2;
        }
    } else {
        for (i = 0; i < 5; i++) {
            table.weights[i] *= 2;
        }
    }
    total = 0;
    for (i = 0; i < 10; i++) {
        total += table.weights[i];
    }
    roll = rand() % total;
    for (i = 0; i < 10; i++) {
        if (roll < table.weights[i]) {
            break;
        }
    }
    range *= (10.0f - i) / 10.0f - (rand() % 10) / 10.0f;
    angle = (rand() % 360) * 0.017453292f;
    if (angle >= 3.1415927f) {
        angle -= 6.2831855f;
    }
    point[0] = -sinf(angle) * range;
    point[1] = 0.0f;
    point[2] = -cosf(angle) * range;
    Vector_Add(point, point, centre);
}
