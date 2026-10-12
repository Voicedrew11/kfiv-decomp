#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Reports whether the player is at a distance one of the entity's six attacks is used at (three melee, three ranged). */
s32 func_00197B60(Entity *entity) {
    f32 reach[12]; /* nearest, then farthest, per attack */
    f32 range;
    f32 distance;

    range = entity->params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        range *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        range *= 0.5f;
    }
    reach[0] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk04 : 0.0f;
    reach[1] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk04 : 0.0f;
    reach[2] = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk04 : 0.0f;
    reach[3] = entity->params->unk3D8[0].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[0].reach_min : 0.0f;
    reach[4] = entity->params->unk3D8[1].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[1].reach_min : 0.0f;
    reach[5] = entity->params->unk3D8[2].reach_min != 0 ? entity->radius + 100.0f + entity->params->unk3D8[2].reach_min : 0.0f;
    reach[6] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06) + entity->radius + 100.0f;
    reach[7] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06) + entity->radius + 100.0f;
    reach[8] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06) + entity->radius + 100.0f;
    reach[9] = (range < entity->params->unk3D8[0].reach_max ? range : entity->params->unk3D8[0].reach_max) + entity->radius + 100.0f;
    reach[10] = (range < entity->params->unk3D8[1].reach_max ? range : entity->params->unk3D8[1].reach_max) + entity->radius + 100.0f;
    reach[11] = (range < entity->params->unk3D8[2].reach_max ? range : entity->params->unk3D8[2].reach_max) + entity->radius + 100.0f;
    distance = entity->distance_to_player;
    if ((reach[0] <= distance && distance < reach[6]) ||
        (reach[1] <= distance && distance < reach[7]) ||
        (reach[2] <= distance && distance < reach[8]) ||
        (reach[3] <= distance && distance < reach[9]) ||
        (reach[4] <= distance && distance < reach[10]) ||
        (reach[5] <= distance && distance < reach[11])) {
        return TRUE;
    }
    return FALSE;
}
