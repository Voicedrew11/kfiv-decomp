#include "kf_effect.h"

void func_001ABBE0(EffectActor *actor, EffectSlot *spawn_slot) {
    BurstMotion *motion;

    motion = (BurstMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion->unk1C = 0;
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
