#include "common.h"
#include "kf_effect.h"

/* Spawn: starts the velocity from the definition's unkDC and unkE0. */
void func_001AA8E0(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;

    velocity = &effect->motion.velocity;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    velocity->x = effect->def->unkDC;
    velocity->y = effect->def->unkE0;
}
