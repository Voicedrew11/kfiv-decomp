#include "common.h"
#include "kf_effect.h"

/* Per-frame update: clears the motion's 0x24 flag and sets pool flag bit 43. */
void func_001A8408(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->unk24 = 0;
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
