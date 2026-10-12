#include "common.h"
#include "kf_entity.h"

float atan2f(float, float);

/* Turns the entity's heading toward a point, speeding the turn rate up to the limit; returns 1 once facing it. */
int func_0019A050(Entity *entity, f32 *turn_rate, f32 *target) {
    f32 angle;
    f32 diff;

    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->model->unkD0[1] == angle) {
        *turn_rate = 0.0f;
        return 1;
    }
    diff = angle - entity->model->unkD0[1];
    diff -= (f32)(int)(diff / 6.2831855f) * 6.2831855f;
    if (diff > 3.1415927f) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (diff > 0.0f) {
        *turn_rate += entity->params->unk58;
        if (*turn_rate > entity->params->turn_limit) {
            *turn_rate = entity->params->turn_limit;
        }
        if (diff <= *turn_rate) {
            entity->unk94 += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->unk94 += *turn_rate;
    } else if (diff < 0.0f) {
        *turn_rate -= entity->params->unk58;
        if (*turn_rate < -entity->params->turn_limit) {
            *turn_rate = -entity->params->turn_limit;
        }
        if (*turn_rate <= diff) {
            entity->unk94 += diff;
            *turn_rate = 0.0f;
            return 1;
        }
        entity->unk94 += *turn_rate;
    } else {
        *turn_rate = 0.0f;
        return 1;
    }
    return 0;
}
