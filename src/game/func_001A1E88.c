#include "common.h"
#include "kf_effect.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;
extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern EffectDef D_00502260[];
extern void (*D_00402550[][2])();
extern int D_004468CC;

void func_001A24D8(EffectSlot *);
void func_00216778(void *, void *);

/* Runs the spawner slots and the effects for one frame. While the game status has bit 0x400
   set, only those of the owner in unk40 and of the kinds 0x280, 0x282 and 0x284 run. */
void func_001A1E88(void) {
    Vector4 unused[2]; /* unused, but retail reserves the bytes */
    EffectDef *kind_280 = &D_00502260[0x280];
    EffectDef *kind_282 = &D_00502260[0x282];
    EffectDef *kind_284 = &D_00502260[0x284];
    Entity *only_owner = NULL;
    EffectSlot *slot;
    EffectActor *effect;
    int i;
    int key;

    if (D_0038CC50.unk30 & 0x400) {
        only_owner = D_0038CC50.unk40;
    }
    if (D_004468CC > 0) {
        slot = D_003F1750;
        for (i = 0; i < 0x60; i++, slot++) {
            if (slot->flags & EFFECT_SLOT_LIVE) {
                if (only_owner == NULL || only_owner == slot->owner_entity || EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_280 ||
                    EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_282 || EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_284) {
                    func_001A24D8(slot);
                    if (!D_003F1750) { // match: tests the pool's address, which is never null
                        break;
                    }
                }
            }
        }
    }
    effect = D_003F3550;
    for (i = 0; i < 0x180; i++, effect++) {
        if (!(effect->flags & EFFECT_FLAG_LIVE)) {
            if (effect->unk90 > 0) {
                effect->unk90--;
            }
        } else if (only_owner == NULL || only_owner == effect->owner_entity || effect->def == kind_280 ||
                   effect->def == kind_282 || effect->def == kind_284) {
            func_00216778(&effect->unk70, &effect->model->position);
        }
    }
    effect = D_003F3550;
    for (i = 0; i < 0x180; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (only_owner == NULL || only_owner == effect->owner_entity || effect->def == kind_280 || effect->def == kind_282 || effect->def == kind_284) {
                if (EFFECT_FLAG_PARTS(effect)->unk40 > 0) {
                    EFFECT_FLAG_PARTS(effect)->unk40--;
                }
                for (key = 0; key < 3; key++) {
                    if (effect->def->unk112[key] == effect->frame) {
                        func_001A2838(effect->def->unk10C[key], effect);
                    }
                }
                D_00402550[effect->def->unk100][1](effect);
            }
        }
    }
}
