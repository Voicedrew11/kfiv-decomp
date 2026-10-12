#include "kf_entity.h"

extern PlayerData D_00413F80;

float func_001954A8(float *, float *);
float func_001957B8(float, float);
float func_00194F80(void *, void *);

s32 func_00180CA0(Entity *entity, s32 turn, f32 near_range, f32 far_range) {
    f32 angle;
    f32 turn_limit;
    f32 distance;
    s32 command;

    command = 0;
    if (entity->target == NULL) {
        return 0;
    }
    if (turn == 0) {
        angle = func_001954A8(entity->model->position, entity->target);
        angle = func_001957B8(entity->model->unkD0[1], angle);
        turn_limit = entity->params->turn_limit;
        if (0.0f <= angle ? turn_limit < angle : turn_limit < -angle) {
            command = 0x100;
            if (0.0f <= angle) {
                command = 0x200;
            }
        }
    } else {
        if (turn > 0) {
            command |= 0x200;
        } else {
            command |= 0x100;
        }
    }
    distance = func_00194F80(entity->model->position, &D_00413F80);
    if (distance < near_range + 100.0f + entity->radius) {
        command |= 2;
    } else if (far_range + 100.0f + entity->radius < distance) {
        command |= 1;
    }
    return command;
}
