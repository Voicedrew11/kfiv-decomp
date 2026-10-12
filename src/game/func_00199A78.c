#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Returns whether the player's distance lies within the reach of attack record index. */
int func_00199A78(Entity *entity, int index) {
    EntityParams *params = entity->params;
    f32 reach = params->unk18;
    f32 limits[2];
    f32 near_limit;

    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        reach *= 0.5f;
    }
    if (params->unk3D8[index].reach_min != 0) {
        near_limit = entity->radius + 100.0f + params->unk3D8[index].reach_min;
    } else {
        near_limit = 0.0f;
    }
    limits[0] = near_limit;
    limits[1] = ((reach < params->unk3D8[index].reach_max) ? reach : params->unk3D8[index].reach_max) + entity->radius + 100.0f;
    if (limits[0] <= entity->distance_to_player && entity->distance_to_player < limits[1]) {
        return 1;
    }
    return 0;
}
