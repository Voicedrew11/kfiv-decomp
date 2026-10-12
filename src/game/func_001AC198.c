#include "kf_effect.h"

/* Copies the target's unk20 into this actor's motion, after the common update. */
void func_001AC198(EffectActor *effect, EffectSlot *spawn_slot) {
    LinkedMotion *motion;
    LinkedMotion *target_motion;

    target_motion = (LinkedMotion *)&effect->target->motion;
    motion = (LinkedMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->unk20 = target_motion->unk20;
}
