#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* func_00194FB8, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* whether the player is within the height, angle and distance reach of ranged attack record index */
s32 func_001992F0(Entity *entity, s32 index) {
    EntityParams *params = entity->params;
    EntityModel *model = entity->model;
    f32 angle;
    f32 yaw;
    f32 diff;
    f32 abs_diff;
    f32 height;
    f32 bottom;
    f32 top;
    f32 farthest;
    f32 reach[2];

    angle = ENTITY_PARAMS_RANGED(params)[index].unk0E * 0.017453292f;
    if (!(model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    yaw = wrap_angle(model->unkD0[1] + angle);
    diff = wrap_angle(entity->unk68 - yaw);
    abs_diff = ABS(diff);

    height = entity->height;
    bottom = model->position[1] - (height + params->unk1A) * 0.5f;
    top = model->position[1] - (height - params->unk1A) * 0.5f;

    farthest = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        farthest *= 0.5f;
    }
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
        farthest *= 0.5f;
    }

    reach[0] = ENTITY_PARAMS_RANGED(params)[index].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[index].reach_min : 0.0f;
    reach[1] = (farthest < ENTITY_PARAMS_RANGED(params)[index].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[index].reach_max + entity->radius) + 100.0f;

    if (bottom <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= top && abs_diff < ENTITY_PARAMS_RANGED(params)[index].unk08 &&
        reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[1]) {
        return TRUE;
    }
    return FALSE;
}
