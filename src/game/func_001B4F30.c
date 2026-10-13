#include "common.h"
#include "kf_effect.h"

extern FxTableEntry D_00402DA8[];

int func_001B4F30(u16 effect_id, void *shot, void *from, void *angles, void *entity) {
    FxTableEntry *entry;

    if (effect_id >= 0x100) {
        return 0;
    }
    entry = &D_00402DA8[effect_id];
    if (entry->unk102 != 0) {
        return 0;
    }
    return Effect_Spawn(entry->unk100, from, angles, entity, effect_id, shot);
}
