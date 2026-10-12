#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Whether the entity is within the reach window; window[1] is the shorter of the two reaches plus radius of attack record index. */
int func_00198BD8(Entity *entity, int index) {
    EntityParams *params;
    float reach;
    float window[2];
    float distance;
    int in_range;

    params = entity->params;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.unk45 == 0x9C) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).unk06) + entity->radius + 100.0f;
    distance = entity->distance_to_player;
    in_range = 0;
    if (window[0] <= distance && distance < window[1]) {
        in_range = 1;
    }
    return in_range;
}
