#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Whether the player is within the vertical, horizontal and angular reach of attack record index, facing yaw. */
int func_001995A0(Entity *entity, int index, float yaw) {
    EntityParams *params;
    EntityModel *model;
    float angle;
    float diff;
    float abs_diff;
    float low;
    float high;
    float reach;
    float window[2];

    params = entity->params;
    angle = params->unk3D8[index].unk0E * 0.017453292f;
    yaw += angle;
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
    model = entity->model;
    high = model->position[1] - (entity->height + params->unk1A) * 0.5f;
    low = model->position[1] - (entity->height - params->unk1A) * 0.5f;
    reach = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        reach *= 0.5f;
    }
    window[0] = params->unk3D8[index].reach_min != 0 ? entity->radius + 100.0f + params->unk3D8[index].reach_min : 0.0f;
    window[1] = (reach < params->unk3D8[index].reach_max ? reach : params->unk3D8[index].reach_max) + entity->radius + 100.0f;
    if (high <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= low && abs_diff < params->unk3D8[index].unk08) {
        if (window[0] <= entity->distance_to_player && entity->distance_to_player < window[1]) {
            return 1;
        }
    }
    return 0;
}
