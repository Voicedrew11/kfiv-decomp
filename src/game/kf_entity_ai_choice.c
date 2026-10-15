/* The choices of the shared entity AI: what the entity knows of the player (Entity.state), guarding and
   dodging, which attack and which combat choice to take and how their weights change, and the movement
   commands (Entity.unkC8) that take it towards its target or aside from a push. */
#include "kf_entity_ai_choice.h"
#include "kf_entity_aim.h"
#include "kf_player.h"

float sinf(float);
float cosf(float);
double acos(double);
void Vector_Normalize(void *dst, void *src);
void Vector_Cross(void *, void *, void *);
extern u8 D_004141D8[];
extern EntityModel *D_00446978;
int rand(void);
extern PlayerState D_00413F80;
extern u32 D_00446ED4;
float atan2f(float, float);

/* EntityMath_WrapAngle, which the game inlines here: wraps an angle to +-pi */
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
    if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
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

/* Moves *level by delta, keeping it between 0x40 and max.
   match: max is a parameter so the 0xC0 cap stays in a register (slt/movn), as in EntityAi_RaiseAttackWeight */
static inline void adjust_level(u8 *level, s32 delta, s32 max) {
    s32 value = *level + delta;

    if (value < 0x40) {
        value = 0x40;
    }
    if (value > max) {
        value = max;
    }
    *level = value;
}

/* Steps Entity.state from flag bit 33 (the player is noticed): 0 nothing, 2 noticed, 1 was noticed and
   Entity.timer (EntityController.unk12 frames) still runs; 3 stays. Clears unkA0 when the state changes. */
void EntityAi_StepState(Entity *entity) {
    s64 next;

    next = entity->state;
    entity->prev_state = next;
    switch ((char)next) {
    case 0:
        if (((int)((u64)entity->flags >> 33) & 1)) {
            next = 2;
            entity->state = 2;
        }
        break;
    case 1:
        if (((int)((u64)entity->flags >> 33) & 1)) {
            next = 2;
            entity->state = 2;
        } else if (entity->timer == 0) {
            entity->state = 0;
            next = 0;
        }
        entity->timer--;
        break;
    case 2:
        if (!(((int)((u64)entity->flags >> 33) & 1))) {
            entity->state = 1;
            next = 1;
            entity->timer = ENTITY_CONTROLLER(entity)->unk12;
        }
        break;
    case 3:
        entity->state = 3;
        next = 3;
        break;
    default:
        entity->state = 0;
        next = 0;
        break;
    }
    if ((char)next != entity->prev_state) {
        entity->unkA0 = 0;
    }
}

/* While the entity is pushed (flags 45, 46, 48) from the front: the heading that turns it a degree past side
   on to the push, written to `heading_out`. Returns the side it turns to (1, -1), or 0 when there is nothing
   to do. */
int EntityAi_TurnFromPush(void *entity_arg, void *heading_out) {
    Entity *entity = entity_arg;
    f32 facing[4];
    f32 push[4];
    f32 angle;

    *(f32 *)heading_out = 0.0f;
    if (!(entity->flags & (0xB000L << 33))) {
        return 0;
    }
    angle = entity->model->unkD0[1];
    facing[0] = -sinf(angle);
    facing[1] = 0.0f;
    facing[2] = -cosf(angle);
    facing[3] = 1.0f;
    push[0] = push[1] = push[2] = push[3] = 0.0f;
    if (entity->flags & (0xC000L << 31)) {
        push[0] += entity->unkA4[0];
        push[2] += entity->unkA4[2];
    }
    if ((s32)(entity->flags >> 48) & 1) {
        push[0] += entity->unkB0[0];
        push[2] += entity->unkB0[2];
    }
    Vector_Normalize(push, push);
    angle = facing[0] * push[0] + facing[2] * push[2];
    if (angle >= 0.0f) {
        return 0;
    }
    angle = (f32)acos(angle) - 1.5707964f;
    Vector_Cross(facing, push, facing);
    if (facing[1] <= 0.0f) {
        angle = entity->model->unkD0[1] + angle + 0.017453292f;
        *(f32 *)heading_out = angle;
        return 1;
    }
    angle = entity->model->unkD0[1] - angle - 0.017453292f;
    *(f32 *)heading_out = angle;
    return -1;
}

