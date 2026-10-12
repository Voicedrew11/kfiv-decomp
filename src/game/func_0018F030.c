#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

int rand(void);
void func_00190B28(Entity *);

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

/* picks which of the six attacks (three melee, EntityParams.unk240, then three ranged) to use: a weighted
   draw among those the player is in reach of. Sets flag bits 58-59 to its number and bit 57 for a ranged
   one, and returns bit 57. */
s32 func_0018F030(Entity *entity) {
    s32 weights[6];
    f32 reach[2];
    s32 total;
    s32 choice;
    s32 roll;
    s32 sum;
    s32 weight;
    s32 i;

    choice = 0;
    total = 0;
    for (i = 0; i < 6; i++) {
        if (i < 3) {
            f32 angle;
            f32 difference;
            f32 farthest;
            f32 nearest;
            f32 radius;
            f32 top;
            f32 bottom;
            s32 in_reach;

            angle = entity->params->unk240[i].unk0E * 0.017453292f;
            if (!(entity->model->unkE0[0] >= 0.0f)) {
                angle = -angle;
            }
            difference = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
            difference = ABS(difference);
            farthest = entity->params->unk18;
            bottom = D_00413F80.position.y - 230.0f;
            top = D_00413F80.position.y + entity->height;
            if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
                farthest *= 0.5f;
            }
            if (D_00413F80.equipment[7] == 0x9C) {
                farthest *= 0.5f;
            }
            if (entity->params->unk240[i].unk04 != 0) {
                nearest = entity->radius + 100.0f + entity->params->unk240[i].unk04;
            } else {
                nearest = 0.0f;
            }
            reach[0] = nearest;
            radius = entity->radius; // match: the far limit reads the radius through a local, the near one directly
            reach[1] = (farthest < entity->params->unk240[i].unk06 ? farthest + radius : entity->params->unk240[i].unk06 + radius) + 100.0f;
            in_reach = FALSE;
            if (bottom <= entity->model->position[1] && entity->model->position[1] <= top && difference < entity->params->unk240[i].unk08 &&
                reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[1]) {
                in_reach = TRUE;
            }
            if (!in_reach) {
                weight = 0;
            } else {
                weight = (entity->unkC0[i] * entity->params->unk240[i].unk00) >> 6;
            }
        } else {
            f32 angle;
            f32 difference;
            f32 farthest;
            f32 nearest;
            f32 radius;
            f32 top;
            f32 bottom;
            s32 in_reach;
            s32 j;

            j = i - 3;
            angle = ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].unk0E * 0.017453292f;
            if (!(entity->model->unkE0[0] >= 0.0f)) {
                angle = -angle;
            }
            difference = wrap_angle(entity->unk68 - wrap_angle(entity->model->unkD0[1] + angle));
            difference = ABS(difference);
            farthest = entity->params->unk18;
            bottom = entity->model->position[1] - (entity->height + entity->params->unk1A) * 0.5f;
            top = entity->model->position[1] - (entity->height - entity->params->unk1A) * 0.5f;
            if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
                farthest *= 0.5f;
            }
            if (D_00413F80.equipment[7] == 0x9C) {
                farthest *= 0.5f;
            }
            if (ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].reach_min != 0) {
                nearest = entity->radius + 100.0f + ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].reach_min;
            } else {
                nearest = 0.0f;
            }
            reach[0] = nearest;
            radius = entity->radius; // match: as in the melee half
            reach[1] = (farthest < ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].reach_max ? farthest + radius : ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].reach_max + radius) + 100.0f;
            in_reach = FALSE;
            if (bottom <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= top && difference < ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].unk08 &&
                reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[1]) {
                in_reach = TRUE;
            }
            weight = 0;
            if (in_reach) {
                weight = (entity->unkC0[i] * ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[j].record.unk00) >> 6;
            }
        }
        weights[i] = weight;
        total += weight;
    }
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 6; i++) {
        sum += weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice < 3) {
        ENTITY_FLAG_BITS(entity)->bits58 = choice;
        ENTITY_FLAG_BITS(entity)->bit57 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bits58 = choice - 3;
        ENTITY_FLAG_BITS(entity)->bit57 = TRUE;
    }
    func_00190B28(entity);
    return (s32)(entity->flags >> 57) & 1;
}
