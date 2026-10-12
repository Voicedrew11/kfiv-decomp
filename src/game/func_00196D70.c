#include "common.h"
#include "kf_entity.h"

float atan2f(float, float);

/* Tests whether the target is within a distance and inside the horizontal and vertical view cones (each given as a full angle); a limit of 0 skips that test. */
s32 func_00196D70(Entity *entity, f32 *target, f32 yaw_range, f32 pitch_range, f32 max_distance) {
    f32 distance[4];
    f32 angle;
    f32 diff;
    f32 magnitude;

    if (max_distance != 0.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%1)\n"
            "lqc2 $vf17, 0(%2)\n"
            "vsub.xyz $vf16, $vf16, $vf17\n"
            "vmul.xyz $vf16, $vf16, $vf16\n"
            "vaddy.x $vf16, $vf16, $vf16y\n"
            "vaddz.x $vf16, $vf16, $vf16z\n"
            "vsqrt Q, $vf16x\n"
            "vwaitq\n"
            "vaddq.x $vf16, $vf0, Q\n"
            "sqc2 $vf16, 0(%3)\n"
            : "=m"(distance) : "r"(entity->model->position), "r"(target), "r"(distance));
        if (max_distance < distance[0]) {
            return 0;
        }
    }
    if (yaw_range != 0.0f) {
        angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
        angle += 3.1415927f;
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        angle -= entity->model->unkD0[1];
        angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        diff = angle;
        magnitude = 0.0f <= diff ? diff : -diff;
        if (yaw_range * 0.5f < magnitude) {
            return 0;
        }
    }
    if (pitch_range != 0.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%1)\n"
            "lqc2 $vf17, 0(%2)\n"
            "vsub.xyz $vf16, $vf16, $vf17\n"
            "vmul.xz $vf16, $vf16, $vf16\n"
            "vaddz.x $vf16, $vf16, $vf16z\n"
            "vsqrt Q, $vf16x\n"
            "vwaitq\n"
            "vaddq.x $vf16, $vf0, Q\n"
            "sqc2 $vf16, 0(%3)\n"
            : "=m"(distance) : "r"(entity->model->position), "r"(target), "r"(distance));
        diff = atan2f(target[1] - entity->model->position[1], distance[0]);
        diff -= entity->model->unkD0[0];
        diff = diff - (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
        if (3.1415927f < diff) {
            diff -= 6.2831855f;
        } else if (diff < -3.1415927f) {
            diff += 6.2831855f;
        }
        magnitude = 0.0f <= diff ? diff : -diff;
        if (pitch_range * 0.5f < magnitude) {
            return 0;
        }
    }
    return 1;
}
