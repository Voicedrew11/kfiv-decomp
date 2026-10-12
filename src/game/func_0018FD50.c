#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern u32 D_00446ED4;

int rand(void);
int func_0018CA50(int *);

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

/* how much weaker than 0x80 something is `elapsed` frames into a fade of `duration` frames */
static inline s32 fade_out(s32 elapsed, s32 duration) {
    return 0x80 - elapsed * (elapsed << 7) / (duration * duration);
}

/* whether the player is in reach of one of the six attacks */
static inline s32 player_in_reach(Entity *entity, EntityParams *params) {
    f32 angles[6];
    f32 reach[2][6];
    f32 angle;
    f32 height;
    f32 farthest;
    f32 top;
    f32 bottom;

    angle = params->unk240[0].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[0] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[0] = ABS(angles[0]);
    angle = params->unk240[1].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[1] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[1] = ABS(angles[1]);
    angle = params->unk240[2].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[2] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[2] = ABS(angles[2]);
    angle = ENTITY_PARAMS_RANGED(params)[0].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[3] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[3] = ABS(angles[3]);
    angle = ENTITY_PARAMS_RANGED(params)[1].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[4] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[4] = ABS(angles[4]);
    angle = ENTITY_PARAMS_RANGED(params)[2].unk0E * 0.017453292f;
    if (!(entity->model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    angles[5] = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
    angles[5] = ABS(angles[5]);
    height = entity->height;
    bottom = entity->model->position[1] - (height + params->unk1A) * 0.5f;
    top = entity->model->position[1] - (height - params->unk1A) * 0.5f;
    farthest = params->unk18;
    if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
        farthest *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        farthest *= 0.5f;
    }
    reach[0][0] = params->unk240[0].unk04 != 0 ? entity->radius + 100.0f + params->unk240[0].unk04 : 0.0f;
    reach[0][1] = params->unk240[1].unk04 != 0 ? entity->radius + 100.0f + params->unk240[1].unk04 : 0.0f;
    reach[0][2] = params->unk240[2].unk04 != 0 ? entity->radius + 100.0f + params->unk240[2].unk04 : 0.0f;
    reach[0][3] = ENTITY_PARAMS_RANGED(params)[0].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[0].reach_min : 0.0f;
    reach[0][4] = ENTITY_PARAMS_RANGED(params)[1].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[1].reach_min : 0.0f;
    reach[0][5] = ENTITY_PARAMS_RANGED(params)[2].reach_min != 0 ? entity->radius + 100.0f + ENTITY_PARAMS_RANGED(params)[2].reach_min : 0.0f;
    reach[1][0] = (farthest < params->unk240[0].unk06 ? farthest + entity->radius : params->unk240[0].unk06 + entity->radius) + 100.0f;
    reach[1][1] = (farthest < params->unk240[1].unk06 ? farthest + entity->radius : params->unk240[1].unk06 + entity->radius) + 100.0f;
    reach[1][2] = (farthest < params->unk240[2].unk06 ? farthest + entity->radius : params->unk240[2].unk06 + entity->radius) + 100.0f;
    reach[1][3] = (farthest < ENTITY_PARAMS_RANGED(params)[0].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[0].reach_max + entity->radius) + 100.0f;
    reach[1][4] = (farthest < ENTITY_PARAMS_RANGED(params)[1].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[1].reach_max + entity->radius) + 100.0f;
    reach[1][5] = (farthest < ENTITY_PARAMS_RANGED(params)[2].reach_max ? farthest + entity->radius : ENTITY_PARAMS_RANGED(params)[2].reach_max + entity->radius) + 100.0f;
    if (entity->model->position[1] - height <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= entity->model->position[1]) {
        if (params->unk240[0].unk00 != 0 && angles[0] < params->unk240[0].unk08 && reach[0][0] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][0]) {
            return TRUE;
        }
        if (params->unk240[1].unk00 != 0 && angles[1] < params->unk240[1].unk08 && reach[0][1] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][1]) {
            return TRUE;
        }
        if (params->unk240[2].unk00 != 0 && angles[2] < params->unk240[2].unk08 && reach[0][2] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][2]) {
            return TRUE;
        }
    }
    if (bottom <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= top) {
        if (ENTITY_PARAMS_RANGED(params)[0].record.unk00 != 0 && angles[3] < ENTITY_PARAMS_RANGED(params)[0].unk08 && reach[0][3] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][3]) {
            return TRUE;
        }
        if (ENTITY_PARAMS_RANGED(params)[1].record.unk00 != 0 && angles[4] < ENTITY_PARAMS_RANGED(params)[1].unk08 && reach[0][4] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][4]) {
            return TRUE;
        }
        if (ENTITY_PARAMS_RANGED(params)[2].record.unk00 != 0 && angles[5] < ENTITY_PARAMS_RANGED(params)[2].unk08 && reach[0][5] <= entity->distance_to_player &&
            entity->distance_to_player < reach[1][5]) {
            return TRUE;
        }
    }
    return FALSE;
}

/* picks the next of the four actions (EntityParams.unk14): a weighted draw, -1 when none can be
   chosen. Action 0, the attack, is left out while unkA2 is set, while the player is in reach of
   none of the six attacks, and depending on `mode`. */
s32 func_0018FD50(Entity *entity, s32 mode) {
    s32 weights[4];
    s32 chances[4];
    EntityParams *params = entity->params;
    s32 choice;
    s32 total;
    s32 elapsed;
    s32 fade;
    s32 roll;
    s32 sum;
    s32 i;

    choice = -1;
    for (i = 0; i < 4; i++) {
        chances[i] = params->unk14[i];
    }
    if (entity->unkA2 != 0) {
        chances[0] = 0;
    } else if (!player_in_reach(entity, params)) {
        chances[0] = 0;
    }
    if (mode == 0) {
        if ((s32)(entity->flags >> 63) || func_0018CA50((int *)entity)) {
            chances[0] = 0;
        }
    } else if (mode == 1) {
        if (func_0018CA50((int *)entity)) {
            chances[0] = 0;
        }
    } else if (mode == 2) {
        if ((s32)(entity->flags >> 63)) {
            chances[0] = 0;
        }
    }
    total = 0;
    if (chances[0] + chances[1] + chances[2] + chances[3] == 0) {
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (i == entity->unkCD) {
            elapsed = (u16)D_00446ED4 - entity->unk9E;
            if (elapsed < 240) {
                fade = fade_out(elapsed, 240);
                fade += 0x80;
            } else {
                fade = 0x80;
            }
        } else {
            fade = 0x80;
        }
        weights[i] = (fade * ((entity->unkBC[i] * chances[i]) >> 6)) >> 7;
        total += weights[i];
    }
    if (total == 0) {
        return -1;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 4; i++) {
        sum += weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    return choice;
}
