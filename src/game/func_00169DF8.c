#include "kf_entity.h"

extern PlayerData D_00413F80;

float func_00195478(float *, char *);
float func_001957B8(float, float);
int rand(void);

s32 func_00169DF8(Entity *entity, f32 min_gap, f32 max_angle) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 gap = entity->distance_to_player - (entity->radius + 100.0f);
    f32 angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    f32 facing = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));

    if (entity->unkC6 != 0 || gap < min_gap || max_angle < ABS(angle) || 0.7853982f < ABS(facing) || unkE0->unk7C <= 0) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB;
}
