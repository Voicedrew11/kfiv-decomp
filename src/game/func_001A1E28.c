#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];

int func_001A1C58(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity);

/* Spawns effect effect_id at position and rotation; returns its slot, or -1 when none is free. */
int func_001A1E28(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity, u16 effect_param, void *owner) {
    int slot_index;
    EffectSlot *slot;

    slot_index = func_001A1C58(effect_id, position, rotation, owner_entity);
    if (slot_index != -1) {
        slot = &D_003F1750[slot_index];
        slot->effect_param = effect_param;
        slot->owner = owner;
    }
    return slot_index;
}
