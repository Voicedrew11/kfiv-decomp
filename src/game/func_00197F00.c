#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Reports whether the player is inside the reach of any of the entity's three melee attacks. */
s32 func_00197F00(Entity *entity) {
    f32 angle_off[3];
    f32 reach[6]; /* nearest, then farthest, per attack */
    f32 range;
    f32 lower;
    f32 upper;

    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[0] = heading;
            magnitude = angle_off[0];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[0] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[1] = heading;
            magnitude = angle_off[1];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[1] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk0E * 0.017453292f;
        if (!(entity->model->unkE0[0] >= 0.0f)) {
            turn = -turn;
        }
        heading = entity->model->unkD0[1] + turn;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        heading = entity->unk68 - heading;
        heading = heading - (f32)(s32)(heading / 6.2831855f) * 6.2831855f;
        if (3.1415927f < heading) {
            heading -= 6.2831855f;
        } else if (heading < -3.1415927f) {
            heading += 6.2831855f;
        }
        {
            f32 magnitude;

            angle_off[2] = heading;
            magnitude = angle_off[2];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[2] = magnitude;
        }
    }
    lower = D_00413F80.position.y - 230.0f;
    upper = D_00413F80.position.y + entity->height;
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
    reach[3] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk06) + entity->radius + 100.0f;
    reach[4] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk06) + entity->radius + 100.0f;
    reach[5] = (range < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06 ? range : ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk06) + entity->radius + 100.0f;
    if (lower <= entity->model->position[1] && entity->model->position[1] <= upper) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk00 != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk08 && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[3]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk00 != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk08 && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[4]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk00 != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk08 && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[5]) {
            return 1;
        }
    }
    return 0;
}
