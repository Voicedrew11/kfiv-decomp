#include "kf_effect.h"
#include "kf_entity.h"

extern ObjectSlot D_003F1750[];
extern EffectTableEntry D_00502260[];
int func_001A1C58(int, void *, void *, void *);

/* Allocates an object slot for effect_id at the actor's model and links it to the actor. */
int func_001A2838(int effect_id, EffectActor *effect) {
    int slot_index;
    ObjectSlot *slot;

    slot_index = func_001A1C58(effect_id, &effect->model->position, &effect->model->rotation, (void *)effect->owner_entity);
    if (slot_index != -1) {
        slot = &D_003F1750[slot_index];
        slot->object_id = effect->effect_param;
        slot->unk38 = (s32)effect->owner;
        slot->actor = effect;
        OBJECT_SLOT_FLAG_BITS(slot)->bit33 = EFFECT_FLAG_BITS(effect)->bit59;
        if (D_00502260[effect_id].track) {
            EFFECT_FLAG_BITS(effect)->has_slots = TRUE;
        }
    }
    return slot_index;
}
