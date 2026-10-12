#include "kf_entity.h"

float atan2f(float, float);

/* returns 0x100 or 0x200 when the target is outside the turn limit to one side or the other */
int func_00190BE0(Entity *entity) {
    int result = 0;
    f32 *target = entity->target;
    f32 angle;
    f32 diff;

    if (target == NULL) {
        return 0;
    }
    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    diff = angle - entity->model->unkD0[1];
    diff -= (f32)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (entity->params->turn_limit < ABS(diff)) {
        result = 0x100;
        if (0.0f <= diff) {
            result = 0x200;
        }
    }
    return result;
}
