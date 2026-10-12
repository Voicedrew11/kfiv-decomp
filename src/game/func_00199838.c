#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Whether the player's distance falls inside the window of any of the three attack records. */
int func_00199838(Entity *entity) {
    EntityParams *params;
    float reach;
    float window[6];

    params = entity->params;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        reach *= 0.5f;
    }
    // match: the window bounds re-read entity->params, which keeps a second copy of the pointer
    window[0] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    window[1] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    window[2] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    window[3] = (reach < entity->params->unk3D8[0].reach_max ? reach : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->unk3D8[1].reach_max ? reach : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    window[5] = (reach < params->unk3D8[2].reach_max ? reach : params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    if ((params->unk3D8[0].record.unk00 != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->unk3D8[1].record.unk00 != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (params->unk3D8[2].record.unk00 != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
        return 1;
    }
    return 0;
}
