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

/* the angle between the direction to the player and an attack's direction, `degrees` off the heading */
static inline f32 attack_angle(EntityModel *body, EntityModel *model, f32 heading_to_player, s16 degrees) {
    f32 angle = degrees * 0.017453292f;

    if (!(model->unkE0[0] >= 0.0f)) {
        angle = -angle;
    }
    return wrap_angle(heading_to_player - wrap_angle(body->unkD0[1] + angle));
}

/* picks which of the three attacks (EntityParams.unk240) to use: a weighted draw among those the
   player is in reach of. Sets flag bits 58-59 to it, clears bit 57, and returns it. */
// match: returns the bits it set (the callers declare it void and drop the value)
s32 func_0018F648(Entity *entity) {
    s32 weights[3];
    f32 reach[2];
    f32 angle;
    f32 farthest;
    f32 nearest;
    f32 top;
    f32 bottom;
    s32 weight;
    s32 total;
    s32 choice;
    s32 in_reach;
    s32 roll;
    s32 sum;
    s32 i;
    EntityModel *model;
    f32 radius;
    f32 height;

    choice = 0;
    total = 0;
    for (i = 0; i < 3; i++) {
        // match: these copies, next to the fields spelled out below, give the loop its hoisted registers
        // (two each for the model and the radius, the second a copy of the first) in retail's order
        model = entity->model;
        height = entity->height;
        radius = entity->radius;
        angle = attack_angle(model, entity->model, entity->unk68, entity->params->unk240[i].unk0E);
        angle = ABS(angle);
        farthest = entity->params->unk18;
        bottom = D_00413F80.position.y - 230.0f;
        top = D_00413F80.position.y + height;
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
        farthest = (farthest < entity->params->unk240[i].unk06 ? farthest + radius
                                                                                               : entity->params->unk240[i].unk06 + radius) + 100.0f;
        reach[1] = farthest;
        in_reach = FALSE;
        if (bottom <= entity->model->position[1] && entity->model->position[1] <= top &&
            angle < entity->params->unk240[i].unk08 && reach[0] <= entity->distance_to_player &&
            entity->distance_to_player < farthest) {
            in_reach = TRUE;
        }
        if (!in_reach) {
            weight = 0;
        } else {
            weight = (entity->unkC0[i] * entity->params->unk240[i].unk00) >> 6;
        }
        weights[i] = weight;
        total += weight;
    }
    if (total == 0) {
        total++;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    ENTITY_FLAG_BITS(entity)->bits58 = choice;
    ENTITY_FLAG_BITS(entity)->bit57 = FALSE;
    func_00190B28(entity);
    return ENTITY_FLAG_BITS(entity)->bits58;
}
