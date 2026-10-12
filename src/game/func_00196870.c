#include "kf_entity.h"
#include "kf_player.h"

int rand(void);
s32 func_0018CAF8(Entity *entity);
s32 func_0018C728(Entity *entity);
extern PlayerState D_00413F80;

/* Updates the flag-32 and flag-33 bits from how near the player is, then marks the actor (flag 63) when it is busy. */
void func_00196870(Entity *actor, s32 mode) {
    EntityController *ctrl = ENTITY_CONTROLLER(actor);
    f32 half_height = ctrl->unk1A;
    f32 reach = ctrl->unk18;
    f32 arc = ctrl->unk1C;
    f32 turn;
    f32 turn_abs;
    f32 lower;
    f32 upper;
    f32 near_reach;
    f32 limit;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        arc *= 0.5f;
        half_height *= 0.5f;
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
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
    near_reach = ctrl->unk18;
    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        near_reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        near_reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + near_reach;
    if (actor->flags & ((u64)0x20001 << 32)) {
        actor->flags |= (u64)0x8000 << 18;
    } else if (limit < actor->distance_to_player) {
        actor->flags &= ~((u64)1 << 33);
    } else {
        s32 chance = (s32)((limit - actor->distance_to_player) * 100.0f / limit) * 2;

        if (rand() % 100 < chance) {
            actor->flags |= (u64)0x8000 << 18;
        }
    }
    actor->flags &= ~((u64)1 << 63);
    if (func_0018CAF8(actor) - 1 < 3U || func_0018C728(actor) != 0) {
        actor->flags |= (u64)0x8000 << 48;
        if (mode < 2) {
            actor->flags &= ~((u64)1 << 32);
            if (mode == 0) {
                actor->flags &= ~((u64)1 << 33);
            }
        }
    }
}
