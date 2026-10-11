#include "kf_entity.h"

float func_001954A8(int, int *);
float func_001957B8(float, float);

s32 func_0017EA98(Entity *entity) {
    s32 result;
    f32 angle;
    f32 limit;

    if (entity->unk5C == 0) {
        return 0;
    }
    result = 1;
    angle = func_001954A8((int)entity->model->position, (int *)entity->unk5C);
    angle = func_001957B8(entity->model->unkD0[1], angle);
    limit = entity->params->unk5C;
    if ((0.0f <= angle) ? (limit < angle) : (limit < -angle)) {
        if (0.0f <= angle) {
            result = 0x201;
        } else {
            result = 0x101;
        }
    }
    return result;
}
