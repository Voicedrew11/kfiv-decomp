#include "kf_effect.h"

/* Re-seeds the motion from the model and definition, then sets bit 58 of the flags. */
void func_001AE278(EffectActor *actor, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->flags |= (s64)0x8000 << 0x2B;
}
