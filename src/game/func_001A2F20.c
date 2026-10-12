#include "kf_effect.h"
#include "kf_entity.h"

extern ObjectSlot D_003F1750[];
extern EffectActor D_003F3550[];
extern int D_004468CC;
extern int D_004468D0;

void func_00216C48(void *, void *, void *);

/* Adds an offset to the position of every live slot whose definition is not flagged to skip it. */
void func_001A2F20(void *offset) {
    ObjectSlot *slot = D_003F1750;
    EffectActor *effect = D_003F3550;
    int i;

    if (D_004468CC > 0) {
        for (i = 0; i < 0x60; i++, slot++) {
            if (slot->flags & (0x8000L << 17)) {
                if (!(EFFECT_DEF_FLAGS(OBJECT_SLOT_DEF(slot)) & 0x600000)) {
                    func_00216C48(slot, slot, offset);
                }
            }
        }
    }
    if (D_004468D0 > 0) {
        for (i = 0; i < 0x180; i++, effect++) {
            if (effect->flags & (0x8000L << 41)) {
                if (!(EFFECT_DEF_FLAGS(effect->def) & 0x600000)) {
                    func_00216C48(&effect->model->position, &effect->model->position, offset);
                    func_00216C48(&effect->unk70, &effect->unk70, offset);
                }
            }
        }
    }
}
