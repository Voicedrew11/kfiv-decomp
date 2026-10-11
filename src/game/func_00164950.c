#include "kf_entity.h"

int rand(void);
float func_001957B8(float, float);

s32 func_00164950(Entity *entity, f32 min_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    if (unkE0->unk6C <= 0) {
        return FALSE;
    }
    if (unkE0->unk6A > 0) {
        return FALSE;
    }
    distance = entity->distance_to_player - (entity->unk38 + 100.0f);
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (distance < min_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB / 5;
}
