#include "kf_entity.h"

extern u8 D_004141D8[];
extern EntityModel *D_00446978;

int rand(void);

/* whether to react (chance params->unkCB / 5 percent) while the player, close enough and in front, is early in a swing */
s32 func_0018ED98(Entity *entity, f32 max_distance, f32 max_angle) {
    f32 angle = entity->unk68 - entity->model->unkD0[1];
    f32 distance = entity->distance_to_player - (entity->radius + 100.0f);

    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB / 5;
}
