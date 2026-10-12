#include "kf_effect.h"
#include "kf_entity.h"

extern ObjectSlot D_003F1750[];
extern EffectTableEntry D_00502260[];
int func_001A1C58(int, void *, void *, void *);

/* Allocates an object slot for effect_id and links it to the actor. */
int func_001A2928(int effect_id, EffectActor *actor, Vector4 *position) {
    int slot_index;
    ObjectSlot *slot;

    slot_index = func_001A1C58(effect_id, position, (char *)actor->model + 0xD0, (void *)actor->owner_entity);
    if (slot_index != -1) {
        slot = &D_003F1750[slot_index];
        slot->object_id = actor->effect_param;
        slot->unk38 = (s32)actor->owner;
        slot->actor = actor;
        OBJECT_SLOT_FLAG_BITS(slot)->bit33 = EFFECT_FLAG_BITS(actor)->bit59;
        if (D_00502260[effect_id].track) {
            EFFECT_FLAG_BITS(actor)->has_slots = TRUE;
        }
    }
    return slot_index;
}
