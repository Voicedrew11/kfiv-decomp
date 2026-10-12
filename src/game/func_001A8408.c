#include "common.h"
#include "kf_effect.h"

/* Per-frame update: clears the motion's 0x24 flag and sets pool flag bit 43. */
void func_001A8408(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->unk24 = 0;
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
