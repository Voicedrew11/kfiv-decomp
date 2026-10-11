#include "kf_entity.h"

extern u8 D_004141D8[];
extern EntityModel *D_00446978;

int rand(void);
float func_001957B8(float, float);

s32 func_0015C688(Entity *entity, f32 max_distance, f32 max_angle) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 distance;
    f32 angle;

    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (distance > max_distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    return rand() % 100 < unkE0->unk6C / 5;
}
