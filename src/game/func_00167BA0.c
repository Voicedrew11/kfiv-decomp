#include "kf_entity.h"

float func_00194FB8(float);
int func_00198730(void *, float, int);
int func_001995A0(void *, float, int);

s32 func_00167BA0(Entity *entity) {
    f32 angle = func_00194FB8(entity->model->unkD0[1] + entity->unkE0.unk48[0]);

    if (func_00198730(entity, angle, 0)) {
        return TRUE;
    }
    if (func_00198730(entity, angle, 1)) {
        return TRUE;
    }
    if (func_00198730(entity, angle, 2)) {
        return TRUE;
    }
    if (func_001995A0(entity, angle, 0)) {
        return TRUE;
    }
    if (func_001995A0(entity, angle, 1)) {
        return TRUE;
    }
    return func_001995A0(entity, angle, 2) != 0;
}
