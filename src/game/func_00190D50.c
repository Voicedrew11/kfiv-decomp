#include "kf_entity.h"

float atan2f(float, float);

/* Reports whether the entity must turn (0x100 left, 0x200 right) or tilt (0x10, 0x20) toward its target and whether it is farther than the range (1, plus 0x40 if asked). */
s32 func_00190D50(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 *target;
    f32 angle;
    f32 diff;
    f32 distance[4];

    result = 0;
    target = entity->target;
    if (target == NULL) {
        return 0;
    }
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
    if (entity->params->turn_limit < (0.0f <= diff ? diff : -diff)) {
        result = 0.0f <= angle ? 0x200 : 0x100;
    }
    diff = ((f32 *)entity->target)[1] - entity->model->position[1];
    if (entity->params->unk54 < (0.0f <= diff ? diff : -diff)) {
        result |= 0.0f <= diff ? 0x20 : 0x10;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(entity->model->position), "r"(entity->target), "r"(distance) : "memory");
    if (range < distance[0]) {
        result |= 1;
        if (mark_far != 0) {
            result = (result | 0x40) & 0xFFFF;
        }
    }
    return result;
}
