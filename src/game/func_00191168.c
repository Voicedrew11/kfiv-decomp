#include "kf_entity.h"

float atan2f(float, float);

/* Checks whether the entity's target is within turn and reach: 0x100/0x200 when it must turn left/right, 1 or 0x41 when it is in reach. */
s32 func_00191168(Entity *entity, s32 far_flag, f32 reach) {
    u16 result = 0;
    f32 *target_pos;
    f32 angle;
    f32 distance;
    f32 turn_limit;
    f32 vec[4];

    if (entity->target == NULL) {
        return 0;
    }
    target_pos = entity->target;
    angle = atan2f(target_pos[0] - entity->model->position[0], target_pos[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle -= entity->model->unkD0[1];
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    turn_limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? turn_limit < angle : turn_limit < -angle) {
        if (0.0f <= angle) {
            result = 0x200;
        } else {
            result = 0x100;
        }
    } else {
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
            : : "r"(entity->model->position), "r"(entity->target), "r"(vec) : "memory");
        distance = vec[0];
        if (reach < distance) {
            result |= 1;
            if (far_flag != 0) {
                result |= 0x40;
            }
        }
    }
    return result;
}