/* Whether the push the entity is under (flags 45, 46, 48) comes against `heading` (from the front). */
s32 EntityAi_IsPushedFromFront(Entity *entity, f32 heading) {
    f32 facing[4];
    f32 push[4];

    // match: the early return and the second `return FALSE` below leave a label in front of the body
    // until jump2 merges the two, which keeps `heading` copied back into $f12 for sinf
    if (!(entity->flags & (0xB000L << 33))) {
        return FALSE;
    }
    facing[0] = -sinf(heading);
    facing[1] = 0.0f;
    facing[2] = -cosf(heading);
    facing[3] = 1.0f;
    push[0] = push[1] = push[2] = push[3] = 0.0f;
    if (entity->flags & (0xC000L << 31)) {
        push[0] += entity->unkA4[0];
        push[2] += entity->unkA4[2];
    }
    if ((s32)(entity->flags >> 48) & 1) {
        push[0] += entity->unkB0[0];
        push[2] += entity->unkB0[2];
    }
    Vector_Normalize(push, push);
    if (facing[0] * push[0] + facing[2] * push[2] >= 0.0f) {
        return FALSE;
    }
    return TRUE;
}

/* Whether to guard: by a chance of EntityParams.unkCB / 5 percent while the player, within `max_distance` and
   `max_angle` of the heading, is early in a swing, and the entity is not busy. */
s32 EntityAi_ShouldGuard(Entity *entity, f32 max_distance, f32 max_angle) {
    f32 angle = entity->unk68 - entity->model->unkD0[1];
    f32 distance = entity->distance_to_player - (entity->radius + 100.0f);

    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCB / 5;
}

/* As EntityAi_ShouldGuard, by a chance of EntityParams.unkCA / 5 percent. */
s32 EntityAi_ShouldDodge(Entity *entity, f32 max_distance, f32 max_angle) {
    f32 angle;
    f32 distance;

    if (D_004141D8[0] == 0 || D_00446978->unkFC->unk82 >= 6) {
        return FALSE;
    }
    angle = entity->unk68 - entity->model->unkD0[1];
    distance = entity->distance_to_player - (entity->radius + 100.0f);
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (entity->unkC6 != 0) {
        return FALSE;
    }
    if (max_distance < distance) {
        return FALSE;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    return rand() % 100 < entity->params->unkCA / 5;
}

/* Picks which of the six attacks (three melee, EntityParams.unk240, then three ranged) to use: a weighted
   draw among those the player is in reach of. Sets flag bits 58-59 to its number and bit 57 for a ranged one,
   and returns bit 57. */
s32 EntityAi_PickAttack(Entity *entity) {
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
            if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
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
            if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
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
    EntityAi_LowerAttackWeight(entity);
    return (s32)(entity->flags >> 57) & 1;
}

/* Picks which of the three melee attacks (EntityParams.unk240) to use: a weighted draw among those the player
   is in reach of. Sets flag bits 58-59 to it, clears bit 57, and returns it. */
// match: returns the bits it set (the callers declare it void and drop the value)
s32 EntityAi_PickMeleeAttack(Entity *entity) {
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
        if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
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
    EntityAi_LowerAttackWeight(entity);
    return ENTITY_FLAG_BITS(entity)->bits58;
}

/* Picks which of the three ranged attacks (EntityParams.unk3D8) to use: a weighted draw among those the
   player is in reach of. Sets flag bits 58-59 to it, sets bit 57, and returns it. */
// match: returns the bits it set (the callers declare it void and drop the value)
s32 EntityAi_PickRangedAttack(Entity *entity) {
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

    choice = 0;
    total = 0;
    for (i = 0; i < 3; i++) {
        // match: a second copy of the model and the radius, next to the fields spelled out below, is what
        // gives the loop its two hoisted registers for each (the second a copy of the first)
        model = entity->model;
        radius = entity->radius;
        angle = attack_angle(model, entity->model, entity->unk68, ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].unk0E);
        angle = ABS(angle);
        bottom = entity->model->position[1] - (entity->height + entity->params->unk1A) * 0.5f;
        top = entity->model->position[1] - (entity->height - entity->params->unk1A) * 0.5f;
        farthest = entity->params->unk18;
        if (entity->unk24 != 0 || (D_00413F80.unk2E4 > 0 && entity->unk60 < D_00413F80.unk2E8)) {
            farthest *= 0.5f;
        }
        if (D_00413F80.equipment[EQUIP_AMULET] == ITEM_WORN_INDEX(ITEM_THIEVES_NECKLACE)) {
            farthest *= 0.5f;
        }
        if (ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].reach_min != 0) {
            nearest = entity->radius + 100.0f + ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].reach_min;
        } else {
            nearest = 0.0f;
        }
        reach[0] = nearest;
        farthest = (farthest < ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].reach_max ? farthest + radius
                                                                                               : ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].reach_max + radius) + 100.0f;
        reach[1] = farthest;
        in_reach = FALSE;
        if (bottom <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= top &&
            angle < ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].unk08 && reach[0] <= entity->distance_to_player &&
            entity->distance_to_player < farthest) {
            in_reach = TRUE;
        }
        if (!in_reach) {
            weight = 0;
        } else {
            weight = (entity->unkC0[3 + i] * ENTITY_PARAMS_RANGED_VIEW(entity->params)->ranged[i].record.unk00) >> 6;
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
    ENTITY_FLAG_BITS(entity)->bit57 = TRUE;
    EntityAi_LowerAttackWeight(entity);
    return ENTITY_FLAG_BITS(entity)->bits58;
}

