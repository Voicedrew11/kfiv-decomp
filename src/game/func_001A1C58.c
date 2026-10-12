#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectDef D_00502260[];
extern Vector4 D_00431C80;
extern int D_004468CC;

void func_00216778(void *dst, void *src);

/* Takes a free spawner slot for effect_id at position and rotation; returns its index, or -1. */
int func_001A1C58(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity) {
    EffectSlot unused_slot;
    EffectDef *def;
    EffectSlot *slot;
    int slot_index;
    u64 placement;

    if ((u32)effect_id >= 0x400) {
        return -1;
    }
    def = &D_00502260[effect_id];
    if (def->unkFE >= 0x200) {
        return -1;
    }
    if (D_004468CC >= 0x60) {
        return -1;
    }
    slot = D_003F1750;
    for (slot_index = 0; slot_index < 0x60; slot_index++, slot++) {
        if (!(slot->flags & EFFECT_SLOT_LIVE)) {
            break;
        }
    }
    if (slot_index >= 0x60) {
        return -1;
    }
    slot->flags |= EFFECT_SLOT_LIVE;
    D_004468CC++;
    EFFECT_SLOT_FLAG_PARTS(slot)->level = def->max_level;
    EFFECT_SLOT_FLAG_PARTS(slot)->unk27 = def->unkFD;
    slot->owner_entity = owner_entity;
    EFFECT_SLOT_FLAG_PARTS(slot)->def = def;
    EFFECT_SLOT_FLAG_PARTS(slot)->unk26 = 0;
    slot->owner_effect = NULL;
    slot->unk30 = NULL;
    slot->unk34 = NULL;
    placement = EFFECT_DEF_FLAG_WORD(def) & 0x600000;
    if (placement == 0 || placement == 0x600000) {
        func_00216778(&slot->position, position);
        func_00216778(&slot->unk10, rotation);
        if (EFFECT_DEF_FLAGS(def) & 0x10000) {
            slot->unk30 = position;
            slot->unk34 = rotation;
        }
    } else {
        func_00216778(&slot->position, &D_00431C80);
        func_00216778(&slot->unk10, &D_00431C80);
        if (EFFECT_DEF_FLAGS(def) & 0x10000) {
            slot->unk34 = slot->unk30 = &D_00431C80;
        }
    }
    slot->flags &= ~(EFFECT_SLOT_LIVE << 1);
    slot->effect_param = 0xFFFF;
    return slot_index;
}
