#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Whether the player's distance falls inside the window of any of the three attack records. */
int func_00198998(Entity *entity) {
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
    window[0] = entity->params->unk240[0].unk04 != 0 ? entity->radius + 100.0f + entity->params->unk240[0].unk04 : 0.0f;
    window[1] = entity->params->unk240[1].unk04 != 0 ? entity->radius + 100.0f + entity->params->unk240[1].unk04 : 0.0f;
    window[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    window[3] = (reach < entity->params->unk240[0].unk06 ? reach : entity->params->unk240[0].unk06) + entity->radius + 100.0f;
    window[4] = (reach < entity->params->unk240[1].unk06 ? reach : entity->params->unk240[1].unk06) + entity->radius + 100.0f;
    window[5] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, 2).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, 2).unk06) + entity->radius + 100.0f;
    if ((params->unk240[0].unk00 != 0 && window[0] <= entity->distance_to_player && entity->distance_to_player < window[3]) ||
        (params->unk240[1].unk00 != 0 && window[1] <= entity->distance_to_player && entity->distance_to_player < window[4]) ||
        (ENTITY_PARAMS_UNK240_RECORD(params, 2).unk00 != 0 && window[2] <= entity->distance_to_player && entity->distance_to_player < window[5])) {
        return 1;
    }
    return 0;
}