/* Picks the next of the four combat choices (Entity.unkCD; 0 attack, 1 close in, 2 wait) by their chances
   EntityParams.unk14 and weights unkBC, the current choice counting up to twice for 240 frames after it was
   taken; -1 when none can be chosen. The attack is left out while unkA2 runs, while the player is in reach of
   none of the six attacks, and by `mode`. */
s32 EntityAi_PickCombatChoice(Entity *entity, s32 mode) {
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
        if ((s32)(entity->flags >> 63) || EntitySight_EntityInWay(entity)) {
            chances[0] = 0;
        }
    } else if (mode == 1) {
        if (EntitySight_EntityInWay(entity)) {
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

/* After a hit in state 2: lowers the weight (unkBC) of the choice the entity was on and raises the other's,
   and after an attack does the same to the attack weights (unkC0; retail offsets the melee ones by 3, the
   other way round from EntityAi_LowerAttackWeight). All kept between 0x40 and 0xC0. */
void EntityAi_LowerWeightsAfterHit(Entity *entity) {
    s32 i;
    s32 first;
    u8 *weights;

    if (entity->state == 2) {
        for (i = 0; i < 2; i++) {
            if (entity->unkCD == i) {
                adjust_level(&entity->unkBC[i], -10, 0xC0);
            } else {
                adjust_level(&entity->unkBC[i], 5, 0xC0);
            }
        }
        if (entity->unkCD == 0) {
            first = 3;
            if ((entity->flags >> 57) & 1) {
                first = 0;
            }
            for (i = 0, weights = entity->unkC0; i < 6; i++) {
                if (((s32)(entity->flags >> 58) & 3) + first == i) {
                    adjust_level(&weights[i], -10, 0xC0);
                } else {
                    adjust_level(&weights[i], 5, 0xC0);
                }
            }
        }
    }
}

/* Lowers the weight (unkC0) of the attack flag bits 57-59 name by 10, kept between 0x40 and 0xC0. */
void EntityAi_LowerAttackWeight(Entity *entity) {
    EntityFlagBits *flags = ENTITY_FLAG_BITS(entity);
    u32 high_bits = entity->flags >> 57;
    s32 index;
    u8 *level;

    if (high_bits & 1) {
        index = flags->bits58 + 3;
    } else {
        index = flags->bits58;
    }
    level = entity->unkC0;
    level += index;
    adjust_level(level, -10, 0xC0);
}

/* Lowers the weight (unkBC) of the chosen of the first two combat choices by 10 and raises the other's by 5,
   each kept between 0x40 and 0xC0. */
void EntityAi_LowerChoiceWeight(Entity *entity, s32 chosen) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (i == chosen) {
            adjust_level(&entity->unkBC[i], -10, 0xC0);
        } else {
            adjust_level(&entity->unkBC[i], 5, 0xC0);
        }
    }
}

/* The turn command towards the target: 0x100 or 0x200 when it lies outside EntityParams.turn_limit to one
   side or the other, else 0. */
int EntityAi_TurnCommand(Entity *entity) {
    int result = 0;
    f32 *target = entity->target;
    f32 angle;
    f32 diff;

    if (target == NULL) {
        return 0;
    }
    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (f32)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    diff = angle - entity->model->unkD0[1];
    diff -= (f32)(int)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (entity->params->turn_limit < ABS(diff)) {
        result = 0x100;
        if (0.0f <= diff) {
            result = 0x200;
        }
    }
    return result;
}

/* The move command towards the target: turn (0x100, 0x200), up or down (0x10, 0x20) beyond
   EntityParams.unk54, and forward (1, with 0x40 when `run` is set) while it is farther than `range`. */
