#include "kf_entity.h"
#include "kf_player.h"

int rand(void);
extern PlayerState D_00413F80;

/* Sets or clears the flag-33 bit depending on how near the actor is, with some chance. */
void func_00196718(Entity *actor) {
    f32 reach = ENTITY_CONTROLLER(actor)->unk18;
    f32 limit;

    if (actor->unk24 != 0 || (D_00413F80.unk2E4 > 0 && actor->unk60 < D_00413F80.unk2E8)) {
        reach *= 0.5f;
    }
    if (D_00413F80.equipment[7] == 0x9C) {
        reach *= 0.5f;
    }
    limit = actor->radius + 100.0f + reach;
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
}
