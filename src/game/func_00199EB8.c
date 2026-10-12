#include "kf_entity.h"

/* Turns the entity heading toward target at a speed that builds up to the limit; returns 1 once it arrives. */
s32 func_00199EB8(Entity *entity, f32 *speed, f32 target) {
    f32 diff;

    target -= (int)(target / 6.2831855f) * 6.2831855f;
    if (3.1415927f < target) {
        target -= 6.2831855f;
    } else if (target < -3.1415927f) {
        target += 6.2831855f;
    }
    if (entity->model->unkD0[1] == target) {
        *speed = 0.0f;
        return 1;
    }
    diff = target - entity->model->unkD0[1];
    diff -= (int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (diff > 0.0f) {
        *speed += entity->params->unk58;
        if (*speed > entity->params->turn_limit) {
            *speed = entity->params->turn_limit;
        }
        if (diff <= *speed) {
            entity->unk94 += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->unk94 += *speed;
    } else if (diff < 0.0f) {
        *speed -= entity->params->unk58;
        if (*speed < -entity->params->turn_limit) {
            *speed = -entity->params->turn_limit;
        }
        if (*speed <= diff) {
            entity->unk94 += diff;
            *speed = 0.0f;
            return 1;
        }
        entity->unk94 += *speed;
    } else {
        *speed = 0.0f;
        return 1;
    }
    return 0;
}