s32 EntityAi_MoveCommand3D(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 *target;
    f32 angle;
    f32 diff;
    f32 distance[4];

    result = 0;
    target = entity->target;
    if (target == NULL) {
        return 0;
    }
    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle -= entity->model->unkD0[1];
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    diff = angle;
    if (entity->params->turn_limit < (0.0f <= diff ? diff : -diff)) {
        result = 0.0f <= angle ? 0x200 : 0x100;
    }
    diff = ((f32 *)entity->target)[1] - entity->model->position[1];
    if (entity->params->unk54 < (0.0f <= diff ? diff : -diff)) {
        result |= 0.0f <= diff ? 0x20 : 0x10;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(entity->model->position), "r"(entity->target), "r"(distance) : "memory");
    if (range < distance[0]) {
        result |= 1;
        if (mark_far != 0) {
            result = (result | 0x40) & 0xFFFF;
        }
    }
    return result;
}

/* The move command towards the target: turn (0x100, 0x200), and forward (1, with 0x40 when `run` is set)
   while it is farther than `range` on the ground. */
s32 EntityAi_MoveCommand(Entity *entity, s32 mark_far, f32 range) {
    s32 result;
    f32 *target;
    f32 angle;
    f32 diff;
    f32 distance[4];

    result = 0;
    target = entity->target;
    if (target == NULL) {
        return 0;
    }
    angle = atan2f(target[0] - entity->model->position[0], target[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    diff = angle - entity->model->unkD0[1];
    diff = diff - (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
    } else if (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (entity->params->turn_limit < (0.0f <= diff ? diff : -diff)) {
        result = 0.0f <= diff ? 0x200 : 0x100;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(entity->model->position), "r"(entity->target), "r"(distance) : "memory");
    if (range < distance[0]) {
        result |= 1;
        if (mark_far != 0) {
            result = (result | 0x40) & 0xFFFF;
        }
    }
    return result;
}

/* As EntityAi_MoveCommand, but moves forward only once it does not have to turn. Nothing calls it. */
s32 EntityAi_TurnOrMoveCommand(Entity *entity, s32 far_flag, f32 reach) {
    u16 result = 0;
    f32 *target_pos;
    f32 angle;
    f32 distance;
    f32 turn_limit;
    f32 vec[4];

    if (entity->target == NULL) {
        return 0;
    }
    target_pos = entity->target;
    angle = atan2f(target_pos[0] - entity->model->position[0], target_pos[2] - entity->model->position[2]);
    angle += 3.1415927f;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    angle -= entity->model->unkD0[1];
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    turn_limit = ENTITY_CONTROLLER(entity)->turn_limit;
    if (0.0f <= angle ? turn_limit < angle : turn_limit < -angle) {
        if (0.0f <= angle) {
            result = 0x200;
        } else {
            result = 0x100;
        }
    } else {
        __asm__ volatile(
            "lqc2 $vf16, 0(%0)\n"
            "lqc2 $vf17, 0(%1)\n"
            "vsub.xyz $vf16, $vf16, $vf17\n"
            "vmul.xz $vf16, $vf16, $vf16\n"
            "vaddz.x $vf16, $vf16, $vf16z\n"
            "vsqrt Q, $vf16x\n"
            "vwaitq\n"
            "vaddq.x $vf16, $vf0, Q\n"
            "sqc2 $vf16, 0(%2)\n"
            : : "r"(entity->model->position), "r"(entity->target), "r"(vec) : "memory");
        distance = vec[0];
        if (reach < distance) {
            result |= 1;
            if (far_flag != 0) {
                result |= 0x40;
            }
        }
    }
    return result;
}

/* The move command for turning aside from a push (`turn` from EntityAi_TurnFromPush): forward with the turn,
   and up or down by which way the push points. */
int EntityAi_PushTurnCommand3D(Entity *entity, int turn) {
    float push[4];
    int code = 0;

    push[0] = push[1] = push[2] = push[3] = 0.0f;
    if (entity->flags & (0xC000L << 31)) {
        push[0] += entity->unkA4[0];
        push[1] += entity->unkA4[1];
        push[2] += entity->unkA4[2];
    }
    if ((s32)(entity->flags >> 48) & 1) {
        push[0] += entity->unkB0[0];
        push[1] += entity->unkB0[1];
        push[2] += entity->unkB0[2];
    }
    Vector_Normalize(push, push);
    if (turn > 0) {
        code = 0x201;
        if (push[1] > 0.0f) {
            code = 0x221;
        } else if (push[1] < 0.0f) {
            code = 0x211;
        }
    } else if (turn < 0) {
        code = 0x101;
        if (push[1] > 0.0f) {
            code = 0x121;
        } else if (push[1] < 0.0f) {
            code = 0x111;
        }
    }
    return code;
}
