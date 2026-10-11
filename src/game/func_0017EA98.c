#include "kf_entity.h"

float func_001954A8(float *, float *);
float func_001957B8(float, float);

s32 func_0017EA98(Entity *entity) {
    s32 result;
    f32 angle;
    f32 limit;

    if (entity->target == NULL) {
        return 0;
    }
    result = 1;
    angle = func_001954A8(entity->model->position, entity->target);
    angle = func_001957B8(entity->model->unkD0[1], angle);
    limit = entity->params->turn_limit;
    if ((0.0f <= angle) ? (limit < angle) : (limit < -angle)) {
        if (0.0f <= angle) {
            result = 0x201;
        } else {
            result = 0x101;
        }
    }
    return result;
}
