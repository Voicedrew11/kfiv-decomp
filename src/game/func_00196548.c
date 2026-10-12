#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;

/* Sets the flag-32 bit when the player is inside the actor's reach: near enough, in front of it and within its height. */
void func_00196548(Entity *actor) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 reach = ctrl->unk18;
    f32 half_height = ctrl->unk1A;
    f32 arc = ctrl->unk1C;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (D_00413F80.unk45 == 0x9C) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    turn = actor->unk68 - actor->model->unkD0[1];
    turn -= (s32)(turn / 6.2831855f) * 6.2831855f;
    if (3.1415927f < turn) {
        turn -= 6.2831855f;
    } else if (turn < -3.1415927f) {
        turn += 6.2831855f;
    }
    turn_abs = 0.0f <= turn ? turn : -turn;
    upper = actor->model->position[1] - (actor->height + half_height) * 0.5f;
    lower = actor->model->position[1] - (actor->height - half_height) * 0.5f;
    ENTITY_FLAG_BITS(actor)->bit32 = 0;
    if (actor->distance_to_player < actor->radius + 100.0f + reach && turn_abs < arc && upper <= D_00413F80.position.y && D_00413F80.position.y - 230.0f <= lower) {
        ENTITY_FLAG_BITS(actor)->bit32 = 1;
    }
}
