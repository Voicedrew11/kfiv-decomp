#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Reports whether the player is inside the reach of any of the entity's three melee or three ranged attacks. */
s32 func_00197078(Entity *entity) {
    f32 angle_off[6];
    f32 reach[12]; /* nearest per attack, then farthest per attack */
    f32 range;
    f32 top;
    f32 bottom;

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
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[0].unk0E * 0.017453292f;
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

            angle_off[3] = heading;
            magnitude = angle_off[3];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[3] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[1].unk0E * 0.017453292f;
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

            angle_off[4] = heading;
            magnitude = angle_off[4];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[4] = magnitude;
        }
    }
    {
        f32 heading;
        f32 turn;

        turn = entity->params->unk3D8[2].unk0E * 0.017453292f;
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

            angle_off[5] = heading;
            magnitude = angle_off[5];
            if (!(magnitude >= 0.0f)) {
                magnitude = -magnitude;
            }
            angle_off[5] = magnitude;
        }
    }
    top = entity->model->position[1] - (entity->height + entity->params->unk1A) * 0.5f;
    bottom = entity->model->position[1] - (entity->height - entity->params->unk1A) * 0.5f;
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
    if (entity->model->position[1] - entity->height <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= entity->model->position[1]) {
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk00 != 0 && angle_off[0] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 0).unk08 && reach[0] <= entity->distance_to_player && entity->distance_to_player < reach[6]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk00 != 0 && angle_off[1] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 1).unk08 && reach[1] <= entity->distance_to_player && entity->distance_to_player < reach[7]) {
            return 1;
        }
        if (ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk00 != 0 && angle_off[2] < ENTITY_PARAMS_UNK240_RECORD(entity->params, 2).unk08 && reach[2] <= entity->distance_to_player && entity->distance_to_player < reach[8]) {
            return 1;
        }
    }
    if (top <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= bottom) {
        if (entity->params->unk3D8[0].record.unk00 != 0 && angle_off[3] < entity->params->unk3D8[0].unk08 && reach[3] <= entity->distance_to_player && entity->distance_to_player < reach[9]) {
            return 1;
        }
        if (entity->params->unk3D8[1].record.unk00 != 0 && angle_off[4] < entity->params->unk3D8[1].unk08 && reach[4] <= entity->distance_to_player && entity->distance_to_player < reach[10]) {
            return 1;
        }
        if (entity->params->unk3D8[2].record.unk00 != 0 && angle_off[5] < entity->params->unk3D8[2].unk08 && reach[5] <= entity->distance_to_player && entity->distance_to_player < reach[11]) {
            return 1;
        }
    }
    return 0;
}
