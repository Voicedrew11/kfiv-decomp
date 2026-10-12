#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Whether the player is within the vertical, horizontal and angular reach of attack record index. */
int func_001984B0(Entity *entity, int index) {
    EntityParams *params;
    EntityModel *model;
    float angle;
    EntityParamsUnk240 *record;
    float yaw;
    float diff;
    float abs_diff;
    float low;
    float high;
    float reach;
    float window[2];

    params = entity->params;
    model = entity->model;
    record = &ENTITY_PARAMS_UNK240_RECORD(params, index);
    angle = record->unk0E * 0.017453292f;
    if (!(model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = model->unkD0[1] + angle;
    yaw -= (float)(int)(yaw / 6.2831855f) * 6.2831855f;
    if (3.1415927f < yaw) {
        yaw -= 6.2831855f;
    } else if (yaw < -3.1415927f) {
        yaw += 6.2831855f;
    }
    diff = entity->unk68 - yaw;
    diff -= (float)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    abs_diff = diff;
    if (!(abs_diff >= 0.0f)) {
        abs_diff = -abs_diff;
    }
    low = D_00413F80.position.y - 230.0f;
    high = D_00413F80.position.y + entity->height;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        reach *= 0.5f;
    }
    window[0] = ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_UNK240_RECORD(params, index).unk04 : 0.0f;
    window[1] = (reach < ENTITY_PARAMS_UNK240_RECORD(params, index).unk06 ? reach : ENTITY_PARAMS_UNK240_RECORD(params, index).unk06) + entity->radius + 100.0f;
    if (low <= model->position[1] && model->position[1] <= high && abs_diff < ENTITY_PARAMS_UNK240_RECORD(params, index).unk08) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}
